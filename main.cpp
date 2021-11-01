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
int version = 0;

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
        guard_radii.insert(guardtypes[i].radius);
        guard_heights.insert(guardtypes[i].height);
    }
}

int main(int argc, char** argv) {
    srand(3);
    if(argc != 3) 
        return 1;
    
    str path=argv[1], filename=argv[2];
    InputFileData input;
    read_file(path, filename, input);
    Terrain dem;
    std::vector<GuardType> guard_types;

    std::set<int> guard_radii;
    std::set<int> guard_heights;

    dem.read_file(site_folder, input.test_case_name);
    read_guardtypelist_file(input.guardtypelist_abspath, input.guardtypelist_filename, guard_types, guard_radii, guard_heights);
    for(auto& guard: guard_types) guard.adjustICost(dem.nrows);
    dem.fill_best_observers(site_folder, input.test_case_name, input.roi_folder, guard_radii, guard_heights);

    str outputPath = "Outputs/" + std::to_string(version) + "/" + filename + "/";
    std::filesystem::create_directory(outputPath);
    std::filesystem::create_directory(outputPath+"GreedyLS/");
    std::filesystem::create_directory(outputPath+"ILS/");
    std::filesystem::create_directory(outputPath+"GA/");

    str msg = "Greedy+LS";
    Timer tGreedyLS(msg);
    Situation currSit(guard_types, dem);
    Greedy s0(guard_types, dem);
    s0.solve(currSit);
    LS sStar(guard_types, dem);
    sStar.one_neigh_until_localopt(currSit, 1, 0);
    str filepath = outputPath + "GreedyLS/" + std::to_string(tGreedyLS.getTimeNow()) + ".csv";
    currSit.print(filepath);
    tGreedyLS.~Timer();
    std::cout << currSit.numCovered << "\t" << currSit.numTwiceCovered << "\t" << currSit.iCost << "\t" << currSit.OF << " a\n";
    currSit.calculate_OF();
    std::cout << currSit.numCovered << "\t" << currSit.numTwiceCovered << "\t" << currSit.iCost << "\t" << currSit.OF << " b\n";


    Timer tILS("ILS");
    ILS test(guard_types, dem);
    int it = 0;
    while(tILS.getTimeNow() < 450) {
        test.solve(currSit, it%3);
        it++;
        std::cout << it << " " << tILS.getTimeNow() << "\n";
        filepath = outputPath + "ILS/" + std::to_string(tILS.getTimeNow()) + ".csv";
        currSit.print(filepath);
    }
    
    tILS.~Timer();
    currSit.calculate_OF();
    std::cout << currSit.numCovered << "\t" << currSit.numTwiceCovered << "\t" << currSit.iCost << "\t" << currSit.OF << "\n";

    msg = "GA";
    Timer tGA(msg);
    GA teste(guard_types, dem);
    it = 0;
    str toPrint = ""; teste.best.print(toPrint);
    while(tGA.getTimeNow() < 450) {
        // std::cout << 4*teste.best.numCovered << "\t" << teste.best.numTwiceCovered << "\t" << teste.best.iCost << "\t" << teste.best.OF << "\n";
        teste.createNewGeneration();
        it++;
        std::cout << it << " " << tGA.getTimeNow() << "\n";
        filepath = outputPath + "GA/" + std::to_string(tGA.getTimeNow()) + ".csv";
        teste.best.print(filepath);
    }
    tGA.~Timer();

    teste.best.calculate_OF();
    std::cout << teste.best.numCovered << "\t" << teste.best.numTwiceCovered << "\t" << teste.best.iCost << "\t" << teste.best.OF << "\n";

}
