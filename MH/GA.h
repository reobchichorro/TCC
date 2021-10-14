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
#include "../Domain/Terrain.h"
#include "../Domain/GuardType.h"
#include "../Domain/Situation.h"
#include "GreedyLS.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

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
