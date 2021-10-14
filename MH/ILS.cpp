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
