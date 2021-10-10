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

#ifndef __NEWALLOC_
#define __NEWALLOC_
class NewAlloc{
public:
    Allocation alloc;
    long long int OF_inc;
    long long int numCovered_inc;
    long long int numTwiceCovered_inc;

    NewAlloc(const int angle, GuardPos& guardPos, const Observer& position, int gidx, long long int OF_inc, const long long int& numCovered_diff, const long long int& numTwiceCovered_diff);
    bool operator<(const NewAlloc& other) const;
    long double operator-(const NewAlloc& other);
    // NewAlloc(const int angle, GuardType& guard_type, const Observer& position, const std::vector<std::vector<bool> >& covered);
};
#endif

#ifndef __SITUATION_
#define __SITUATION_
class Situation {
public:
    Terrain* dem;
    std::vector<GuardType>* guard_types;
    std::vector<std::vector<short int> > covered;
    std::list<Allocation> allocations;
    std::vector<int> guard_amount;
    long long int OF;
    long long int numCovered;
    long long int numTwiceCovered;
    long long int iCost;
    long long int mCost;

    std::list<NewAlloc> possibilities;

    Situation();
    Situation(std::vector<GuardType>& guard_types, Terrain& dem);

    long double calculate_OF();

    bool calculate_possibilities();
    void updateCovered(Allocation& alloc);
    void insertNewAlloc(NewAlloc& newAlloc);

    bool addRandomNewAlloc();

    void updateCovered_Pos(Allocation& alloc, const Observer* oldPos);
    void updateCovered_Guard(Allocation& alloc, const GuardType* oldGuard);
    void updateCovered_Angle(Allocation& alloc, const int oldAngle);
    void replaceAlloc(NewAlloc& newAlloc, std::list<Allocation>::iterator& oldAlloc, int mod);
    void switchPos(std::list<Allocation>::iterator& alloc);
    void switchGuard(std::list<Allocation>::iterator& alloc);
    void switchAngle(std::list<Allocation>::iterator& alloc);
};
#endif
