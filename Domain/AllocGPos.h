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

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

#ifndef __ALLOCATION_
#define __ALLOCATION_
class Allocation {
public:
    const GuardType* guard;
    const Observer* position;
    int angle; //has to be a multiple of 45
    int guardidx;

    Allocation();

    Allocation(const int angle, const GuardType& guard_type, const Observer& position, int gidx);
    Allocation(const int angle, const GuardType* guard_type, const Observer& position, int gidx);
    // Allocation(const int angle, GuardType* guard_type, const int x, const int y);
    Allocation(const Allocation& other);
};
#endif

#ifndef __GUARDPOS__
#define __GUARDPOS__
class GuardPos{

public:
    int x; int y;
    const GuardType* guard;
    std::vector<long long int> sector_covered_points;
    std::vector<long long int> sector_twiceCovered_points;
    std::vector<long long int> sector_old_covered_points;
    std::vector<long long int> sector_old_twiceCovered_points;

    GuardPos(const GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered);
    GuardPos(const GuardType& guard_type, const Observer& oldPosition, const Observer& newPosition, const std::vector<std::vector<short int> >& covered);
    GuardPos(const GuardType& guard_type, const GuardType& newGuard, const Observer& position, const std::vector<std::vector<short int> >& covered);
    GuardPos(const GuardType& guard_type, const int oldAngle, const Observer& position, const std::vector<std::vector<short int> >& covered);

    long long int calculateOF_inc(const int angle, long long int& numCovered_diff, long long int& numTwiceCovered_diff, int nrows, bool hasOld);
    long long int calculateOF_incGuard(const int angle, long long int oldICost, long long int& numCovered_diff, long long int& numTwiceCovered_diff, int nrows, bool hasOld);
    long long int calculateOF_incAngle(const int oldAngle, const int newAngle, long long int& numCovered_diff, long long int& numTwiceCovered_diff, int nrows, bool hasOld);
};
#endif
