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
#include "Situation.h"

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

std::vector<int> angles = {0, 45, 90, 135, 180, 225, 270, 315};

Allocation::Allocation() {
    guard = NULL;
    position = NULL;
    angle = -30;
}

Allocation::Allocation(const int angle, const GuardType& guard_type, const Observer& position, int gidx) {
    this->guard = &guard_type;
    this->position = &position;
    this->angle = angle;
    this->guardidx = gidx;
}

Allocation::Allocation(const int angle, const GuardType* guard_type, const Observer& position, int gidx) {
    this->guard = guard_type;
    this->position = &position;
    this->angle = angle;
    this->guardidx = gidx;
}

// Allocation::Allocation(const int angle, GuardType* guard_type, const int x, const int y) {
//     this->guard = guard_type;
//     this->x = x;
//     this->y = y;
//     this->angle = angle;
// }

Allocation::Allocation(const Allocation& other) {
    if(&other==this) 
        return;

    this->guard = other.guard;
    this->position = other.position;
    this->angle = other.angle;
    this->guardidx = other.guardidx;
}

GuardPos::GuardPos(const GuardType& guard_type, const Observer& oldPosition, const Observer& newPosition, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = newPosition.x; this->y = newPosition.y;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_covered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    
    int radius = guard_type.radius*covered.size()/100;
    int start = std::max(0, newPosition.x - radius);
    int stop = std::min((int)covered.size()-1, newPosition.x + radius);
    int i = start;

    int ii, jj;
    
    for(int line=0; line<newPosition.limits_row.at(guard_type.radius).size(); line++, i++) {
        for(int j=newPosition.limits_row.at(guard_type.radius)[line].first; j<=newPosition.limits_row.at(guard_type.radius)[line].second; j++) {
            ii = i - newPosition.x;
            jj = j - newPosition.y;
            // if(newPosition.shed[i][j] && (covered[i][j]-(int)oldPosition.shed[i][j])==0) {
            //     if(ii <= 0 && jj > 0) {
            //         if(ii > -jj)
            //             sector_covered_points[0]++;
            //         else
            //             sector_covered_points[1]++;
            //     } else if(ii < 0 && jj <= 0) {
            //         if(ii < jj)
            //             sector_covered_points[2]++;
            //         else
            //             sector_covered_points[3]++;
            //     } else if(ii >= 0 && jj < 0) {
            //         if(ii < -jj)
            //             sector_covered_points[4]++;
            //         else
            //             sector_covered_points[5]++;
            //     } else if(ii > 0 && jj >= 0) {
            //         if(ii > jj)
            //             sector_covered_points[6]++;
            //         else
            //             sector_covered_points[7]++;
            //     }
            // }
            if(newPosition.shed[i][j]) {
                if(covered[i][j]-oldPosition.shed[i][j]==0) { //aaaaaaaaaa
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj)
                            sector_covered_points[0]++;
                        else
                            sector_covered_points[1]++;
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj)
                            sector_covered_points[2]++;
                        else
                            sector_covered_points[3]++;
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj)
                            sector_covered_points[4]++;
                        else
                            sector_covered_points[5]++;
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj)
                            sector_covered_points[6]++;
                        else
                            sector_covered_points[7]++;
                    }
                } else if(covered[i][j]-oldPosition.shed[i][j]==1) {
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj)
                            sector_twiceCovered_points[0]++;
                        else
                            sector_twiceCovered_points[1]++;
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj)
                            sector_twiceCovered_points[2]++;
                        else
                            sector_twiceCovered_points[3]++;
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj)
                            sector_twiceCovered_points[4]++;
                        else
                            sector_twiceCovered_points[5]++;
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj)
                            sector_twiceCovered_points[6]++;
                        else
                            sector_twiceCovered_points[7]++;
                    }
                }
            }
        }
    }

    start = std::max(0, oldPosition.x - radius);
    stop = std::min((int)covered.size()-1, oldPosition.x + radius);
    i = start;
    
    for(int line=0; line<oldPosition.limits_row.at(guard_type.radius).size(); line++, i++) {
        for(int j=oldPosition.limits_row.at(guard_type.radius)[line].first; j<=oldPosition.limits_row.at(guard_type.radius)[line].second; j++) {
            ii = i - oldPosition.x;
            jj = j - oldPosition.y;
            // if(oldPosition.shed[i][j] && covered[i][j]==1) {
            //     if(ii <= 0 && jj > 0) {
            //         if(ii > -jj)
            //             sector_old_covered_points[0]++;
            //         else
            //             sector_old_covered_points[1]++;
            //     } else if(ii < 0 && jj <= 0) {
            //         if(ii < jj)
            //             sector_old_covered_points[2]++;
            //         else
            //             sector_old_covered_points[3]++;
            //     } else if(ii >= 0 && jj < 0) {
            //         if(ii < -jj)
            //             sector_old_covered_points[4]++;
            //         else
            //             sector_old_covered_points[5]++;
            //     } else if(ii > 0 && jj >= 0) {
            //         if(ii > jj)
            //             sector_old_covered_points[6]++;
            //         else
            //             sector_old_covered_points[7]++;
            //     }
            // }
            if(oldPosition.shed[i][j]) {
                if(covered[i][j]==1) {
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj)
                            sector_old_covered_points[0]++;
                        else
                            sector_old_covered_points[1]++;
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj)
                            sector_old_covered_points[2]++;
                        else
                            sector_old_covered_points[3]++;
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj)
                            sector_old_covered_points[4]++;
                        else
                            sector_old_covered_points[5]++;
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj)
                            sector_old_covered_points[6]++;
                        else
                            sector_old_covered_points[7]++;
                    }
                } else if(covered[i][j]==2) {
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj)
                            sector_old_twiceCovered_points[0]++;
                        else
                            sector_old_twiceCovered_points[1]++;
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj)
                            sector_old_twiceCovered_points[2]++;
                        else
                            sector_old_twiceCovered_points[3]++;
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj)
                            sector_old_twiceCovered_points[4]++;
                        else
                            sector_old_twiceCovered_points[5]++;
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj)
                            sector_old_twiceCovered_points[6]++;
                        else
                            sector_old_twiceCovered_points[7]++;
                    }
                }
            }
        }
    }
}

GuardPos::GuardPos(const GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = position.x; this->y = position.y;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_covered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]
    
    int radius = guard_type.radius*covered.size()/100;
    int start = std::max(0, position.x - radius);
    int stop = std::min((int)covered.size()-1, position.x + radius);
    int i = start;

    int ii, jj;
    
    for(int line=0; line<position.limits_row.at(guard_type.radius).size(); line++, i++) {
        for(int j=position.limits_row.at(guard_type.radius)[line].first; j<=position.limits_row.at(guard_type.radius)[line].second; j++) {
            ii = i - position.x;
            jj = j - position.y;
            if(position.shed[i][j]) {
                if(covered[i][j]==0) {
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj)
                            sector_covered_points[0]++;
                        else
                            sector_covered_points[1]++;
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj)
                            sector_covered_points[2]++;
                        else
                            sector_covered_points[3]++;
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj)
                            sector_covered_points[4]++;
                        else
                            sector_covered_points[5]++;
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj)
                            sector_covered_points[6]++;
                        else
                            sector_covered_points[7]++;
                    }
                } else if(covered[i][j]==1) {
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj)
                            sector_twiceCovered_points[0]++;
                        else
                            sector_twiceCovered_points[1]++;
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj)
                            sector_twiceCovered_points[2]++;
                        else
                            sector_twiceCovered_points[3]++;
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj)
                            sector_twiceCovered_points[4]++;
                        else
                            sector_twiceCovered_points[5]++;
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj)
                            sector_twiceCovered_points[6]++;
                        else
                            sector_twiceCovered_points[7]++;
                    }
                }
            }
        }
    }
}

long long int GuardPos::calculateOF_inc(const int angle, long long int& numCovered_diff, long long int& numTwiceCovered_diff, int nrows, bool hasOld=false) {
    long long int OF_inc = 0;
    long long int numCovered_inc = 0;
    long long int numTwiceCovered_inc = 0;
    long long int numOldCovered_inc = 0;
    long long int numOldTwiceCovered_inc = 0;

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]

    int angle_min = angle/45;
    int angle_max = ((angle + this->guard->angle)%360)/45;

    if(angle_min < angle_max) {
        for(int i = angle_min; i<angle_max; i++) {
            numCovered_inc += this->sector_covered_points[i];
            numTwiceCovered_inc += this->sector_twiceCovered_points[i];
            numOldCovered_inc += this->sector_old_covered_points[i];
            numOldTwiceCovered_inc += this->sector_old_twiceCovered_points[i];
        }
    } else {
        for(int i = angle_min; i<8; i++) {
            numCovered_inc += this->sector_covered_points[i];
            numTwiceCovered_inc += this->sector_twiceCovered_points[i];
            numOldCovered_inc += this->sector_old_covered_points[i];
            numOldTwiceCovered_inc += this->sector_old_twiceCovered_points[i];
        }
        for(int i = 0; i<angle_max; i++) {
            numCovered_inc += this->sector_covered_points[i];
            numTwiceCovered_inc += this->sector_twiceCovered_points[i];
            numOldCovered_inc += this->sector_old_covered_points[i];
            numOldTwiceCovered_inc += this->sector_old_twiceCovered_points[i];
        }
    }

        // OF = (80000*numCovered + 20000*numTwiceCovered)/(dem->nrows*dem->nrows) - iCost;
    if(hasOld)
        OF_inc = 4*(numCovered_inc - numOldCovered_inc) + (numTwiceCovered_inc - numOldTwiceCovered_inc);
    else
        OF_inc = 4*numCovered_inc + numTwiceCovered_inc - this->guard->icost;

    numCovered_diff = numCovered_inc - numOldCovered_inc;
    numTwiceCovered_diff = numTwiceCovered_inc - numOldTwiceCovered_inc;

    return OF_inc;
}

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

bool Situation::calculate_possibilities() {
    long long int OF_inc;
    long long int numCovered_inc;
    long long int numTwiceCovered_inc;
    for(const Observer& position: dem->best_observers) {
        for(int i=0; i<guard_types->size(); i++) {
            if(guard_amount[i] == 0)
                continue;
            GuardPos possibility(guard_types->at(i), position, covered);
            if(guard_types->at(i).angle == 360) {
                OF_inc = possibility.calculateOF_inc(0, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
                possibilities.push_back(NewAlloc(0, possibility, position, i, OF_inc, numCovered_inc, numTwiceCovered_inc));
            }
            else {
                for(int angle: angles) {
                    OF_inc = possibility.calculateOF_inc(angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, false);
                    possibilities.push_back(NewAlloc(angle, possibility, position, i, OF_inc, numCovered_inc, numTwiceCovered_inc));
                }
            }
        }
    }
    return !possibilities.empty();
}

void Situation::updateCovered(Allocation& alloc) {
    const GuardType* guard = alloc.guard;
    const Observer* position = alloc.position;
    int x = position->x; int y = position->y;
    
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
            if(position->shed[i][j]) {
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
                }
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
    guard_amount[newAlloc.alloc.guardidx]--;
    possibilities.clear();
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

void Situation::updateCovered(Allocation& alloc, const Observer* oldPos) {
    const GuardType* guard = alloc.guard;
    const Observer* position = alloc.position;
    int x = position->x; int y = position->y;
    
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
            if(position->shed[i][j]) {
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
                }
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
            if(oldPos->shed[i][j]) {
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
                }
            }
        }
    }
}

void Situation::replaceAlloc(NewAlloc& newAlloc, std::list<Allocation>::iterator& oldAlloc) {
    auto newIt = allocations.insert(oldAlloc, newAlloc.alloc);
    OF += newAlloc.OF_inc;
    numCovered += newAlloc.numCovered_inc;
    numTwiceCovered += newAlloc.numTwiceCovered_inc;
    updateCovered(newAlloc.alloc, oldAlloc->position);
    allocations.erase(oldAlloc);
    oldAlloc = newIt;
    possibilities.clear();
}

void Situation::switchPos(std::list<Allocation>::iterator& alloc) {
    const GuardType* guard = alloc->guard;
    const Observer* pos = alloc->position;
    int angle = alloc->angle;
    int guardidx = alloc->guardidx;

    for(const Observer& newPosition: dem->best_observers) {
        if(rand()%100 >= 10) continue;
        GuardPos possibility(*guard, *pos, newPosition, covered);

        long long int numCovered_inc, numTwiceCovered_inc;
        long long int OF_inc = possibility.calculateOF_inc(angle, numCovered_inc, numTwiceCovered_inc, dem->nrows, true);

        possibilities.push_back(NewAlloc(angle, possibility, newPosition, guardidx, OF_inc, numCovered_inc, numTwiceCovered_inc));
    }
}
