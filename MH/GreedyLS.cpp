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
