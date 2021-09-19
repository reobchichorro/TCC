#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <fstream>

#define all(v) v.begin(),v.end()
#define eps 1e-6

typedef std::string str;

#ifndef __DOUBLEOPS_
#define __DOUBLEOPS_
class DoubleOps {

public:
    bool isEqual(const double a, const double b);
    bool isZero(const double num);
};
#endif

#ifndef __STRMANIPULATION_
#define __STRMANIPULATION_
class StringManipulation {

public:

    void removeFileType(const str& filename, str& outname);
};
#endif

#ifndef __XYZ_
#define __XYZ_
class xyz {

public:
    double x;
    double y;
    double z;
    DoubleOps op;

    xyz();

    xyz(const double xx, const double yy, const double zz);

    double diff(const xyz& other);

    void take_min(const xyz& other);

    void take_max(const xyz& other);

    bool operator<(const xyz& other);

    double dot(const xyz& other);
};
#endif

#ifndef __IJ_
#define __IJ_
class ij { // adicione 0.5 a i e j
private:
    int ii;
    int jj;

public:
    bool corner = false;

    ij(int i, int j, bool corner = false);

    double i() const;
    double j() const;

    int idxi() const;
    int idxj() const;
};
#endif
