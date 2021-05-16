#include <string>
#include <algorithm>

#include <vector>
#include <unordered_map>
#include <fstream>

#define all(v) v.begin(),v.end()
#define eps 1e-6

class DoubleOps {

public:

    bool isEqual(const double a, const double b) {
        return isZero(a-b);
    }

    bool isZero(const double num) {
        return abs(num) < eps;
    }
};

class StringManipulation {

public:

    void removeFileType(const std::string& filename, std::string& outname) {
        outname = filename;
        outname.erase(std::find(all(outname), '.'),outname.end());
    }
};
