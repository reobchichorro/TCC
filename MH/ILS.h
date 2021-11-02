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

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

#ifndef __ILS_
#define __ILS_
class ILS {
public:
    std::vector<GuardType>* guard_types;
    double percent_to_keep = 0.50;
    Terrain* dem;
    Situation curr;
    Situation best;

    ILS(std::vector<GuardType>& guard_types, Terrain& dem, Situation& curr);

    void perturb(Situation& s1);
    void solve(int i);
};
#endif
