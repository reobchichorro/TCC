#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
#include "GuardType.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

GuardType::GuardType() {
    name = "";

    height = 0;
    radius = 0;
    angle = 0;
    icost = 0;
    mcost = 0;

    amount = 0;
}

GuardType::GuardType(const str& name, const double height, const double radius, const double angle, const int icost, const double mcost, const int amount) {
    this->name = name;

    this->height = height;
    this->radius = radius;
    this->angle = angle;
    this->icost = icost;
    this->mcost = mcost;

    this->amount = amount;
}

void GuardType::read_file(const str& path, const str& filename) {
    std::ifstream guardtypefile(path + filename);
    guardtypefile >> name;
    guardtypefile >> height;
    guardtypefile >> radius;
    guardtypefile >> angle;
    guardtypefile >> icost;
    guardtypefile >> mcost;

    guardtypefile >> amount;
}

void GuardType::adjustICost(int nrows) {
    icost = 5*nrows*nrows*icost/100000;
}
