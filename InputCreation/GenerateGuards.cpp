#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
#include "../Utils/Utils.cpp"
#include "../Data/GuardType.cpp"

#define eps 1e-6
#define all(v) v.begin(),v.end()

class xyz {

public:
    double x;
    double y;
    double z;
    DoubleOps op;

    xyz() {}

    xyz(const double xx, const double yy, const double zz) {
        x = xx; y = yy; z = zz;
    }

    double diff(const xyz& other) {
        xyz diff(other.x - x, other.y - y, other.z - z);

        if (op.isZero(diff.x) || abs(diff.x) > abs(diff.y))
            return abs(diff.y);
        return abs(diff.x);
    }

    void take_min(const xyz& other) {
        x = std::min(x, other.x);
        y = std::min(y, other.y);
        z = std::min(z, other.z);
    }

    void take_max(const xyz& other) {
        x = std::max(x, other.x);
        y = std::max(y, other.y);
        z = std::max(z, other.z);
    }

    bool operator<(const xyz& other) {
        return op.isEqual(x, other.x) ? (y < other.y) : (x < other.x);
    }
};

class GenerateGuards {

public:
    std::vector<std::string> names;

    std::vector<double> heights;
    std::vector<double> radiuses;
    std::vector<double> angles;
    std::vector<int> icosts;
    std::vector<double> mcosts;

    std::vector<int> amounts;

    GenerateGuards() {
        
    }

    template <class T>
    void insert_attribute(std::vector<T>& value_set, const int n, const std::string& line) {
        value_set = new std::vector<T>(n);
        std::stringstream ss(line);

        for (int i=0; i<n; i++)
            ss >> value_set[i];
    }

    void read_file(std::string path, std::string filename) {
        std::cout << filename << "\n";

        std::string full_path = path + filename;
        std::ifstream file(full_path);
        
        std::string line;
        std::stringstream ss;
        
        std::string attribute;
        int n;

        while(getline(file,line)) {
            ss.str(line);
            ss >> attribute >> n;

            getline(file, line);
            if (attribute == "names")
                insert_attribute(names, n, line);
            else if (attribute == "heights")
                insert_attribute(heights, n, line);
            else if (attribute == "radiuses")
                insert_attribute(radiuses, n, line);
            else if (attribute == "angles")
                insert_attribute(angles, n, line);
            else if (attribute == "icosts")
                insert_attribute(icosts, n, line);
            else if (attribute == "mcosts")
                insert_attribute(mcosts, n, line);
            else if (attribute == "amounts")
                insert_attribute(amounts, n, line);
        }
    }
    
    const GuardType& create_GuardType(const int name, const int height, const int radius, const int angle, const int icost, const int mcost, const int amount) {
        return GuardType(names[name], heights[height], radiuses[radius], angles[angle], icosts[icost], mcosts[mcost], amounts[amount]);
    }

    const void print_GuardType(std::ofstream& file, const int name, const int height, const int radius, const int angle, const int icost, const int mcost, const int amount) {
        
        file << names[name] << '\t' << heights[height] << '\t' << radiuses[radius] << '\t' << angles[angle] << '\t' << icosts[icost] << '\t' << mcosts[mcost] << '\t' << amounts[amount] << '\n';
        
    }
};
