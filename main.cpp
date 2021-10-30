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

void read_guardtypelist_file(const str& path, const str& filename, std::vector<GuardType>& guardtypes, std::set<int>& guard_radii, std::set<int>& guard_heights) {
    str guardtype_abspath;
    int n;
    std::ifstream guardtypelistfile(path + filename);
    guardtypelistfile >> guardtype_abspath;
    guardtypelistfile >> n;
    guardtypes.resize(n);
    
    str guardtype_filename;
    for(int i=0; i<n; i++) {
        guardtypelistfile >> guardtype_filename;
        guardtypes[i].read_file(guardtype_abspath, guardtype_filename);
        // guard_radii.insert(guardtypes[i].radius);
        // guard_heights.insert(guardtypes[i].height);
    }
}

int main(int argc, char** argv) {
    InputFileData input;
    // read_file(path, filename, input);

    std::set<int> guard_radii = {5, 10, 15, 20, 25, 50};
    std::set<int> guard_heights = {0, 5, 10, 20, 50};

    for(const auto& entry: std::filesystem::directory_iterator("wrf/c/site/")) {
        if(entry.is_directory() && entry.path().filename().string()[0]=='2') {
            Terrain dem;
            std::cout << entry.path().filename().string() << "\n";
            dem.read_file(site_folder, input.test_case_name);
            // dem.fill_best_observers(site_folder, input.test_case_name, input.roi_folder, guard_radii, guard_heights);
        }

    }

    // str outputPath = "Outputs/" + std::to_string(version) + "/" + filename + "/";


}
