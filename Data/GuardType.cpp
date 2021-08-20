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

    void read_file(const std::string& path, const std::string& filename) {
        std::ifstream guardtypefile(path + filename);
        guardtypefile >> name;
        guardtypefile >> height;
        guardtypefile >> radius;
        guardtypefile >> angle;
        guardtypefile >> icost;
        guardtypefile >> mcost;

        guardtypefile >> amount;
    }
};
