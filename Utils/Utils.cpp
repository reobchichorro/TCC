#include <string>
#include <algorithm>

#include <vector>
#include <unordered_map>
#include <fstream>
#include "Utils.h"

#define all(v) v.begin(),v.end()
#define eps 1e-6

typedef std::string str;

bool DoubleOps::isEqual(const double a, const double b) {
    return isZero(a-b);
}

bool DoubleOps::isZero(const double num) {
    return abs(num) < eps;
}

void StringManipulation::removeFileType(const str& filename, str& outname) {
    outname = filename;
    outname.erase(std::find(all(outname), '.'),outname.end());
}

xyz::xyz() {}

xyz::xyz(const double xx, const double yy, const double zz) {
    x = xx; y = yy; z = zz;
}

double xyz::diff(const xyz& other) {
    xyz diff(other.x - x, other.y - y, other.z - z);

    if (op.isZero(diff.x) || abs(diff.x) > abs(diff.y))
        return abs(diff.y);
    return abs(diff.x);
}

void xyz::take_min(const xyz& other) {
    x = std::min(x, other.x);
    y = std::min(y, other.y);
    z = std::min(z, other.z);
}

void xyz::take_max(const xyz& other) {
    x = std::max(x, other.x);
    y = std::max(y, other.y);
    z = std::max(z, other.z);
}

bool xyz::operator<(const xyz& other) {
    return op.isEqual(x, other.x) ? (y < other.y) : (x < other.x);
}

double xyz::dot(const xyz& other) {
    return x*other.x + y*other.y + z*other.z;
}

ij::ij(int i, int j, bool corner) {
    ii = i;
    jj = j;
    this->corner = corner;
}

double ij::i() const { return ii + corner*0.5; };
double ij::j() const { return jj + corner*0.5; };

int ij::idxi() const { return ii; }
int ij::idxj() const { return jj; }
