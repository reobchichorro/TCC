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
    GuardType* guard;
    const Observer* position;
    int angle; //has to be a multiple of 45

    Allocation();

    Allocation(const int angle, GuardType& guard_type, const Observer& position);
    Allocation(const int angle, GuardType* guard_type, const Observer& position);
    // Allocation(const int angle, GuardType* guard_type, const int x, const int y);
    Allocation(const Allocation& other);
};
#endif

#ifndef __GUARDPOS__
#define __GUARDPOS__
class GuardPos{

public:
    int x; int y;
    GuardType* guard;
    std::vector<long long int> sector_covered_points;
    std::vector<long long int> sector_uncovered_points;
    GuardPos(GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered);
    GuardPos(GuardType& guard_type, const Observer& oldPosition, const Observer& newPosition, const std::vector<std::vector<short int> >& covered);
};
#endif

#ifndef __NEWALLOC_
#define __NEWALLOC_
class NewAlloc{
public:
    Allocation alloc;
    long long int numCovered_inc;
    long double OF_inc;

    NewAlloc(const int angle, GuardPos& guardPos, const Observer& position, const std::vector<std::vector<short int> >& covered);
    bool operator<(const NewAlloc& other) const;
    long double operator-(const NewAlloc& other);
    // NewAlloc(const int angle, GuardType& guard_type, const Observer& position, const std::vector<std::vector<bool> >& covered);
};
#endif

#ifndef __SITUATION_
#define __SITUATION_
class Situation {
public:
    std::vector<std::vector<short int> > covered;
    std::list<Allocation> allocations;
    Terrain* dem;
    std::vector<GuardType>* guard_types;
    long double OF;
    long long int numCovered;
    long long int iCost;
    long long int mCost;

    std::list<NewAlloc> possibilities;
    Situation(std::vector<GuardType>& guard_types, Terrain& dem);

    long double calculate_OF();

    bool calculate_possibilities(std::vector<GuardType>& guard_types);
    void updateCovered(Allocation& alloc);
    void insertNewAlloc(NewAlloc& newAlloc);

    void updateCovered(Allocation& alloc, const Observer* oldPos);
    void replaceAlloc(NewAlloc& newAlloc, std::list<Allocation>::iterator& oldAlloc);
    void switchPos(std::list<Allocation>::iterator& alloc);
};
#endif

#ifndef __INTERSECTION_
#define __INTERSECTION_
class Intersection {
public:
    std::vector<ij> squares_to_check;
    std::vector<std::vector<std::vector<std::vector<double> > > > view_angle;

    int orientation(const ij& p, const ij& q, const ij& r);

    bool checkIntersection(const ij& as, const ij& af, const ij& bs, const ij& bf);

    bool check_line_crosses_square(const ij& s, const ij& e, const ij& sq);

    void fill_squares_to_check(const ij& s, const ij& e);

    bool check_if_square_blocks_view(const ij& s, const ij& e, const ij& sq, const Terrain& dem);

    bool check_if_s_views_e(const ij& s, const ij& e, const Terrain& dem);

    double calculate_view_angle(const ij& s, const ij& e, const Terrain& dem);

    void fill_view_angle(const Terrain& dem);
};
#endif
