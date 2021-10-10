#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <fstream>
#include <math.h>
#include <filesystem>
#include "../Utils/Utils.h" 
#include "Terrain.h"
#include "GuardType.h"
#include "Situation.h"
#include "Solver.h"

Greedy::Greedy(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
}

bool Greedy::advance(Situation& curr) {
    bool hasGuards = curr.calculate_possibilities();
    if(hasGuards) {
        auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        // curr.possibilities.sort([](const NewAlloc& a, const NewAlloc& b){return b < a;});
        curr.insertNewAlloc(*bestAlloc);
    }
    return hasGuards;
}

bool Greedy::advance_if_OF_gt_1(Situation& curr) {
    curr.calculate_possibilities();
    auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
    
    if(curr.possibilities.empty() || bestAlloc->OF_inc < 0.0)
        return false;

    curr.insertNewAlloc(*bestAlloc);
    return true;
}

void Greedy::until100(Situation& curr) {
    int nrows = curr.dem->nrows;
    while(curr.numCovered < nrows*nrows) {
        advance(curr);
    }
}

void Greedy::spendAllGuards(Situation& curr) {
    int nrows = curr.dem->nrows;
    while(advance(curr));
}

void Greedy::solve(Situation& curr) {
    while(advance_if_OF_gt_1(curr));
}

LS::LS(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
}

void LS::only_one_neigh(Situation& curr, int neighborhood, int improv) {
    if (neighborhood == 0) { //switch the pos of guard
        int i = 0;
        for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
            curr.switchPos(alloc);
            auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(curr.possibilities.empty() || bestAlloc->OF_inc <= 0) {
                curr.possibilities.clear();
            }
            else {
                curr.replaceAlloc(*bestAlloc, alloc, 0);
            }
        }
    } else if (neighborhood == 1) { //switch the guard in pos
        int i = 0;
        for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
            curr.switchGuard(alloc);
            auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(curr.possibilities.empty() || bestAlloc->OF_inc <= 0) {
                curr.possibilities.clear();
            }
            else {
                curr.replaceAlloc(*bestAlloc, alloc, 1);
            }
        }
    } else if (neighborhood == 2) { // switch angle of guard
        int i = 0;
        for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
            curr.switchAngle(alloc);
            auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(curr.possibilities.empty() || bestAlloc->OF_inc <= 0) {
                curr.possibilities.clear();
            }
            else {
                curr.replaceAlloc(*bestAlloc, alloc, 2);
            }
        }
    }
}

void LS::mix1(Situation& curr) {
    return;
}


ILS::ILS(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
}

void ILS::perturb(Situation& sStar, Situation& s1) {
    double percent_to_keep = 0.75;
    int total = sStar.allocations.size();
    int num_to_keep = percent_to_keep*total;
    std::unordered_set<int> to_keep;

    for(int i=0; i<total; i++)
        to_keep.insert(i);

    while(to_keep.size() > num_to_keep) {
        to_keep.erase(rand()%total);
    }

    int i=0;
    for(auto it = sStar.allocations.begin(); it != sStar.allocations.end(); it++, i++) {
        if(to_keep.find(i) != to_keep.end()) {
            GuardPos gPos(*(it->guard), *(it->position), s1.covered);

            long long int numCovered_inc, numTwiceCovered_inc;
            long long int OF_inc = gPos.calculateOF_inc(it->angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);

            NewAlloc toInsert(it->angle, gPos, *(it->position), it->guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc);
            s1.insertNewAlloc(toInsert);
        }
    }
}

void ILS::solve(Situation& curr) {
    Greedy s0(*guard_types, *dem);
    s0.solve(curr);
    std::cout << curr.calculate_OF() << "s0\n";
    LS sStar(*guard_types, *dem);
    sStar.only_one_neigh(curr, 1, 0);
    std::cout << curr.calculate_OF() << "sStar\n";

    Situation s1(*guard_types, *dem);
    perturb(curr, s1);
    std::cout << s1.calculate_OF() << "s1\n";
    Greedy s1g(*guard_types, *dem);
    s1g.solve(s1);
    std::cout << s1.calculate_OF() << "s1g\n";
    LS s1Star(*guard_types, *dem);
    s1Star.only_one_neigh(s1, 2, 0);
    std::cout << s1.calculate_OF() << "s1Star\n";
    if(s1.OF > curr.OF) {
        curr = s1;
    }

    for(int i=0; i<2; i++) {
        s1 = Situation(*guard_types, *dem);
        perturb(curr, s1);
        std::cout << s1.calculate_OF() << "s1i\n";
        s1g.solve(s1);
        std::cout << s1.calculate_OF() << "s1gi\n";
        s1Star.only_one_neigh(s1, i, 0);
        std::cout << s1.calculate_OF() << "s1Stari\n";
        if(s1.OF > curr.OF) {
            curr = s1;
        }
    }

}

Population::Population(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->individuals = std::vector<Situation>(36, Situation(guard_types, dem));
    popSize = 0;
}

void Population::addIndividual(const Situation& indi) {
    this->individuals[popSize] = indi;
    popSize++;
}

void Population::buildIndividual() {
    // individuals[popSize] = Situation(*guard_types, *dem);
    for(int i=0; i<25; i++) {
        individuals[popSize].addRandomNewAlloc();
    }
    popSize++;
}

void Population::generatePop() {
    while(popSize < this->individuals.size()) {
        buildIndividual();
    }
}

void Population::fillCovered() {
    for(auto& individual: this->individuals) {
        individual.covered = std::vector<std::vector<short int> >(dem->nrows, std::vector<short int>(dem->nrows, 0));
    }
}

void Population::reproduce(Situation& child, const Situation& dad, const Situation& mom) {
    int dadRemaining = dad.allocations.size();
    int momRemaining = mom.allocations.size();
    std::vector<bool> dadPicked(dadRemaining, false);
    std::vector<bool> momPicked(momRemaining, false);
    std::vector<bool> originParent;
    std::vector<int> idxParent;
    int i=0, j=0, k=0;
    double best;
    originParent.reserve(dadRemaining+momRemaining);
    idxParent.reserve(dadRemaining+momRemaining);

    while((dadRemaining > 0 || momRemaining > 0) && child.allocations.size() < dadPicked.size()) {
        i=0;
        for(auto& alloc : dad.allocations) {
            if(dadPicked[i]) {
                i++;
                continue;   
            }

            GuardPos possibility(*alloc.guard, *alloc.position, child.covered);
            long long int numCovered_inc, numTwiceCovered_inc;
            long long int OF_inc = possibility.calculateOF_inc(alloc.angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);

            child.possibilities.push_back(NewAlloc(alloc.angle, possibility, *alloc.position, alloc.guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));
            originParent.push_back(false);
            idxParent.push_back(i);
            i++;
        }

        j=0;
        for(auto& alloc : mom.allocations) {
            if(momPicked[j]) {
                j++;
                continue;   
            }

            GuardPos possibility(*alloc.guard, *alloc.position, child.covered);
            long long int numCovered_inc, numTwiceCovered_inc;
            long long int OF_inc = possibility.calculateOF_inc(alloc.angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);

            child.possibilities.push_back(NewAlloc(alloc.angle, possibility, *alloc.position, alloc.guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));
            originParent.push_back(true);
            idxParent.push_back(j);
            j++;
        }

        auto bestAlloc = std::min_element(child.possibilities.begin(), child.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});

        k=0;
        for(auto& newAlloc : child.possibilities) {
            if(newAlloc.OF_inc == bestAlloc->OF_inc) {
                break;
            }
            k++;
        }
        if(originParent[k]) {
            momPicked[idxParent[k]] = true;
            momRemaining--;
        } else {
            dadPicked[idxParent[k]] = true;
            dadRemaining--;
        }
        child.insertNewAlloc(*bestAlloc);
        originParent.clear();
        idxParent.clear();
    }
}

void Population::mutate(Situation& child) {
    int mut = rand()%100;
    if(mut >= 30) return;
    if(mut < 10) { // switch pos of guard
        int i = 0;
        for(auto alloc = child.allocations.begin(); alloc != child.allocations.end(); alloc++, i++) {
            child.switchPos(alloc);
            auto bestAlloc = std::min_element(child.possibilities.begin(), child.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(child.possibilities.empty() || bestAlloc->OF_inc <= 0) {
                child.possibilities.clear();
            }
            else {
                child.replaceAlloc(*bestAlloc, alloc, 0);
            }
        }
    } else if(mut < 20) { // switch guard in pos
        int i = 0;
        for(auto alloc = child.allocations.begin(); alloc != child.allocations.end(); alloc++, i++) {
            child.switchGuard(alloc);
            auto bestAlloc = std::min_element(child.possibilities.begin(), child.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(child.possibilities.empty() || bestAlloc->OF_inc <= 0) {
                child.possibilities.clear();
            }
            else {
                child.replaceAlloc(*bestAlloc, alloc, 1);
            }
        }
    } else { // switch angle of guard
        int i = 0;
        for(auto alloc = child.allocations.begin(); alloc != child.allocations.end(); alloc++, i++) {
            child.switchAngle(alloc);
            auto bestAlloc = std::min_element(child.possibilities.begin(), child.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(child.possibilities.empty() || bestAlloc->OF_inc <= 0) {
                child.possibilities.clear();
            }
            else {
                child.replaceAlloc(*bestAlloc, alloc, 2);
            }
        }
    }
}

void Population::crossover(const Population& oldGen) {
    // std::vector<int> bestOld(oldGen.popSize);
    // for(int i=0; i<bestOld.size(); i++)
    //     bestOld[i] = i;
    // sort(all(bestOld), [&](int a, int b){ return oldGen.individuals[a].OF < oldGen.individuals[b].OF; });

    // std::vector<
    for(int i=0; i<individuals.size(); i++) {
        reproduce(individuals[i], oldGen.individuals[i], oldGen.individuals[individuals.size()-1-i]);
        mutate(individuals[i]);
        popSize++;
    }    
}

GA::GA(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->curr = new Population(guard_types, dem);
    this->curr->generatePop();
    this->best = *std::min_element(curr->individuals.begin(), curr->individuals.end(), [](const Situation& a, const Situation& b){return b.OF < a.OF;});
    this->children = NULL;
    this->improved = NULL;
}

void GA::createNewGeneration() {
    this->children = new Population(*guard_types, *dem);
    // this->children->fillCovered();
    this->children->crossover(*(this->curr));

    std::vector<int> bestOld(this->curr->popSize);
    std::vector<int> bestNew(this->children->popSize);
    for(int i=0; i<bestOld.size(); i++) {
        bestOld[i] = i;
        bestNew[i] = i;
    }
    sort(all(bestOld), [&](int a, int b){ return this->curr->individuals[a].OF > this->curr->individuals[b].OF; });
    sort(all(bestNew), [&](int a, int b){ return this->children->individuals[a].OF > this->children->individuals[b].OF; });
    if(this->curr->individuals[bestOld[0]].OF < this->children->individuals[bestNew[0]].OF) {
        this->best = this->children->individuals[bestNew[0]];
    }

    this->improved = new Population(*guard_types, *dem);
    int i=0, j=0;
    long double oldOF, newOF;
    while(this->improved->popSize < 36) {
        oldOF = this->curr->individuals[bestOld[i]].OF;
        newOF = this->children->individuals[bestNew[j]].OF;
        if(oldOF > newOF) {
            this->improved->addIndividual(this->curr->individuals[bestOld[i]]);
            i++;
        } else {
            this->improved->addIndividual(this->children->individuals[bestNew[j]]);
            j++;
        }
    }

    delete this->curr;
    delete this->children;
    this->curr = this->improved;
    this->improved = NULL;
}
