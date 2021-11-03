#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <fstream>
#include <math.h>
#include <filesystem>
#include "../Utils/Utils.h" 
#include "Terrain.h"
#include "GuardType.h"
#include "AllocGPos.h"
#include "SubAlloc.h"
#include "Situation.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

std::vector<int> angles = {0, 45, 90, 135, 180, 225, 270, 315};

NewAlloc::NewAlloc(const int angle, GuardPos& guardPos, const Observer& position, int gidx, long long int OF_inc, const long long int& numCovered_diff, const long long int& numTwiceCovered_diff) {
    alloc = Allocation(angle, guardPos.guard, position, gidx);
    this->OF_inc = OF_inc;
    this->numCovered_inc = numCovered_diff;
    this->numTwiceCovered_inc = numTwiceCovered_diff;
    

    // double max_radius = guard_type.radius * ((int)position.shed.size());
    // for(int i=0; i<position.shed.size(); i++) {
    //     for(int j=0; j<position.shed.size(); j++) {
    //         if(!covered[i][j] && position.shed[i][j]) {
    //             diff.x = i-from.x; diff.y = j-from.y;
    //             if( sqrt(diff.dot(diff)) <= max_radius)
    //                 OF_inc += 1;
    //         }
    //     }
    // }
}

bool NewAlloc::operator<(const NewAlloc& other) const {
    return this->OF_inc < other.OF_inc;
}

long double NewAlloc::operator-(const NewAlloc& other) {
    return this->OF_inc - other.OF_inc;
}

Situation::Situation() {
    this->guard_types = NULL;
    this->dem = NULL;
    covered = std::vector<std::vector<short int> >();
    guard_amount = std::vector<int>();
    OF = 0.0;
    numCovered = 0;
    numTwiceCovered = 0;
    iCost = 0;
    mCost = 0;
}

Situation::Situation(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    covered = std::vector<std::vector<short int> >(this->dem->nrows, std::vector<short int>(this->dem->nrows, 0));
    guard_amount = std::vector<int>(guard_types.size());
    for(int i=0; i<guard_types.size(); i++)
        guard_amount[i] = guard_types[i].amount;
    OF = 0;
    numCovered = 0;
    numTwiceCovered = 0;
    iCost = 0;
    mCost = 0;
}

long double Situation::calculate_OF() {
    numCovered = 0;
    numTwiceCovered = 0;
    for(int i=0; i<dem->nrows; i++) {
        for(int j=0; j<dem->nrows; j++) {
            numCovered += covered[i][j] > 0;
            numTwiceCovered += covered[i][j] > 1;
        }
    }
    iCost = 0;
    for(auto& alloc: allocations) {
        iCost += alloc.guard->icost;
    }
    OF = 4*numCovered + numTwiceCovered - iCost;
    return OF;
}

void Situation::print(str& filepath) {
    str toPrint = "";
    toPrint += std::to_string(this->numCovered) + "\t" + std::to_string(this->numTwiceCovered) + "\t" + std::to_string(this->iCost) + "\t" + std::to_string(this->OF) + "\n";
    for(const Allocation& alloc: this->allocations) {
        toPrint += alloc.guard->name + "\t" + std::to_string(alloc.position->x) + "\t" + std::to_string(alloc.position->y) + "\t" + std::to_string(alloc.angle) + "\n";
    }
    std::ofstream file(filepath);
    file << toPrint;
    file.close();
}

bool Situation::calculate_newPossibilities() {
    long long int OF_inc;
    long long int numCovered_inc;
    long long int numTwiceCovered_inc;
    for(const Observer& position: dem->best_observers) {
        for(int i=0; i<guard_types->size(); i++) {
            // if(guard_amount[i] == 0)
                // continue;
            GuardPos possibility(guard_types->at(i), position, covered);
            if(guard_types->at(i).angle == 360) {
                OF_inc = possibility.calculateOF_inc(0, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
                newPossibilities.push_back(NewAlloc(0, possibility, position, i, OF_inc, numCovered_inc, numTwiceCovered_inc));
            }
            else {
                for(int angle: angles) {
                    OF_inc = possibility.calculateOF_inc(angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
                    newPossibilities.push_back(NewAlloc(angle, possibility, position, i, OF_inc, numCovered_inc, numTwiceCovered_inc));
                }
            }
        }
    }
    return !newPossibilities.empty();
}

void Situation::random_newPossibilities(int sampleSize) {
    long long int OF_inc;
    long long int numCovered_inc;
    long long int numTwiceCovered_inc;

    for(int i=0; i<sampleSize; i++) {
        const int posidx = rand()%dem->best_observers.size();
        const int guardidx = rand()%guard_types->size();
        GuardPos possibility(guard_types->at(guardidx), dem->best_observers[posidx], covered);
        if(guard_types->at(guardidx).angle == 360) {
            OF_inc = possibility.calculateOF_inc(0, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
            newPossibilities.push_back(NewAlloc(0, possibility, dem->best_observers[posidx], guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));
        }
        else {
            for(int angle: angles) {
                OF_inc = possibility.calculateOF_inc(angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
                newPossibilities.push_back(NewAlloc(angle, possibility, dem->best_observers[posidx], guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));
            }
        }
    }
}

void Situation::updateCovered(Allocation& alloc) {
    const GuardType* guard = alloc.guard;
    const Observer* position = alloc.position;
    int x = position->x; int y = position->y;
    int h = guard->height;
    
    int radius = guard->radius*covered.size()/100;
    int start = std::max(0, x - radius);
    int stop = std::min((int)covered.size()-1, x + radius);
    int i = start;

    int ii, jj;

    int angle_min = alloc.angle/45;
    int angle_max = ((alloc.angle + guard->angle)%360)/45;

    std::vector<bool> sectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }
    
    for(int line=0; line<position->limits_row.at(guard->radius).size(); line++, i++) {
        for(int j=position->limits_row.at(guard->radius)[line].first; j<=position->limits_row.at(guard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(position->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] += sectors[0];
                    else
                        covered[i][j] += sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] += sectors[2];
                    else
                        covered[i][j] += sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] += sectors[4];
                    else
                        covered[i][j] += sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] += sectors[6];
                    else
                        covered[i][j] += sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]++;
            }
        }
    }
}

void Situation::insertNewAlloc(NewAlloc& newAlloc) {
    allocations.push_back(newAlloc.alloc);
    OF += newAlloc.OF_inc;
    numCovered += newAlloc.numCovered_inc;
    numTwiceCovered += newAlloc.numTwiceCovered_inc;
    iCost += newAlloc.alloc.guard->icost;
    updateCovered(newAlloc.alloc);
    // guard_amount[newAlloc.alloc.guardidx]--;
    newPossibilities.clear();
}

void Situation::switchSubAlloc(SubAlloc& subAlloc) {
    allocations.push_back(subAlloc.alloc);
    OF += subAlloc.OF_diff;
    numCovered += subAlloc.numCovered_diff;
    numTwiceCovered += subAlloc.numTwiceCovered_diff;
    iCost += subAlloc.icost_diff;
    // updateCovered(subAlloc.alloc); TODO
    // guard_amount[newAlloc.alloc.guardidx]--;
    newPossibilities.clear();
}

bool Situation::addRandomNewAlloc() {
    int i = rand()%(guard_types->size());
    int j = rand()%(dem->best_observers.size());
    int angle = angles[rand()%angles.size()];

    GuardPos possibility(guard_types->at(i), dem->best_observers[j], covered);

    long long int numCovered_inc, numTwiceCovered_inc;
    long long int OF_inc = possibility.calculateOF_inc(angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);

    NewAlloc toAdd(angle, possibility, dem->best_observers[j], i, OF_inc, numCovered_inc, numTwiceCovered_inc);
    insertNewAlloc(toAdd);
    return true;
}

void Situation::updateCovered_Pos(Allocation& alloc, const Observer* oldPos) {
    const GuardType* guard = alloc.guard;
    const Observer* position = alloc.position;
    int x = position->x; int y = position->y;
    int h = guard->height;
    
    int radius = guard->radius*covered.size()/100;
    int start = std::max(0, x - radius);
    int stop = std::min((int)covered.size()-1, x + radius);
    int i = start;

    int ii, jj;

    int angle_min = alloc.angle/45;
    int angle_max = ((alloc.angle + guard->angle)%360)/45;

    std::vector<bool> sectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }
    
    for(int line=0; line<position->limits_row.at(guard->radius).size(); line++, i++) {
        for(int j=position->limits_row.at(guard->radius)[line].first; j<=position->limits_row.at(guard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(position->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] += sectors[0];
                    else
                        covered[i][j] += sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] += sectors[2];
                    else
                        covered[i][j] += sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] += sectors[4];
                    else
                        covered[i][j] += sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] += sectors[6];
                    else
                        covered[i][j] += sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]++;
            }
        }
    }

    x = oldPos->x; y = oldPos->y;
    start = std::max(0, x - radius);
    stop = std::min((int)covered.size()-1, x + radius);
    i = start;
    
    for(int line=0; line<oldPos->limits_row.at(guard->radius).size(); line++, i++) {
        for(int j=oldPos->limits_row.at(guard->radius)[line].first; j<=oldPos->limits_row.at(guard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(oldPos->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] -= sectors[0];
                    else
                        covered[i][j] -= sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] -= sectors[2];
                    else
                        covered[i][j] -= sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] -= sectors[4];
                    else
                        covered[i][j] -= sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] -= sectors[6];
                    else
                        covered[i][j] -= sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]--;
            }
        }
    }
}

void Situation::updateCovered_Guard(Allocation& alloc, const GuardType* oldGuard) {
    const GuardType* newGuard = alloc.guard;
    const Observer* position = alloc.position;
    int x = position->x; int y = position->y;
    int oh = oldGuard->height;
    int nh = newGuard->height;
    
    volatile int radius = newGuard->radius*covered.size()/100;
    int start = std::max(0, x - radius);
    int stop = std::min((int)covered.size()-1, x + radius);
    int i = start;

    volatile int ii, jj;

    int angle_min = alloc.angle/45;
    int angle_max = ((alloc.angle + newGuard->angle)%360)/45;

    std::vector<bool> sectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }
    
    for(int line=0; line<position->limits_row.at(newGuard->radius).size(); line++, i++) {
        for(int j=position->limits_row.at(newGuard->radius)[line].first; j<=position->limits_row.at(newGuard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(position->shed.at(nh)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] += sectors[0];
                    else
                        covered[i][j] += sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] += sectors[2];
                    else
                        covered[i][j] += sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] += sectors[4];
                    else
                        covered[i][j] += sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] += sectors[6];
                    else
                        covered[i][j] += sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]++;
            }
        }
    }


    angle_min = alloc.angle/45;
    angle_max = ((alloc.angle + oldGuard->angle)%360)/45;

    sectors = {false, false, false, false, false, false, false, false};
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    } 

    x = position->x; y = position->y;
    radius = oldGuard->radius*covered.size()/100;
    start = std::max(0, x - radius);
    stop = std::min((int)covered.size()-1, x + radius);
    i = start;
    
    for(int line=0; line<position->limits_row.at(oldGuard->radius).size(); line++, i++) {
        for(int j=position->limits_row.at(oldGuard->radius)[line].first; j<=position->limits_row.at(oldGuard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(position->shed.at(oh)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] -= sectors[0];
                    else
                        covered[i][j] -= sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] -= sectors[2];
                    else
                        covered[i][j] -= sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] -= sectors[4];
                    else
                        covered[i][j] -= sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] -= sectors[6];
                    else
                        covered[i][j] -= sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]--;
            }
        }
    }
}

void Situation::updateCovered_Angle(Allocation& alloc, const int oldAngle) {
    const GuardType* guard = alloc.guard;
    const Observer* position = alloc.position;
    int x = position->x; int y = position->y;
    int h = guard->height;
    
    int radius = guard->radius*covered.size()/100;
    int start = std::max(0, x - radius);
    int stop = std::min((int)covered.size()-1, x + radius);
    int i = start;

    int ii, jj;

    int angle_min = alloc.angle/45;
    int angle_max = ((alloc.angle + guard->angle)%360)/45;

    std::vector<bool> sectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }

    angle_min = oldAngle/45;
    angle_max = ((oldAngle + guard->angle)%360)/45;
    std::vector<bool> oldSectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            oldSectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            oldSectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            oldSectors[idx] = true;
    }
    
    for(int line=0; line<position->limits_row.at(guard->radius).size(); line++, i++) {
        for(int j=position->limits_row.at(guard->radius)[line].first; j<=position->limits_row.at(guard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(position->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj) {
                        covered[i][j] += sectors[0];
                        covered[i][j] -= oldSectors[0];
                    } else {
                        covered[i][j] += sectors[1];
                        covered[i][j] -= oldSectors[1];
                    }
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj) {
                        covered[i][j] += sectors[2];
                        covered[i][j] -= oldSectors[2];
                    } else {
                        covered[i][j] += sectors[3];
                        covered[i][j] -= oldSectors[3];
                    }
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj) {
                        covered[i][j] += sectors[4];
                        covered[i][j] -= oldSectors[4];
                    } else {
                        covered[i][j] += sectors[5];
                        covered[i][j] -= oldSectors[5];
                    }
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj) {
                        covered[i][j] += sectors[6];
                        covered[i][j] -= oldSectors[6];
                    } else {
                        covered[i][j] += sectors[7];
                        covered[i][j] -= oldSectors[7];
                    }
                }
            }
        }
    }
}

void Situation::updateCovered_All(Allocation& newAlloc, const Allocation& oldAlloc) {
    const GuardType* newGuard = newAlloc.guard;
    const Observer* newPos = newAlloc.position;
    int x = newPos->x; int y = newPos->y;
    int h = newGuard->height;
    
    int radius = newGuard->radius*covered.size()/100;
    int start = std::max(0, x - radius);
    int stop = std::min((int)covered.size()-1, x + radius);
    int i = start;

    int ii, jj;

    int angle_min = newAlloc.angle/45;
    int angle_max = ((newAlloc.angle + newGuard->angle)%360)/45;

    std::vector<bool> sectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }
    
    for(int line=0; line<newPos->limits_row.at(newGuard->radius).size(); line++, i++) {
        for(int j=newPos->limits_row.at(newGuard->radius)[line].first; j<=newPos->limits_row.at(newGuard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(newPos->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] += sectors[0];
                    else
                        covered[i][j] += sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] += sectors[2];
                    else
                        covered[i][j] += sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] += sectors[4];
                    else
                        covered[i][j] += sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] += sectors[6];
                    else
                        covered[i][j] += sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]++;
            }
        }
    }

    
    const GuardType* oldGuard = oldAlloc.guard;
    const Observer* oldPos = oldAlloc.position;
    x = oldPos->x; y = oldPos->y;
    h = oldGuard->height;
    
    radius = oldGuard->radius*covered.size()/100;
    start = std::max(0, x - radius);
    stop = std::min((int)covered.size()-1, x + radius);
    i = start;

    angle_min = oldAlloc.angle/45;
    angle_max = ((oldAlloc.angle + oldGuard->angle)%360)/45;

    sectors = std::vector<bool>(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }
    
    for(int line=0; line<oldPos->limits_row.at(oldGuard->radius).size(); line++, i++) {
        for(int j=oldPos->limits_row.at(oldGuard->radius)[line].first; j<=oldPos->limits_row.at(oldGuard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(oldPos->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] -= sectors[0];
                    else
                        covered[i][j] -= sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] -= sectors[2];
                    else
                        covered[i][j] -= sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] -= sectors[4];
                    else
                        covered[i][j] -= sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] -= sectors[6];
                    else
                        covered[i][j] -= sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]--;
            }
        }
    }
}

void Situation::updateCovered_Remove(Allocation& oldAlloc) {
    const GuardType* oldGuard = oldAlloc.guard;
    const Observer* oldPos = oldAlloc.position;
    int x = oldPos->x; int y = oldPos->y;
    int h = oldGuard->height;
    
    int radius = oldGuard->radius*covered.size()/100;
    int start = std::max(0, x - radius);
    int stop = std::min((int)covered.size()-1, x + radius);
    int i = start;

    int ii, jj;

    int angle_min = oldAlloc.angle/45;
    int angle_max = ((oldAlloc.angle + oldGuard->angle)%360)/45;

    std::vector<bool> sectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            sectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            sectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            sectors[idx] = true;
    }
    
    for(int line=0; line<oldPos->limits_row.at(oldGuard->radius).size(); line++, i++) {
        for(int j=oldPos->limits_row.at(oldGuard->radius)[line].first; j<=oldPos->limits_row.at(oldGuard->radius)[line].second; j++) {
            ii = i - x;
            jj = j - y;
            if(oldPos->shed.at(h)[i][j]) {
                if(ii <= 0 && jj > 0) {
                    if(ii > -jj)
                        covered[i][j] -= sectors[0];
                    else
                        covered[i][j] -= sectors[1];
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        covered[i][j] -= sectors[2];
                    else
                        covered[i][j] -= sectors[3];
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        covered[i][j] -= sectors[4];
                    else
                        covered[i][j] -= sectors[5];
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        covered[i][j] -= sectors[6];
                    else
                        covered[i][j] -= sectors[7];
                } else if(ii == 0 && jj == 0)
                    covered[i][j]--;
            }
        }
    }
}

void Situation::replaceAlloc(SubAlloc& subAlloc, int mod) {
    auto newIt = allocations.insert(subAlloc.oldAlloc, subAlloc.alloc);
    OF += subAlloc.OF_diff;
    numCovered += subAlloc.numCovered_diff;
    numTwiceCovered += subAlloc.numTwiceCovered_diff;
    iCost += subAlloc.icost_diff;
    if(mod == 0)
        updateCovered_Pos(subAlloc.alloc, subAlloc.oldAlloc->position);
    else if(mod==1)
        updateCovered_Guard(subAlloc.alloc, subAlloc.oldAlloc->guard);
    else if(mod==2)
        updateCovered_Angle(subAlloc.alloc, subAlloc.oldAlloc->angle);
    else if(mod==3)
        updateCovered_All(subAlloc.alloc, *subAlloc.oldAlloc);
    allocations.erase(subAlloc.oldAlloc);
    subPossibilities.clear();
}

void Situation::removeAlloc(SubAlloc& subAlloc) {
    OF += subAlloc.OF_diff;
    numCovered += subAlloc.numCovered_diff;
    numTwiceCovered += subAlloc.numTwiceCovered_diff;
    iCost += subAlloc.icost_diff;
    updateCovered_Remove(*subAlloc.oldAlloc);
    allocations.erase(subAlloc.oldAlloc);
    subPossibilities.clear();
}

void Situation::switchPos(std::list<Allocation>::iterator& alloc) {
    const GuardType* guard = alloc->guard;
    const Observer* pos = alloc->position;
    int angle = alloc->angle;
    int guardidx = alloc->guardidx;

    for(const Observer& newPosition: dem->best_observers) {
        // if(rand()%100 >= 40) continue;
        if(newPosition.x == pos->x && newPosition.y == pos->y) continue;

        Allocation newAlloc(angle, guard, newPosition, guardidx);
        subPossibilities.push_back(SubAlloc(alloc, newAlloc, dem->nrows, covered));
        if(subPossibilities.rbegin()->OF_diff <= 0)
            subPossibilities.pop_back();
    }
}

void Situation::switchGuard(std::list<Allocation>::iterator& alloc) {
    const GuardType* guard = alloc->guard;
    const Observer* pos = alloc->position;
    int angle = alloc->angle;
    int guardidx = alloc->guardidx;

    for(const GuardType& newGuard: *guard_types) {
        if (newGuard.name == guard->name) continue;
        // GuardPos possibility(*guard, newGuard, *pos, covered);

        // long long int numCovered_inc, numTwiceCovered_inc;
        // long long int OF_inc = possibility.calculateOF_incGuard(angle, guard, numCovered_inc, numTwiceCovered_inc, dem->nrows, true);

        // possibilities.push_back(NewAlloc(angle, possibility, *pos, guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));

        Allocation newAlloc(angle, newGuard, *pos, guardidx);
        subPossibilities.push_back(SubAlloc(alloc, newAlloc, dem->nrows, covered));
        if(subPossibilities.rbegin()->OF_diff <= 0)
            subPossibilities.pop_back();
    }
}

void Situation::switchAngle(std::list<Allocation>::iterator& alloc) {
    const GuardType* guard = alloc->guard;
    if(guard->angle == 360) return;
    const Observer* pos = alloc->position;
    int angle = alloc->angle;
    int guardidx = alloc->guardidx;

    GuardPos possibility(*guard, angle, *pos, covered);
    for(int newAngle=0; newAngle<360; newAngle+=45) {
        if(newAngle == angle) continue;

        // long long int numCovered_inc, numTwiceCovered_inc;
        // long long int OF_inc = possibility.calculateOF_incAngle(angle, newAngle, numCovered_inc, numTwiceCovered_inc, dem->nrows, true);

        // possibilities.push_back(NewAlloc(angle, possibility, *pos, guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));

        Allocation newAlloc(newAngle, guard, *pos, guardidx);
        subPossibilities.push_back(SubAlloc(alloc, newAlloc, dem->nrows, covered));
        if(subPossibilities.rbegin()->OF_diff <= 0)
            subPossibilities.pop_back();
    }
}

void Situation::switchAll_PosBlock(std::list<Allocation>::iterator& alloc) {
    const GuardType* guard = alloc->guard;
    const Observer* pos = alloc->position;
    int oldAngle = alloc->angle;
    int guardidx = alloc->guardidx;
    Block block(pos->x, pos->y);

    for(const Observer& newPosition: dem->best_observers) {
        Block newBlock(newPosition.x, newPosition.y);
        if(block != newBlock) continue;
        for(const GuardType& newGuard: *guard_types) {

            if(newGuard.angle == 360) {
                if(newPosition.x == pos->x && newPosition.y == pos->y && newGuard.name == guard->name) continue;
                Allocation newAlloc(0, newGuard, newPosition, guardidx);
                subPossibilities.push_back(SubAlloc(alloc, newAlloc, dem->nrows, covered));
            }
            else {
                for(int angle: angles) {
                    if(newPosition.x == pos->x && newPosition.y == pos->y && newGuard.name == guard->name && angle == oldAngle) continue;
                    Allocation newAlloc(angle, newGuard, newPosition, guardidx);
                    subPossibilities.push_back(SubAlloc(alloc, newAlloc, dem->nrows, covered));
                }
            }

            if(subPossibilities.rbegin()->OF_diff <= 0)
                subPossibilities.pop_back();
        }
    }
}

void Situation::checkRemoveAlloc(std::list<Allocation>::iterator& alloc) {
    subPossibilities.push_back(SubAlloc(alloc, dem->nrows, covered));
}
