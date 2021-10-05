#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
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
    bool hasGuards = curr.calculate_possibilities(*guard_types);
    if(hasGuards) {
        auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        // curr.possibilities.sort([](const NewAlloc& a, const NewAlloc& b){return b < a;});
        curr.insertNewAlloc(*bestAlloc);
    }
    return hasGuards;
}

bool Greedy::advance_if_OF_gt_1(Situation& curr) {
    curr.calculate_possibilities(*guard_types);
    auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
    
    if(bestAlloc->OF_inc < 0.3)
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
            if(bestAlloc->OF_inc < 0.0) {
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

void ILS::solve(Situation& curr) {
    Greedy s0(*guard_types, *dem);
    s0.solve(curr);
    LS sStar(*guard_types, *dem);
    sStar.only_one_neigh(curr, 1, 0);
}

bool ILS::advance(Situation& curr) {
    bool hasGuards = curr.calculate_possibilities(*guard_types);
    if(hasGuards) {
        auto bestAlloc = std::min_element(curr.possibilities.begin(), curr.possibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        // curr.possibilities.sort([](const NewAlloc& a, const NewAlloc& b){return b < a;});
        curr.insertNewAlloc(*bestAlloc);
    }
    return hasGuards;
}

void ILS::until100(Situation& curr) {
    int nrows = curr.dem->nrows;
    while(curr.numCovered < nrows*nrows) {
        advance(curr);
    }
}

void ILS::spendAllGuards(Situation& curr) {
    int nrows = curr.dem->nrows;
    while(advance(curr));
}
