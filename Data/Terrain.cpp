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

class Terrain {

public:
    std::string name;
    std::string type;
    std::string category;

    std::vector<std::vector<int> > height;

    int resolution = 20;
    DoubleOps op;

    Terrain() {
        name = "";
        type = "";
        category = "";
    }

    void test() {
        for(int i=0; i<height.size(); i++) {
            for(int j=0; j<height[i].size(); j++) {
                if(height[i][j] != -1) {
                    if(height[i][j] < 1000)
                        std::cerr << "0";
                    std::cerr << height[i][j];
                }
                else
                    std::cerr << "-001";
                if(j < height[i].size() -1)
                    std::cerr << " ";
                else
                    std::cerr << "\n";
            }
        }
    }

    void read_file(std::string path, std::string filename, int resolution) {
        this->resolution = resolution;
        
        std::ifstream file(path + filename);
        std::string line;
        int i=0;
        int point_height;
        while(getline(file, line)) {
            std::stringstream ss(line);
            height.push_back(std::vector<int>());
            while(ss >> point_height) {
                height[i].push_back(point_height);
            }
            i++;
        }
    }
};
