#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
#include "../Utils/str_manipulation.cpp"

#define eps 1e-6
#define all(v) v.begin(),v.end()

class xyz {

public:
    double x;
    double y;
    double z;

    xyz() {}

    xyz(const double xx, const double yy, const double zz) {
        x = xx; y = yy; z = zz;
    }

    double diff(const xyz& other) {
        xyz diff(other.x - x, other.y - y, other.z - z);
        if (abs(diff.x) < eps || abs(diff.x) > abs(diff.y))
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

};

class Terrain {

public:
    std::string name;
    std::string type;
    std::string category;

    std::vector<std::vector<double> > height;
    std::unordered_map<double, int> horizontal_index;
    std::unordered_map<double, int> vertical_index;

    Terrain() {
        name = "";
        type = "";
        category = "";
    }

    void read_file(std::string path, std::string filename) {
        StringManipulation str_man;
        str_man.removeFileType(filename, name);
        std::cout << filename << " " << name << "\n";

        std::string full_path = path + filename;
        std::ifstream file(full_path);
        
        std::string line;
        std::stringstream ss;
        std::vector<xyz> all_points;
        double x,y,z;
        // int i = 0;

        while(getline(file,line)) {
            std::replace(all(line), ',', ' ');
            ss.str(line);
            ss >> x >> y >> z;
            // std::cout << x << " " << y << " " << z << "\n";
            all_points.push_back(xyz(x,y,z));
            // i++;
        }

        double diff = all_points[1].diff(all_points[0]);
        double temp = 0.0;
        for(int i=1; i<all_points.size()-1; i++) {
            temp = all_points[i+1].diff(all_points[i]);
            if (abs(temp-diff) > eps)
                std::cerr << i << " " << temp << " " << diff << "\n";
        }

        xyz smallest=all_points[0], largest=all_points[0];
        std::vector<std::pair<xyz,xyz> > line_limit_points;
        double line_x = -1.0;
        std::map<int, int> x_counter, y_counter;
        std::vector<std::pair<int,int> > keys(all_points.size());
        for(int i=0; i<all_points.size(); i++) {
            smallest.take_min(all_points[i]);
            largest.take_max(all_points[i]);

            if(abs(line_x - all_points[i].x) > eps) {
                line_limit_points.push_back(std::pair<xyz,xyz>(all_points[i],all_points[i]));
                line_x = all_points[i].x;
            } else {
                line_limit_points.back().first.take_min(all_points[i]);
                line_limit_points.back().second.take_max(all_points[i]);
            }

            keys[i].first = (int) all_points[i].x;
            keys[i].second = (int) all_points[i].y;
            if(x_counter.find(keys[i].first) != x_counter.end()) {
                x_counter[keys[i].first]++;
            } else if(x_counter.find(keys[i].first-1) != x_counter.end()) {
                keys[i].first--;
                x_counter[keys[i].first-1]++;
            } else if(x_counter.find(keys[i].first+1) != x_counter.end()) {
                keys[i].first++;
                x_counter[keys[i].first]++;
            } else {
                x_counter[keys[i].first] = 1;
            }

            if(y_counter.find(keys[i].second) != y_counter.end()) {
                y_counter[keys[i].second]++;
            } else if(y_counter.find(keys[i].second-1) != y_counter.end()) {
                keys[i].second--;
                y_counter[keys[i].second]++;
            } else if(y_counter.find(keys[i].second+1) != y_counter.end()) {
                keys[i].second++;
                y_counter[keys[i].second]++;
            } else {
                y_counter[keys[i].second] = 1;
            }
        }

        std::vector<std::pair<xyz,xyz> > column_limit_points;

        for(int i=0; i<line_limit_points.size(); i++) {
            std::cerr << line_limit_points[i].first.x << "," << line_limit_points[i].first.y << " - " << line_limit_points[i].second.x << "," << line_limit_points[i].second.y << "\n";
        }

        // for (auto &p: x_counter) {
        //     std::cout << p.first << " - " << p.second << "\n";
        // }
        // std::cout << "here\n";
        // for (auto &p: y_counter) {
        //     std::cout << p.first << " - " << p.second << "\n";
        // }

        std::map<int,int> x_counter2, y_counter2;
        for (auto &p: x_counter) {
            if(x_counter2.find(p.second) != x_counter2.end()) {
                x_counter2[p.second]++;
            } else {
                x_counter2[p.second] = 1;
            }
        }
        for (auto &p: y_counter) {
            if(y_counter2.find(p.second) != y_counter2.end()) {
                y_counter2[p.second]++;
            } else {
                y_counter2[p.second] = 1;
            }
        }
        // for (auto &p: x_counter2) {
        //     std::cerr << p.first << " - " << p.second << "\n";
        // }
        // std::cout << "here\n";
        // for (auto &p: y_counter2) {
        //     std::cerr << p.first << " - " << p.second << "\n";
        // }
        std::cerr << line_limit_points.size() << "\n";
        
        std::pair<int,int> smallest_key = {x_counter.begin()->second, y_counter.begin()->second}, largest_key = {x_counter.rbegin()->second, y_counter.begin()->second};
        height = std::vector<std::vector<double> >(x_counter.size(), std::vector<double>(y_counter.size(), -1.0));

        std::unordered_map<int, int> xindex, yindex;

        int xpos = 0, ypos = 0;
        for(auto itx=x_counter.cbegin(); itx!=x_counter.cend(); itx++, xpos++) {
            xindex[itx->first] = xpos;
        }
        for(auto ity=y_counter.cbegin(); ity!=y_counter.cend(); ity++, ypos++) {
            yindex[ity->first] = ypos;
        }

        for(int pos = 0; pos < all_points.size(); pos++) {
            int i = xindex[keys[pos].first];
            int j = yindex[keys[pos].second];
            height[i][j] = all_points[pos].z;
            horizontal_index[all_points[pos].x] = i;
            vertical_index[all_points[pos].y] = j;
        }
        xpos = 0, ypos = 0;
        // for(auto itx=x_counter.cbegin(); itx!=x_counter.cend(); itx++, xpos++) {
        //     ypos = 0;
        //     for(auto ity=y_counter.cbegin(); ity!=y_counter.cend(); ity++, ypos++) {
        //         std::pair<int,int> coords(itx->first, ity->first);
        //         // std::find(keys.begin(), keys.end(), coords);
        //         auto itkey = std::find(all(keys), coords);
        //         int pos = itkey - keys.cbegin();
        //         height[xpos][ypos] = all_points[pos].z;
        //         horizontal_index[all_points[pos].x] = xpos;
        //         vertical_index[all_points[pos].y] = ypos;
        //     }
        // }

        int digits = (int) largest.z >= 1000 ? 4 : 3;
        for(int i=0; i<height.size(); i++) {
            for(int j=0; j<height[i].size(); j++) {
                std::cout << std::setfill('0') << std::setw(digits) << (int) height[i][j] << " ";
            }
            std::cout << "\n";
        }
    }
};
