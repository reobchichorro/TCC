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

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

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

GuardPos::GuardPos(const GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = position.x; this->y = position.y;
    int h = guard_type.height;
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
            if(position.shed.at(h)[i][j]) {
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

GuardPos::GuardPos(const GuardType& guard_type, const Observer& oldPosition, const Observer& newPosition, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = newPosition.x; this->y = newPosition.y;
    int h = guard_type.height;
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
            if(newPosition.shed.at(h)[i][j]) {
                if(covered[i][j]-oldPosition.shed.at(h)[i][j]==0) {
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
                } else if(covered[i][j]-oldPosition.shed.at(h)[i][j]==1) {
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
            if(oldPosition.shed.at(h)[i][j]) {
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

GuardPos::GuardPos(const GuardType& guard_type, const GuardType& newGuard, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    this->guard = &newGuard;
    this->x = position.x; this->y = position.y;
    int h = guard_type.height;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_covered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    int maxRadius = (std::max(guard_type.radius, newGuard.radius));
    
    int oldRadius = guard_type.radius*covered.size()/100;
    int newRadius = newGuard.radius*covered.size()/100;
    int start = std::max(0, position.x - std::max(oldRadius, newRadius));
    int stop = std::min((int)covered.size()-1, position.x + std::max(oldRadius, newRadius));
    int i = start;

    int ii, jj;
    bool isOld, isNew;
    
    for(int line=0; line<position.limits_row.at(maxRadius).size(); line++, i++) {
        for(int j=position.limits_row.at(maxRadius)[line].first; j<=position.limits_row.at(maxRadius)[line].second; j++) {
            ii = i - position.x;
            jj = j - position.y;
            
            isOld = (ii*ii + jj*jj)<=oldRadius*oldRadius; isNew = (ii*ii + jj*jj)<=newRadius*newRadius;

            if(position.shed.at(h)[i][j]) {
                if(covered[i][j]-isOld==0 && isNew) {
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
                } else if(covered[i][j]-isOld==1 && isNew) {
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

                if(covered[i][j]==1 && isOld) {
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
                } else if(covered[i][j]==2 && isOld) {
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

GuardPos::GuardPos(const GuardType& guard_type, const int oldAngle, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = position.x; this->y = position.y;
    int h = guard_type.height;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_covered_points = {0, 0, 0, 0, 0, 0, 0, 0};
    sector_old_twiceCovered_points = {0, 0, 0, 0, 0, 0, 0, 0};

    int angle_min = oldAngle/45;
    int angle_max = ((oldAngle + guard_type.angle)%360)/45;
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
    
    int radius = guard_type.radius*covered.size()/100;
    int start = std::max(0, position.x - radius);
    int stop = std::min((int)covered.size()-1, position.x + radius);
    int i = start;

    int ii, jj;
    bool isOld, isNew;
    
    for(int line=0; line<position.limits_row.at(guard_type.radius).size(); line++, i++) {
        for(int j=position.limits_row.at(guard_type.radius)[line].first; j<=position.limits_row.at(guard_type.radius)[line].second; j++) {
            ii = i - position.x;
            jj = j - position.y;

            if(position.shed.at(h)[i][j]) {
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
                        if(ii > -jj) {
                            sector_covered_points[0] += sectors[0];
                            sector_old_covered_points[0]++;
                            sector_twiceCovered_points[0]+= !sectors[0];
                        }
                        else {
                            sector_covered_points[1] += sectors[1];
                            sector_old_covered_points[1]++;
                            sector_twiceCovered_points[1] += !sectors[1];
                        }
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj) {
                            sector_covered_points[2] += sectors[2];
                            sector_old_covered_points[2]++;
                            sector_twiceCovered_points[2] += !sectors[2];
                        }
                        else {
                            sector_covered_points[3] += sectors[3];
                            sector_old_covered_points[3]++;
                            sector_twiceCovered_points[3] += !sectors[3];
                        }
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj) {
                            sector_covered_points[4] += sectors[4];
                            sector_old_covered_points[4]++;
                            sector_twiceCovered_points[4] += !sectors[4];
                        }
                        else {
                            sector_covered_points[5] += sectors[5];
                            sector_old_covered_points[5]++;
                            sector_twiceCovered_points[5] += !sectors[5];
                        }
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj) {
                            sector_covered_points[6] += sectors[6];
                            sector_old_covered_points[6]++;
                            sector_twiceCovered_points[6] += !sectors[6];
                        }
                        else {
                            sector_covered_points[7] += sectors[7];
                            sector_old_covered_points[7]++;
                            sector_twiceCovered_points[7] += !sectors[7];
                        }
                    }
                }  else if(covered[i][j]==2) {
                    if(ii <= 0 && jj > 0) {
                        if(ii > -jj) {
                            sector_old_twiceCovered_points[0]++;
                            sector_twiceCovered_points[0] += sectors[0];
                        }
                        else {
                            sector_old_twiceCovered_points[1]++;
                            sector_twiceCovered_points[1] += sectors[1];
                        }
                    } else if(ii < 0 && jj <= 0) {
                        if(ii < jj) {
                            sector_old_twiceCovered_points[2]++;
                            sector_twiceCovered_points[2] += sectors[2];
                        }
                        else {
                            sector_old_twiceCovered_points[3]++;
                            sector_twiceCovered_points[3] += sectors[3];
                        }
                    } else if(ii >= 0 && jj < 0) {
                        if(ii < -jj) {
                            sector_old_twiceCovered_points[4]++;
                            sector_twiceCovered_points[4] += sectors[4];
                        }
                        else {
                            sector_old_twiceCovered_points[5]++;
                            sector_twiceCovered_points[5] += sectors[5];
                        }
                    } else if(ii > 0 && jj >= 0) {
                        if(ii > jj) {
                            sector_old_twiceCovered_points[6]++;
                            sector_twiceCovered_points[6] += sectors[6];
                        }
                        else {
                            sector_old_twiceCovered_points[7]++;
                            sector_twiceCovered_points[7] += sectors[7];
                        }
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

long long int GuardPos::calculateOF_incGuard(const int angle, long long int oldICost, long long int& numCovered_diff, long long int& numTwiceCovered_diff, int nrows, bool hasOld=false) {
    long long int OF_inc = 0;
    long long int numCovered_inc = 0;
    long long int numTwiceCovered_inc = 0;
    long long int numOldCovered_inc = 0;
    long long int numOldTwiceCovered_inc = 0;

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
        OF_inc = 4*(numCovered_inc - numOldCovered_inc) + (numTwiceCovered_inc - numOldTwiceCovered_inc) - (this->guard->icost - oldICost);
    else
        OF_inc = 4*numCovered_inc + numTwiceCovered_inc - this->guard->icost;

    numCovered_diff = numCovered_inc - numOldCovered_inc;
    numTwiceCovered_diff = numTwiceCovered_inc - numOldTwiceCovered_inc;

    return OF_inc;
}

long long int GuardPos::calculateOF_incAngle(const int oldAngle, const int newAngle, long long int& numCovered_diff, long long int& numTwiceCovered_diff, int nrows, bool hasOld=false) {
    long long int OF_inc = 0;
    long long int numCovered_inc = 0;
    long long int numTwiceCovered_inc = 0;
    long long int numOldCovered_inc = 0;
    long long int numOldTwiceCovered_inc = 0;

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]

    int angle_min = oldAngle/45;
    int angle_max = ((oldAngle + this->guard->angle)%360)/45;

    if(angle_min < angle_max) {
        for(int i = angle_min; i<angle_max; i++) {
            numOldCovered_inc += this->sector_old_covered_points[i];
            numOldTwiceCovered_inc += this->sector_old_twiceCovered_points[i];
        }
    } else {
        for(int i = angle_min; i<8; i++) {
            numOldCovered_inc += this->sector_old_covered_points[i];
            numOldTwiceCovered_inc += this->sector_old_twiceCovered_points[i];
        }
        for(int i = 0; i<angle_max; i++) {
            numOldCovered_inc += this->sector_old_covered_points[i];
            numOldTwiceCovered_inc += this->sector_old_twiceCovered_points[i];
        }
    }

    angle_min = newAngle/45;
    angle_max = ((newAngle + this->guard->angle)%360)/45;

    if(angle_min < angle_max) {
        for(int i = angle_min; i<angle_max; i++) {
            numCovered_inc += this->sector_covered_points[i];
            numTwiceCovered_inc += this->sector_twiceCovered_points[i];
        }
    } else {
        for(int i = angle_min; i<8; i++) {
            numCovered_inc += this->sector_covered_points[i];
            numTwiceCovered_inc += this->sector_twiceCovered_points[i];
        }
        for(int i = 0; i<angle_max; i++) {
            numCovered_inc += this->sector_covered_points[i];
            numTwiceCovered_inc += this->sector_twiceCovered_points[i];
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
