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
#include "AllocGPos.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

#ifndef __SUBALLOC_
#define __SUBALLOC_
class SubAlloc{
public:
    Allocation alloc;
    std::list<Allocation>::iterator oldAlloc;
    long long int OF_diff;
    long long int numCovered_diff;
    long long int numTwiceCovered_diff;
    long long int icost_diff;

    SubAlloc(std::list<Allocation>::iterator& oldAlloc, Allocation& newAlloc, const int nrows, const std::vector<std::vector<short int>>& covered);
    bool operator<(const SubAlloc& other) const;
    long double operator-(const SubAlloc& other);
};
#endif
