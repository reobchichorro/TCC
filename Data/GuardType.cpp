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

// class xyz {

// public:
//     double x;
//     double y;
//     double z;
//     DoubleOps op;

//     xyz() {}

//     xyz(const double xx, const double yy, const double zz) {
//         x = xx; y = yy; z = zz;
//     }

//     double diff(const xyz& other) {
//         xyz diff(other.x - x, other.y - y, other.z - z);

//         if (op.isZero(diff.x) || abs(diff.x) > abs(diff.y))
//             return abs(diff.y);
//         return abs(diff.x);
//     }

//     void take_min(const xyz& other) {
//         x = std::min(x, other.x);
//         y = std::min(y, other.y);
//         z = std::min(z, other.z);
//     }

//     void take_max(const xyz& other) {
//         x = std::max(x, other.x);
//         y = std::max(y, other.y);
//         z = std::max(z, other.z);
//     }

//     bool operator<(const xyz& other) {
//         return op.isEqual(x, other.x) ? (y < other.y) : (x < other.x);
//     }
// };

class GuardType {

public:
    std::string name;

    double height;
    double radius;
    double angle;
    int icost;
    double mcost;

    int amount;

    GuardType() {
        name = "";

        height = 0;
        radius = 0;
        angle = 0;
        icost = 0;
        mcost = 0;

        amount = 0;
    }

    GuardType(const std::string& name, const double height, const double radius, const double angle, const int icost, const double mcost, const int amount) {
        this->name = name;

        this->height = height;
        this->radius = radius;
        this->angle = angle;
        this->icost = icost;
        this->mcost = mcost;

        this->amount = amount;
    }

    
};
