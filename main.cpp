#include <iostream>
#include "Data/Terrain.cpp"
#include <string>

int main() {
    std::string path, filename;
    std::cin >> path >> filename;
    Terrain dem;
    dem.read_file(path, filename);
}