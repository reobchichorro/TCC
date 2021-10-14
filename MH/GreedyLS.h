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

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

#ifndef __GREEDY_
#define __GREEDY_
class Greedy {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;


    Greedy(std::vector<GuardType>& guard_types, Terrain& dem);
    
    bool advance(Situation& curr);
    bool advance_if_OF_gt_1(Situation& curr);
    void until100(Situation& curr);
    void spendAllGuards(Situation& curr);
    void solve(Situation& curr);
};
#endif

#ifndef __LS_
#define __LS_
class LS {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;


    LS(std::vector<GuardType>& guard_types, Terrain& dem);
    
    void only_one_neigh(Situation& curr, int neighborhood, int improv);
    void mix1(Situation& curr);
};
#endif
