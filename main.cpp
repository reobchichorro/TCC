#include <iostream>
#include <string>
#include <set>
#include "Classes/Terrain.h"
#include "Classes/GuardType.h"
#include "Classes/Situation.h"
#include "Classes/Solver.h"
#include "Utils/Utils.h"

typedef std::string str;

str site_folder = "/home/reobc/Documents/Disciplinas/TCC/wrf/c/site/";

struct InputFileData {
    str test_case_name;
    str shedbin_folder;
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
    inputfile >> input.shedbin_folder;

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

void read_guardtypelist_file(const str& path, const str& filename, std::vector<GuardType>& guardtypes, std::set<int>& guard_radii) {
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
        guard_radii.insert(guardtypes[i].radius);
    }
}

int main(int argc, char** argv) {
    srand(time(0));
    if(argc != 3) 
        return 1;
    
    str path=argv[1], filename=argv[2];
    InputFileData input;
    read_file(path, filename, input);
    Terrain dem;
    std::vector<GuardType> guard_types;

    std::set<int> guard_radii;

    dem.read_file(site_folder, input.test_case_name);
    read_guardtypelist_file(input.guardtypelist_abspath, input.guardtypelist_filename, guard_types, guard_radii);
    for(auto& guard: guard_types) guard.adjustICost(dem.nrows);
    dem.fill_best_observers(site_folder, input.test_case_name, input.shedbin_folder, guard_radii);

    // Intersection calculate_angle;
    // calculate_angle.fill_view_angle(dem);

    Situation currSit(guard_types, dem);
    ILS test(guard_types, dem);
    test.solve(currSit);
    std::cout << 4*currSit.numCovered << "\t" << currSit.numTwiceCovered << "\t" << currSit.iCost << "\t" << currSit.OF << "\n";

    GA teste(guard_types, dem);
    for(int i=0; i<20; i++) {
        std::cout << 4*teste.best.numCovered << "\t" << teste.best.numTwiceCovered << "\t" << teste.best.iCost << "\t" << teste.best.OF << "\n";
        teste.createNewGeneration();
    }
    std::cout << 4*teste.best.numCovered << "\t" << teste.best.numTwiceCovered << "\t" << teste.best.iCost << "\t" << teste.best.OF << "\n";

}
