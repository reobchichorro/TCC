#include <iostream>
#include <string>
#include <set>
#include "Domain/Terrain.h"
#include "Domain/GuardType.h"
#include "Domain/AllocGPos.h"
#include "Domain/Situation.h"
#include "MH/GreedyLS.h"
#include "MH/ILS.h"
#include "MH/GA.h"
#include "Utils/Utils.h"

typedef std::string str;
int version = 1;

str site_folder = "/home/reobc/Documents/Disciplinas/TCC/wrf/c/site/";

struct InputFileData {
    str test_case_name;
    str roi_folder;
    str guardtypelist_abspath;
    str guardtypelist_filename;
    std::vector<bool> obj_bitlist;
    std::vector<bool> rest_bitlist;
};

void read_file(const str& path, const str& filename, InputFileData& input) {
    input.obj_bitlist.resize(5);
    input.rest_bitlist.resize(2);

    std::ifstream inputfile(path + filename);
    inputfile >> input.test_case_name;
    inputfile >> input.roi_folder;

    inputfile >> input.guardtypelist_abspath;
    inputfile >> input.guardtypelist_filename;
    for(int i=0; i<5; i++) {
        int k;
        inputfile >> k;
        input.obj_bitlist[i] = (bool)k;
    }
    for(int i=0; i<2; i++) {
        int k;
        inputfile >> k;
        input.rest_bitlist[i] = (bool)k;
    }
}

void makeGuardObs(const Terrain& dem, const std::set<int>& guard_radii, const std::set<int>& guard_heights) {
    int x, y;
    int radius;
    long long int numCovered;
    str k = "a";
    GuardObs aux(k, 0, 0, 0, 0, 0);
    for(const Observer& obs : dem.best_observers) {
        for(int h: guard_heights) {
            for(int r: guard_radii) {
                x = obs.x; y = obs.y;
                numCovered = 0;
                radius = r*dem.nrows/100;
                int start = std::max(0, x - radius);
                int stop = std::min(dem.nrows-1, x + radius);
                int i = start;
                
                for(int line=0; line<obs.limits_row.at(r).size(); line++, i++) {
                    for(int j=obs.limits_row.at(r)[line].first; j<=obs.limits_row.at(r)[line].second; j++) {
                        numCovered += (obs.shed.at(h)[i][j]);
                    }
                }

                aux.input = dem.name;
                aux.x = x;
                aux.y = y;
                aux.height = h;
                aux.radius = r;
                aux.numCovered = numCovered;
                aux.print("Utils/InputCreation/GuardDatabase/numCovered.csv");
            }
        }
    }
}

int main(int argc, char** argv) {
    InputFileData input;
    // read_file(path, filename, input);

    std::set<int> guard_radii = {5, 10, 15, 20, 25, 50};
    std::set<int> guard_heights = {0, 5, 10, 20, 50};
    
    std::ofstream outfile;
    outfile.open("Utils/InputCreation/GuardDatabase/nrows.csv", std::ios_base::app); // append instead of overwrite

    for(const auto& entry: std::filesystem::directory_iterator("wrf/c/site/")) {
        if(entry.is_directory() && entry.path().filename().string()[0]=='2') {
            std::ifstream f(entry.path() / "nrows");
            int nrows; f >> nrows;
            outfile << entry.path().filename().string() << "\t" << nrows << "\n"; 
            // Terrain dem;
            // std::cout << entry.path().filename().string() << "\n";
            // dem.read_file(site_folder, entry.path().filename().string());
            // dem.fill_best_observers(site_folder, entry.path().filename().string(), "r500/", guard_radii, guard_heights);
            // makeGuardObs(dem, guard_radii, guard_heights);
        }

    }

    // str outputPath = "Outputs/" + std::to_string(version) + "/" + filename + "/";


}
