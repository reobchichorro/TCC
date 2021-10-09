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
    sector_old_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]
    
    int radius = guard_type.radius*covered.size()/100;
    int start = std::max(0, newPosition.x - radius);
    int stop = std::min((int)covered.size()-1, newPosition.x + radius);
    int i = start;

    int ii, jj;
    
    for(int line=0; line<newPosition.limits_row.at(guard_type.radius).size(); line++, i++) {
        for(int j=newPosition.limits_row.at(guard_type.radius)[line].first; j<=newPosition.limits_row.at(guard_type.radius)[line].second; j++) {
            ii = i - newPosition.x;
            jj = j - newPosition.y;
            if(newPosition.shed[i][j] && (covered[i][j]-(int)oldPosition.shed[i][j])==0) {
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
            if(oldPosition.shed[i][j] && covered[i][j]==1) {
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
            }
        }
    }
}

GuardPos::GuardPos(const GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = position.x; this->y = position.y;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_old_covered_points = {0, 0, 0, 0, 0, 0, 0, 0};

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
            if(covered[i][j]==0 && position.shed[i][j]) {
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
            }
        }
    }
}

NewAlloc::NewAlloc(const int angle, GuardPos& guardPos, const Observer& position, const std::vector<std::vector<short int> >& covered, int gidx) {
    alloc = Allocation(angle, guardPos.guard, position, gidx);
    OF_inc = 0;
    numCovered_inc = 0;
    int numOldCovered_inc = 0;

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]

    int angle_min = angle/45;
    int angle_max = ((angle + guardPos.guard->angle)%360)/45;

    if(angle_min < angle_max) {
        for(int i = angle_min; i<angle_max; i++) {
            numCovered_inc += guardPos.sector_covered_points[i];
            numOldCovered_inc += guardPos.sector_old_covered_points[i];
        }
    } else {
        for(int i = angle_min; i<8; i++) {
            numCovered_inc += guardPos.sector_covered_points[i];
            numOldCovered_inc += guardPos.sector_old_covered_points[i];
        }
        for(int i = 0; i<angle_max; i++) {
            numCovered_inc += guardPos.sector_covered_points[i];
            numOldCovered_inc += guardPos.sector_old_covered_points[i];
        }
    }

    if(numOldCovered_inc > 0)
        OF_inc = 4*(numCovered_inc - numOldCovered_inc);
    else
        OF_inc = 4*numCovered_inc - guardPos.guard->icost;

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
    OF = 0.0;
    numCovered = 0;
    iCost = 0;
    mCost = 0;
}

long double Situation::calculate_OF() {
    long double ans = 0.0;
    for(int i=0; i<dem->nrows; i++) {
        for(int j=0; j<dem->nrows; j++) {
            ans += covered[i][j];
        }
    }
    OF = ans;
    return ans;
}

bool Situation::calculate_possibilities() {
    for(const Observer& position: dem->best_observers) {
        for(int i=0; i<guard_types->size(); i++) {
            if(guard_amount[i] == 0)
                continue;
            GuardPos possibility(guard_types->at(i), position, covered);
            if(guard_types->at(i).angle == 360)
                possibilities.push_back(NewAlloc(0, possibility, position, covered, i));
            else {
                for(int angle: angles)
                    possibilities.push_back(NewAlloc(angle, possibility, position, covered, i));
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
    NewAlloc toAdd(angle, possibility, dem->best_observers[j], covered, i);
    insertNewAlloc(toAdd);
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
        GuardPos possibility(*guard, *pos, newPosition, covered);
        possibilities.push_back(NewAlloc(angle, possibility, newPosition, covered, guardidx));
    }
}
