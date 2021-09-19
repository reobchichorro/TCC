#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
// #include "../Utils/Utils.cpp"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

#ifndef __GUARDTYPE_
#define __GUARDTYPE_
class GuardType {

public:
    str name;

    int height;
    int radius;
    int angle;
    int icost;
    int mcost;

    int amount;

    GuardType();

    GuardType(const str& name, const double height, const double radius, const double angle, const int icost, const double mcost, const int amount);

    void read_file(const str& path, const str& filename);
};
#endif
