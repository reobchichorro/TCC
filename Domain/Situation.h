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
#include "SubAlloc.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

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

    std::list<NewAlloc> newPossibilities;
    std::list<SubAlloc> subPossibilities;

    Situation();
    Situation(std::vector<GuardType>& guard_types, Terrain& dem);

    long double calculate_OF();
    void print(str& toPrint);

    bool calculate_newPossibilities();
    void updateCovered(Allocation& alloc);
    void insertNewAlloc(NewAlloc& newAlloc);
    void switchSubAlloc(SubAlloc& subAlloc);

    bool addRandomNewAlloc();

    void updateCovered_Pos(Allocation& alloc, const Observer* oldPos);
    void updateCovered_Guard(Allocation& alloc, const GuardType* oldGuard);
    void updateCovered_Angle(Allocation& alloc, const int oldAngle);
    void replaceAlloc(SubAlloc& subAlloc, int mod);
    void switchPos(std::list<Allocation>::iterator& alloc);
    void switchGuard(std::list<Allocation>::iterator& alloc);
    void switchAngle(std::list<Allocation>::iterator& alloc);
};
#endif
