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

#define eps 1e-6
#define all(v) v.begin(),v.end()
typedef std::string str;

SubAlloc::SubAlloc(std::list<Allocation>::iterator& oldAlloc, Allocation& newAlloc, const int nrows, const std::vector<std::vector<short int>>& covered) {
    // alloc = Allocation(angle, guardPos.guard, position, gidx);
    this->oldAlloc = oldAlloc;
    this->OF_diff = 0;
    this->numCovered_diff = 0;
    this->numTwiceCovered_diff = 0;
    this->icost_diff = newAlloc.guard->icost - oldAlloc->guard->icost;

    //Setting up heights
    int oh = oldAlloc->guard->height;
    int nh = newAlloc.guard->height;

    //Setting up radii
    int oldRadius = oldAlloc->guard->radius*nrows/100;
    int newRadius = newAlloc.guard->radius*nrows/100;

    int oi, oj, ni, nj;
    bool insideOld, insideNew;

    //Setting up angles
    int angle_min = oldAlloc->angle/45;
    int angle_max = ((oldAlloc->angle + oldAlloc->guard->angle)%360)/45;
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

    angle_min = newAlloc.angle/45;
    angle_max = ((newAlloc.angle + newAlloc.guard->angle)%360)/45;
    std::vector<bool> newSectors(8, false);
    if(angle_min < angle_max) {
        for(int idx = angle_min; idx<angle_max; idx++)
            newSectors[idx] = true;
    } else {
        for(int idx = angle_min; idx<8; idx++)
            newSectors[idx] = true;
        for(int idx = 0; idx<angle_max; idx++)
            newSectors[idx] = true;
    }

    int oldSector, newSector;
    bool oldAngle, newAngle;

    bool wasCovering, willCover;

    if(oldAlloc->position == newAlloc.position) {
        int maxRadius = std::max(oldAlloc->guard->radius, newAlloc.guard->radius);
        int maxRad = maxRadius*nrows/100;
        int start = std::max(0, newAlloc.position->x - maxRad);
        int stop = std::min((int)covered.size()-1, newAlloc.position->x + maxRad);
        int i = start;

        for(int line=0; line<newAlloc.position->limits_row.at(maxRadius).size(); line++, i++) {
            for(int j=newAlloc.position->limits_row.at(maxRadius)[line].first; j<=newAlloc.position->limits_row.at(maxRadius)[line].second; j++) {
                oi = i - oldAlloc->position->x; oj = j - oldAlloc->position->y;
                ni = i - newAlloc.position->x; nj = j - newAlloc.position->y;

                insideOld = oi*oi + oj*oj <= oldRadius*oldRadius;
                insideNew = ni*ni + nj*nj <= newRadius*newRadius;

                oldSector = getSector(oi, oj);
                newSector = getSector(ni, nj);
                oldAngle = (oldSector != 8) ? oldSectors[oldSector] : true;
                newAngle = (newSector != 8) ? newSectors[newSector] : true;

                wasCovering = (covered[i][j] > 0) && insideOld && oldAngle && oldAlloc->position->shed.at(oh)[i][j];
                willCover = insideNew && newAngle && newAlloc.position->shed.at(nh)[i][j];

                if(covered[i][j] == 0 && !wasCovering && willCover)
                    numCovered_diff++;
                else if(covered[i][j] == 1 && !wasCovering && willCover)
                    numTwiceCovered_diff++;
                else if(covered[i][j] == 1 && wasCovering && !willCover)
                    numCovered_diff--;
                else if(covered[i][j] == 2 && wasCovering && !willCover)
                    numTwiceCovered_diff--;
            }
        }
    } else {
        for(int i=0; i<nrows; i++) {
            for(int j=0; j<nrows; j++) {
                oi = i - oldAlloc->position->x; oj = j - oldAlloc->position->y;
                ni = i - newAlloc.position->x; nj = j - newAlloc.position->y;

                insideOld = oi*oi + oj*oj <= oldRadius*oldRadius;
                insideNew = ni*ni + nj*nj <= newRadius*newRadius;

                oldSector = getSector(oi, oj);
                newSector = getSector(ni, nj);
                oldAngle = (oldSector != 8) ? oldSectors[oldSector] : true;
                newAngle = (newSector != 8) ? newSectors[newSector] : true;

                wasCovering = (covered[i][j] > 0) && insideOld && oldAngle && oldAlloc->position->shed.at(oh)[i][j];
                willCover = insideNew && newAngle && newAlloc.position->shed.at(nh)[i][j];

                if(covered[i][j] == 0 && !wasCovering && willCover)
                    numCovered_diff++;
                else if(covered[i][j] == 1 && !wasCovering && willCover)
                    numTwiceCovered_diff++;
                else if(covered[i][j] == 1 && wasCovering && !willCover)
                    numCovered_diff--;
                else if(covered[i][j] == 2 && wasCovering && !willCover)
                    numTwiceCovered_diff--;
            }
        }
    }


    OF_diff = 4*numCovered_diff + numTwiceCovered_diff - icost_diff;
    this->alloc = newAlloc;

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

bool SubAlloc::operator<(const SubAlloc& other) const {
    return this->OF_diff < other.OF_diff;
}

long double SubAlloc::operator-(const SubAlloc& other) {
    return this->OF_diff - other.OF_diff;
}
