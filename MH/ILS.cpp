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

void ILS::solve(Situation& curr, int i) {
    str toPrint = "";

    long long int a, b;

    Situation s1(*guard_types, *dem);
    perturb(curr, s1);

    a = s1.OF; b = s1.calculate_OF();
    if(a != b)
        std::cout << a << " " << b << "s1\n";

    Greedy s1g(*guard_types, *dem);
    s1g.solve(s1);

    a = s1.OF; b = s1.calculate_OF();
    if(a != b)
        std::cout << a << " " << b << "s1\n";

    LS s1Star(*guard_types, *dem);
    s1Star.only_one_neigh(s1, i, 0);

    a = s1.OF; b = s1.calculate_OF();
    if(a != b)
        std::cout << a << " " << b << "s1\n";
        
    if(s1.OF > curr.OF) {
        curr = s1;
    }

}
