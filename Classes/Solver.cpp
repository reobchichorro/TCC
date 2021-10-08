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
    if (neighborhood == 0) { //switch the guard in pos, requires available guard
        return;
    } else if (neighborhood == 1) { //switch the pos of guard
        int i = 0;
        for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
            curr.switchPos(alloc);
            auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
            if(bestAlloc->OF_inc <= 0.0) {
                curr.possibilities.clear();
            }
            else {
                curr.replaceAlloc(*bestAlloc, alloc);
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
            NewAlloc toInsert(it->angle, gPos, *(it->position), s1.covered, it->guardidx);
            s1.insertNewAlloc(toInsert);
        }
    }
}

void ILS::solve(Situation& curr) {
    Greedy s0(*guard_types, *dem);
    s0.solve(curr);
    LS sStar(*guard_types, *dem);
    sStar.only_one_neigh(curr, 1, 0);

    Situation s1(*guard_types, *dem);
    perturb(curr, s1);
    Greedy s1g(*guard_types, *dem);
    s1g.solve(s1);
    LS s1Star(*guard_types, *dem);
    s1Star.only_one_neigh(s1, 1, 0);
    if(s1.OF > curr.OF) {
        curr = s1;
    }

    for(int i=0; i<2; i++) {
        s1 = Situation(*guard_types, *dem);
        perturb(curr, s1);
        s1g.solve(s1);
        s1Star.only_one_neigh(s1, 1, 0);
        if(s1.OF > curr.OF) {
            curr = s1;
        }
    }

}

Population::Population(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->individuals = std::list<Situation>();
}

GA::GA(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->curr = NULL;
    this->improved = NULL;
    this->children = NULL;
    this->best = Situation();
}
