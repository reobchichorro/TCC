#include <iostream>
#include <string>
#include "Data/Terrain.cpp"
// #include "Utils/Utils.cpp"

// DoubleOps op;

int main() {
    std::string path, filename;
    std::cin >> path >> filename;
    Terrain dem;
    dem.read_file(path, filename);
}
