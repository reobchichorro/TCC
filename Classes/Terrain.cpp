#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
#include <math.h>
#include <filesystem>
#include "../Utils/Utils.h"
#include "Terrain.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()

typedef std::string str;
typedef std::pair<int, int> pii;

Observer::Observer(str& point_name, const std::filesystem::path& path, const int nrows) {
    point_name.erase(0, 1);
    auto pos = point_name.find('y');
    point_name.replace(pos, 1, " ");
    std::stringstream ss(point_name);
    ss >> x >> y;

    std::ifstream file_observer(path, std::ios::binary);
    std::vector<unsigned char> o_shed(std::istreambuf_iterator<char>(file_observer), {});
    shed = std::vector<std::vector<bool> >(nrows, std::vector<bool>(nrows, 0));
    
    int i=0, j=0;
    for(int l=0; l<o_shed.size(); l++) {
        for(int k=7; k>=0; k--) {
            shed[i][j] = (o_shed[l])&(1<<k);
            j++;
            if(j >= nrows) {
                j=0; i++;
                if(i >= nrows)
                    break;
            }
        }
    }

    limits_row = std::vector<std::vector<pii> >(6);
    limits_col = std::vector<std::vector<pii> >(6);
    
    int dist = 0, dist2 = 0;
    int deltax, deltay;
    long double root;
    int ans1, ans2;
    int start, stop;

    for(int a=0; a<5; a++) {
        dist = (a+1)*5*nrows/100; dist2 = dist*dist;
        start = std::max(0, x - dist);
        stop = std::min(nrows-1, x + dist);

        limits_row[a] = std::vector<pii>(stop-start+1);

        for(int i=start; i<=stop; i++) {
            deltax = x - i;
            root = sqrt(dist2 - deltax*deltax);

            ans2 = std::max(0, (int)std::ceil(y - root));
            ans1 = std::min(nrows-1, (int)std::floor(y + root));

            limits_row[a][i-start] = {ans2, ans1};
        }

        start = std::max(0, y - dist);
        stop = std::min(nrows-1, y + dist);

        limits_col[a] = std::vector<pii>(stop-start+1);

        for(int j=start; j<=stop; j++) {
            deltay = y - j;
            root = sqrt(dist2 - deltay*deltay);

            ans2 = std::max(0, (int)std::ceil(x - root));
            ans1 = std::min(nrows-1, (int)std::floor(x + root));

            limits_col[a][j-start] = {ans2, ans1};
        }
    }

    {
        dist = nrows/2; dist2 = dist*dist;
        start = std::max(0, x - dist);
        stop = std::min(nrows-1, x + dist);

        limits_row[5] = std::vector<pii>(stop-start+1);

        for(int i=start; i<=stop; i++) {
            deltax = x - i;
            root = sqrt(dist2 - deltax*deltax);

            ans2 = std::max(0, (int)std::ceil(y - root));
            ans1 = std::min(nrows-1, (int)std::floor(y + root));

            limits_row[5][i-start] = {ans2, ans1};
        }

        start = std::max(0, y - dist);
        stop = std::min(nrows-1, y + dist);

        limits_col[5] = std::vector<pii>(stop-start+1);

        for(int j=start; j<=stop; j++) {
            deltay = y - j;
            root = sqrt(dist2 - deltay*deltay);

            ans2 = std::max(0, (int)std::ceil(x - root));
            ans1 = std::min(nrows-1, (int)std::floor(x + root));

            limits_col[5][j-start] = {ans2, ans1};
        }
    }
}

Terrain::Terrain() {
    name = "";
    type = "";
    category = "";
}

void Terrain::test() {
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

void Terrain::read_file(const str& site_folder, const str& test_case_name, const str& shedbin_folder) {
    name = test_case_name;
    
    std::ifstream file_elsize(site_folder + test_case_name + "/elsize");
    file_elsize >> elsize; file_elsize.close();

    std::ifstream file_nrows(site_folder + test_case_name + "/nrows");
    file_nrows >> nrows; file_nrows.close();
    
    std::ifstream file_resolution(site_folder + test_case_name + "/resolution");
    file_resolution >> resolution; file_resolution.close();

    std::ifstream file_elevs(site_folder + test_case_name + "/elevs", std::ios::binary);
    std::vector<unsigned char> elevs(std::istreambuf_iterator<char>(file_elevs), {});
    height = std::vector<std::vector<int> >(nrows, std::vector<int>(nrows, 0));
    
    std::vector<int> number(elsize);
    int l=0;
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<nrows; j++) {
            for(int k=0; k<elsize; k++, l++) {
                number[k] = elevs[l];
                for(int p = 0; p<k; p++)
                    number[k] *= 256;
                height[i][j] += number[k];
            }
        }
    }

    str point_name;
    for(const auto& file: std::filesystem::directory_iterator(site_folder + test_case_name + "/" + shedbin_folder)) {
        point_name = file.path().filename();
        best_observers.push_back(Observer(point_name, file.path(), nrows));
    }
}
