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
#include "../Domain/Terrain.h"
#include "../Domain/GuardType.h"
#include "../Domain/Situation.h"
#include "GreedyLS.h"
#include "GA.h"

Population::Population(std::vector<GuardType>& guard_types, Terrain& dem, int it) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->individuals = std::vector<Situation>(36, Situation(guard_types, dem));
    popSize = 0;
    this->it = it;
}

void Population::addIndividual(const Situation& indi) {
    this->individuals[popSize] = indi;
    popSize++;
}

void Population::buildIndividual() {
    individuals[popSize] = Situation(*guard_types, *dem);
    for(auto& pos: dem->best_observers) {
        if(rand()%100 >= 5) continue;
        int gidx = rand()%guard_types->size();
        int angle = 0;
        if(guard_types->at(gidx).angle != 360)
            angle = 45*(rand()%8);
        
        GuardPos possibility(guard_types->at(gidx), pos, individuals[popSize].covered);

        long long int numCovered_inc, numTwiceCovered_inc;
        long long int OF_inc = possibility.calculateOF_inc(angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);

        NewAlloc toAdd(angle, possibility, pos, gidx, OF_inc, numCovered_inc, numTwiceCovered_inc);
        individuals[popSize].insertNewAlloc(toAdd);
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

    std::unordered_set<const Allocation*> parentAllocs;

    for(auto& alloc : dad.allocations)
        parentAllocs.insert(&alloc);
    for(auto& alloc : mom.allocations)
        parentAllocs.insert(&alloc);

    long long int numCovered_inc, numTwiceCovered_inc, OF_inc;

    while(parentAllocs.size() > 0) {
        const Allocation* bestParentAlloc = NULL;
        long long int bestOF_inc;

        for(auto& ptr_alloc: parentAllocs) {
            GuardPos possibility(*(ptr_alloc->guard), *(ptr_alloc->position), child.covered);
            OF_inc = possibility.calculateOF_inc(ptr_alloc->angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
            child.newPossibilities.push_back(NewAlloc(ptr_alloc->angle, possibility, *(ptr_alloc->position), ptr_alloc->guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));

            if(!bestParentAlloc) {
                bestParentAlloc = ptr_alloc;
                bestOF_inc = child.newPossibilities.back().OF_inc;
            } else if(child.newPossibilities.back().OF_inc > bestOF_inc) {
                bestParentAlloc = ptr_alloc;
                bestOF_inc = child.newPossibilities.back().OF_inc;
            }
        }

        auto bestAlloc = std::min_element(child.newPossibilities.begin(), child.newPossibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        if(bestOF_inc == bestAlloc->OF_inc && (bestAlloc->OF_inc > 0 || rand()%1000 < 1000*exp(bestAlloc->OF_inc/temp()))) {
            child.insertNewAlloc(*bestAlloc);
        }
        parentAllocs.erase(bestParentAlloc);
    }
    std::cerr << child.allocations.size() << " " << child.OF << "; ";
}

float Population::temp() {
    return 100000*exp(-it);
}

void Population::mutate(Situation& child) {
    int mut = rand()%100;
    int c1 = 30;
    int c2 = c1 + 2*it;
    int c3 = c2 + 10-it;
    if(mut<c1) {
        int i=0;
        for(auto alloc = child.allocations.begin(); alloc != child.allocations.end(); alloc++, i++) {
            child.checkRemoveAlloc(alloc);
            if(child.subPossibilities.crbegin()->OF_diff > 0) {
                child.removeAlloc(child.subPossibilities.back());
                break;
            }
        }
    } else if(mut<c2) {
        child.random_newPossibilities(10+it);
        auto bestAlloc = std::min_element(child.newPossibilities.begin(), child.newPossibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        
        if(!child.newPossibilities.empty() && bestAlloc->OF_inc >= 0.0)
            child.insertNewAlloc(*bestAlloc);
    } else if(mut<c3) {
        std::list<Allocation>::iterator alloc = child.allocations.begin();
        for(; alloc != child.allocations.end(); alloc++) {
            child.checkRemoveAlloc(alloc);
            if(child.subPossibilities.crbegin()->OF_diff > 0) {
                alloc--;
                child.removeAlloc(child.subPossibilities.back());
                break;
            }
        }
    }
}

void Population::crossover(const Population& oldGen) {
    for(int i=0; i<individuals.size(); i++) {
        reproduce(individuals[i], oldGen.individuals[i], oldGen.individuals[individuals.size()-1-i]);
        mutate(individuals[i]);
        popSize++;
    }    
    std::cerr << "\n";
}

GA::GA(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->it = 0;
    this->curr = new Population(guard_types, dem, it);
    this->curr->generatePop();
    this->best = *std::min_element(curr->individuals.begin(), curr->individuals.end(), [](const Situation& a, const Situation& b){return b.OF < a.OF;});
    this->children = NULL;
    this->improved = NULL;
}

void GA::createNewGeneration() {
    this->children = new Population(*guard_types, *dem, it);
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

    this->improved = new Population(*guard_types, *dem, it);
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
    it++;
}
