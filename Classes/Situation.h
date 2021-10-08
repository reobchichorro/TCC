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
    std::vector<long long int> sector_old_covered_points;
    GuardPos(const GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered);
    GuardPos(const GuardType& guard_type, const Observer& oldPosition, const Observer& newPosition, const std::vector<std::vector<short int> >& covered);
};
#endif

#ifndef __NEWALLOC_
#define __NEWALLOC_
class NewAlloc{
public:
    Allocation alloc;
    long long int numCovered_inc;
    long double OF_inc;

    NewAlloc(const int angle, GuardPos& guardPos, const Observer& position, const std::vector<std::vector<short int> >& covered, int gidx);
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
    long double OF;
    long long int numCovered;
    long long int iCost;
    long long int mCost;

    std::list<NewAlloc> possibilities;

    Situation();
    Situation(std::vector<GuardType>& guard_types, Terrain& dem);

    long double calculate_OF();

    bool calculate_possibilities();
    void updateCovered(Allocation& alloc);
    void insertNewAlloc(NewAlloc& newAlloc);

    void updateCovered(Allocation& alloc, const Observer* oldPos);
    void replaceAlloc(NewAlloc& newAlloc, std::list<Allocation>::iterator& oldAlloc);
    void switchPos(std::list<Allocation>::iterator& alloc);
};
#endif
