#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
#include <math.h>
#include <filesystem>
#include "../Utils/Utils.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()

typedef std::string str;
typedef std::pair<int, int> pii;

#ifndef __OBSERVER_
#define __OBSERVER_
class Observer {

public:
    int x; int y;

    std::unordered_map<int, std::vector<std::vector<bool> > > shed;

    std::map<int, std::vector<pii> > limits_row;
    std::map<int, std::vector<pii> > limits_col;

    Observer(str& point_name, const str& roi_path, const int nrows, const std::set<int>& guard_radii, const std::set<int>& guard_heights);
};
#endif

#ifndef __TERRAIN_
#define __TERRAIN_
class Terrain {

public:
    str name;
    str type;
    str category;

    std::vector<std::vector<int> > height;
    std::vector<Observer> best_observers;

    int resolution = 20;
    int nrows;
    int elsize = 2;

    DoubleOps op;

    Terrain();

    void test();

    void read_file(const str& site_folder, const str& test_case_name);

    void fill_best_observers(const str& site_folder, const str& test_case_name, const str& roi_folder, const std::set<int>& guard_radii, const std::set<int>& guard_heights);
};
#endif
