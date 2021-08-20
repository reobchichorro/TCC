#include <iostream>
#include <string>
#include "Data/Terrain.cpp"
#include "Data/GuardType.cpp"
// #include "Utils/Utils.cpp"

// DoubleOps op;

struct InputFileData {
    std::string dem_abspath;
    std::string dem_filename;
    int resolution;
    std::string guardtypelist_abspath;
    std::string guardtypelist_filename;
    std::vector<bool> obj_bitlist;
    std::vector<bool> rest_bitlist;
};

void read_file(const std::string& path, const std::string& filename, InputFileData& input) {
    input.obj_bitlist.resize(5);
    input.rest_bitlist.resize(2);

    std::ifstream inputfile(path + filename);
    inputfile >> input.dem_abspath;
    inputfile >> input.dem_filename;
    inputfile >> input.resolution;
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

void read_guardtypelist_file(const std::string& path, const std::string& filename, std::vector<GuardType>& guardtypes) {
    std::string guardtype_abspath;
    int n;
    std::ifstream guardtypelistfile(path + filename);
    guardtypelistfile >> guardtype_abspath;
    guardtypelistfile >> n;
    guardtypes.resize(n);
    
    std::string guardtype_filename;
    for(int i=0; i<n; i++) {
        guardtypelistfile >> guardtype_filename;
        guardtypes[i].read_file(guardtype_abspath, guardtype_filename);
    }
}

int main() {
    std::string path, filename;
    std::cin >> path >> filename;
    InputFileData input;
    read_file(path, filename, input);
    Terrain dem;
    std::vector<GuardType> guard_types;

    // std::cout << input.dem_abspath << " ";
    // std::cout << input.dem_filename << "\n";
    // std::cout << input.resolution << "\n";
    // std::cout << input.guardtypelist_abspath << " ";
    // std::cout << input.guardtypelist_filename << "\n";
    // for(int i=0; i<5; i++) {
    //     std::cout << input.obj_bitlist[i] << " ";
    // }
    // std::cout << "\n";
    // for(int i=0; i<2; i++) {
    //     std::cout << input.rest_bitlist[i] << " ";
    // }
    // std::cout << "\n";

    dem.read_file(input.dem_abspath, input.dem_filename, input.resolution);
    read_guardtypelist_file(input.guardtypelist_abspath, input.guardtypelist_filename, guard_types);
}
