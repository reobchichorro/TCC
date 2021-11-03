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
#include "ILS.h"

ILS::ILS(std::vector<GuardType>& guard_types, Terrain& dem, Situation& curr) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    this->curr = curr;
    this->best = curr;
    this->it = 0;
}

void ILS::perturb(Situation& s1) {
    int total = curr.allocations.size();
    int num_to_keep = percent_to_keep*total;
    std::unordered_set<int> to_keep;

    for(int i=0; i<total; i++)
        to_keep.insert(i);

    while(to_keep.size() > num_to_keep) {
        to_keep.erase(rand()%total);
    }

    int i=0;
    for(auto it = curr.allocations.begin(); it != curr.allocations.end(); it++, i++) {
        if(to_keep.find(i) != to_keep.end()) {
            GuardPos gPos(*(it->guard), *(it->position), s1.covered);

            long long int numCovered_inc, numTwiceCovered_inc;
            long long int OF_inc = gPos.calculateOF_inc(it->angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);

            NewAlloc toInsert(it->angle, gPos, *(it->position), it->guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc);
            s1.insertNewAlloc(toInsert);
        }
    }
}

void ILS::solve(int i) {
    str toPrint = "";

    long long int a, b;

    Situation s1(*guard_types, *dem);
    perturb(s1);

    Greedy s1g(*guard_types, *dem);
    s1g.solve(s1);

    LS s1Star(*guard_types, *dem);
    s1Star.until_local_opt(s1);

    std::cerr << best.OF << " " << curr.OF << " " << s1.OF << "\n";
        
    if(curr.OF > best.OF)
        best = curr;
    if(s1.OF > best.OF)
        best = s1;

    if(s1.OF > curr.OF) {
        curr = s1;
    } else if(rand()%100 >= 75) {
        curr = s1;
    }

    it++;
}
