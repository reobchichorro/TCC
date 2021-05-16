#include <string>
#include <algorithm>

#include <vector>
#include <unordered_map>
#include <fstream>

#define all(v) v.begin(),v.end()

class StringManipulation {

public:

    void removeFileType(const std::string& filename, std::string& outname) {
        outname = filename;
        outname.erase(std::find(all(outname), '.'),outname.end());
    }
};
