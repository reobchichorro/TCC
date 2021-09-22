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

private:
    GuardType* guard;

public:
    int x; int y;
    int angle; //has to be a multiple of 45

    Allocation();

    Allocation(const int angle, GuardType* guard_type, const int x, const int y);
    Allocation(const int angle, GuardType& guard_type, const Observer& position);
};
#endif

#ifndef __GUARDPOS__
#define __GUARDPOS__
class GuardPos{

public:
    int x; int y;
    GuardType* guard;
    std::vector<long long int> sector_covered_points;
    GuardPos(GuardType& guard_type, const Observer& position, const std::vector<std::vector<bool> >& covered);
};
#endif

#ifndef __NEWALLOC_
#define __NEWALLOC_
class NewAlloc{

private:
    Allocation alloc;
    // long double OF_inc;
    long long int OF_inc;

public:
    NewAlloc(const int angle, GuardPos& guardPos, const std::vector<std::vector<bool> >& covered);
    bool operator<(const NewAlloc& other) const;
    long long int operator-(const NewAlloc& other);
    // NewAlloc(const int angle, GuardType& guard_type, const Observer& position, const std::vector<std::vector<bool> >& covered);
};
#endif

#ifndef __SITUATION_
#define __SITUATION_
class Situation {

private:
    Terrain* dem;
    std::vector<std::vector<bool> > covered;
    long double OF;
    std::list<Allocation> allocations;

public:
    std::list<NewAlloc> possibilities;
    Situation(Terrain& dem);

    long double calculate_OF();

    void calculate_possibilities(std::vector<GuardType>& guard_types);
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
