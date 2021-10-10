#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <fstream>
#include <math.h>
#include <filesystem>
#include "../Utils/Utils.h" 
#include "Terrain.h"
#include "GuardType.h"
#include "Situation.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

#ifndef __GREEDY_
#define __GREEDY_
class Greedy {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;


    Greedy(std::vector<GuardType>& guard_types, Terrain& dem);
    
    bool advance(Situation& curr);
    bool advance_if_OF_gt_1(Situation& curr);
    void until100(Situation& curr);
    void spendAllGuards(Situation& curr);
    void solve(Situation& curr);
};
#endif

#ifndef __LS_
#define __LS_
class LS {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;


    LS(std::vector<GuardType>& guard_types, Terrain& dem);
    
    void only_one_neigh(Situation& curr, int neighborhood, int improv);
    void mix1(Situation& curr);
};
#endif

#ifndef __ILS_
#define __ILS_
class ILS {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;


    ILS(std::vector<GuardType>& guard_types, Terrain& dem);

    void perturb(Situation& sStar, Situation& s1);
    void solve(Situation& curr);
};
#endif

#ifndef __POPULATION_
#define __POPULATION_
class Population {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;

    std::vector<Situation> individuals;
    int popSize;


    Population(std::vector<GuardType>& guard_types, Terrain& dem);
    void addIndividual(const Situation& indi);
    void buildIndividual();
    void generatePop();
    void fillCovered();

    void reproduce(Situation& child, const Situation& dad, const Situation& mom);
    void mutate(Situation& child);
    void crossover(const Population& oldGen);

    // void perturb(Situation& sStar, Situation& s1);
    // void solve(Situation& curr);
};
#endif

#ifndef __GA_
#define __GA_
class GA {
public:
    std::vector<GuardType>* guard_types;
    Terrain* dem;

    Population* curr;
    Population* improved;
    Population* children;

    Situation best;

    GA(std::vector<GuardType>& guard_types, Terrain& dem);
    void createNewGeneration();

    // void perturb(Situation& sStar, Situation& s1);
    // void solve(Situation& curr);
};
#endif
