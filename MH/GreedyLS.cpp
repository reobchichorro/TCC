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
    bool hasGuards = curr.calculate_newPossibilities();
    if(hasGuards) {
        auto bestAlloc = std::min_element(curr.newPossibilities.begin(), curr.newPossibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        curr.insertNewAlloc(*bestAlloc);
    }
    return hasGuards;
}

bool Greedy::advance_if_OF_gt_1(Situation& curr) {
    curr.calculate_newPossibilities();
    auto bestAlloc = std::min_element(curr.newPossibilities.begin(), curr.newPossibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
    
    if(curr.newPossibilities.empty() || bestAlloc->OF_inc < 0.0)
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

void Greedy::insertPosRandomAllocs(Situation& curr) {
    while(true) {
        curr.random_newPossibilities(20);
        auto bestAlloc = std::min_element(curr.newPossibilities.begin(), curr.newPossibilities.end(), [](const NewAlloc& a, const NewAlloc& b){return b < a;});
        
        if(curr.newPossibilities.empty() || bestAlloc->OF_inc < 0.0)
            break;

        curr.insertNewAlloc(*bestAlloc);
    }
}

LS::LS(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
}

void LS::one_neigh_until_localopt(Situation& curr, int neighborhood, int improv) {
    bool isLocalOpt = false;
    while(!isLocalOpt) {
        isLocalOpt = true;
        std::list<SubAlloc>::iterator bestAlloc;
        std::list<Allocation>::iterator oldAlloc;

        if (neighborhood == 0) { //switch the pos of guard
            int i = 0;
            for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
                curr.switchPos(alloc);
            }
            bestAlloc = std::min_element(curr.subPossibilities.begin(), curr.subPossibilities.end(), [](const SubAlloc& a, const SubAlloc& b){return b < a;});
            if(curr.subPossibilities.empty() || bestAlloc->OF_diff <= 0) {
                curr.subPossibilities.clear();
            }
            else {
                isLocalOpt == false;
                curr.replaceAlloc(*bestAlloc, 0);
            }
        } else if (neighborhood == 1) { //switch the guard in pos
            int i = 0;
            for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
                curr.switchGuard(alloc);
            }
            bestAlloc = std::min_element(curr.subPossibilities.begin(), curr.subPossibilities.end(), [](const SubAlloc& a, const SubAlloc& b){return b < a;});
            if(curr.subPossibilities.empty() || bestAlloc->OF_diff <= 0) {
                curr.subPossibilities.clear();
            }
            else {
                isLocalOpt == false;
                curr.replaceAlloc(*bestAlloc, 1);
            }
        } else if (neighborhood == 2) { // switch angle of guard
            int i = 0;
            for(auto alloc = curr.allocations.begin(); alloc != curr.allocations.end(); alloc++, i++) {
                curr.switchAngle(alloc);
            }
            bestAlloc = std::min_element(curr.subPossibilities.begin(), curr.subPossibilities.end(), [](const SubAlloc& a, const SubAlloc& b){return b < a;});
            if(curr.subPossibilities.empty() || bestAlloc->OF_diff <= 0) {
                curr.subPossibilities.clear();
            }
            else {
                isLocalOpt == false;
                curr.replaceAlloc(*bestAlloc, 2);
            }
        }
    }


}

void LS::mix1(Situation& curr) {
    return;
}
