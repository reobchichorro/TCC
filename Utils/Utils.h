#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <time.h>
#include <iostream>
#include <cstdio>
#include <chrono>

#define all(v) v.begin(),v.end()
#define eps 1e-6

typedef std::string str;

#ifndef __GETSECTOR_
#define __GETSECTOR_
    int getSector(int ii, int jj);
#endif

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

#ifndef __BLOCK_
#define __BLOCK_
class Block {
private:
    int x;
    int y;
    int size = 30;

public:
    Block(const int x, const int y);

    bool operator==(const Block& other) const;
    bool operator!=(const Block& other) const;
};
#endif

#ifndef __TIMER_
#define __TIMER_
// // Utils for measuring time, etc...
// bool readFileContent(char *buffer, size_t sizeRead, FILE *fp);

//This class prints a message (and the ellapsed time) when its destructor is called (unless the message is empty)
//we may also reset the timer and get the current time now (since when the class was constructed)
class Timer {
public:
    Timer(const std::string &msg);
    ~Timer();
    void reset();
    void printNow(const std::string &st) const;
    double getTimeNow() const;
private: 
    typedef std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long long int, std::ratio<1, 1000000000> > > TIME_T;
    std::string st; 
    TIME_T t0;

    double convertTimeMsecs(const std::chrono::duration<double> td) const;
    std::chrono::duration<double> diff(TIME_T  start, TIME_T  end) const;
};
#endif
