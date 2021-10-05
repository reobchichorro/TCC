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

Allocation::Allocation(const int angle, GuardType& guard_type, const Observer& position) {
    this->guard = &guard_type;
    this->position = &position;
    this->angle = angle;
}

Allocation::Allocation(const int angle, GuardType* guard_type, const Observer& position) {
    this->guard = guard_type;
    this->position = &position;
    this->angle = angle;
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
}

GuardPos::GuardPos(GuardType& guard_type, const Observer& oldPosition, const Observer& newPosition, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = newPosition.x; this->y = newPosition.y;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_uncovered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315

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
                        sector_uncovered_points[0]++;
                    else
                        sector_uncovered_points[1]++;
                } else if(ii < 0 && jj <= 0) {
                    if(ii < jj)
                        sector_uncovered_points[2]++;
                    else
                        sector_uncovered_points[3]++;
                } else if(ii >= 0 && jj < 0) {
                    if(ii < -jj)
                        sector_uncovered_points[4]++;
                    else
                        sector_uncovered_points[5]++;
                } else if(ii > 0 && jj >= 0) {
                    if(ii > jj)
                        sector_uncovered_points[6]++;
                    else
                        sector_uncovered_points[7]++;
                }
            }
        }
    }
}

GuardPos::GuardPos(GuardType& guard_type, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    this->guard = &guard_type;
    this->x = position.x; this->y = position.y;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315
    sector_uncovered_points = {0, 0, 0, 0, 0, 0, 0, 0};

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

NewAlloc::NewAlloc(const int angle, GuardPos& guardPos, const Observer& position, const std::vector<std::vector<short int> >& covered) {
    alloc = Allocation(angle, guardPos.guard, position);
    OF_inc = 0;
    numCovered_inc = 0;

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]

    int angle_min = angle/45;
    int angle_max = ((angle + guardPos.guard->angle)%360)/45;

    if(angle_min < angle_max) {
        for(int i = angle_min; i<angle_max; i++) {
            numCovered_inc += guardPos.sector_covered_points[i] - guardPos.sector_uncovered_points[i];
        }
    } else {
        for(int i = angle_min; i<8; i++) {
            numCovered_inc += guardPos.sector_covered_points[i] - guardPos.sector_uncovered_points[i];
        }
        for(int i = 0; i<angle_max; i++) {
            numCovered_inc += guardPos.sector_covered_points[i] - guardPos.sector_uncovered_points[i];
        }
    }

    OF_inc = (long double)numCovered_inc / guardPos.guard->icost;

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

Situation::Situation(std::vector<GuardType>& guard_types, Terrain& dem) {
    this->guard_types = &guard_types;
    this->dem = &dem;
    covered = std::vector<std::vector<short int> >(this->dem->nrows, std::vector<short int>(this->dem->nrows, 0));
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

bool Situation::calculate_possibilities(std::vector<GuardType>& guard_types) {
    for(const Observer& position: dem->best_observers) {
        for(GuardType& guard_type: guard_types) {
            if(guard_type.amount == 0)
                continue;
            GuardPos possibility(guard_type, position, covered);
            if(guard_type.angle == 360)
                possibilities.push_back(NewAlloc(0, possibility, position, covered));
            else {
                for(int angle: angles)
                    possibilities.push_back(NewAlloc(angle, possibility, position, covered));
            }
        }
    }
    return !possibilities.empty();
}

void Situation::updateCovered(Allocation& alloc) {
    GuardType* guard = alloc.guard;
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
    updateCovered(newAlloc.alloc);
    newAlloc.alloc.guard->amount--;
    possibilities.clear();
}

void Situation::updateCovered(Allocation& alloc, const Observer* oldPos) {
    GuardType* guard = alloc.guard;
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
    GuardType* guard = alloc->guard;
    const Observer* pos = alloc->position;
    int angle = alloc->angle;

    for(const Observer& newPosition: dem->best_observers) {
        GuardPos possibility(*guard, *pos, newPosition, covered);
        possibilities.push_back(NewAlloc(angle, possibility, newPosition, covered));
    }
}

int Intersection::orientation(const ij& p, const ij& q, const ij& r) {
    double determinant = (p.j() - q.j()) * (q.i() - r.j()) - (q.j() - r.i()) * (p.i() - q.i());
    if(determinant == 0.0) return 0;
    return (determinant > 0)? 1: -1;
}

bool Intersection::checkIntersection(const ij& as, const ij& af, const ij& bs, const ij& bf) {
    if(
        (orientation(as, af, bs) != orientation(as, af, bf) )
        &&
        (orientation(bs, bf, af) != orientation(bs, bf, af) )
    ) {
        // cerr << "Edges " << a.getLabel() << " and " << b.getLabel() << " intersect.\n";
        return true;
    }
    else {
        // cerr << "Edges " << a.getLabel() << " and " << b.getLabel() << " do not intersect.\n";
        return false;
    }
}

bool Intersection::check_line_crosses_square(const ij& s, const ij& e, const ij& sq) {
    std::vector<ij> sq_points = {ij(sq.i()-1, sq.j()-1, true), ij(sq.i()-1, sq.j(), true), ij(sq.i(), sq.j()-1, true), ij(sq.i(), sq.j(), true)};

    return checkIntersection(s, e, sq_points[0], sq_points[1]) ||
        checkIntersection(s, e, sq_points[0], sq_points[2]) ||
        checkIntersection(s, e, sq_points[1], sq_points[3]) ||
        checkIntersection(s, e, sq_points[2], sq_points[3]);
}

void Intersection::fill_squares_to_check(const ij& s, const ij& e) {
    int i0 = s.i();
    int j0 = s.j();
    int i1 = e.i();
    int j1 = e.j();

    for(int i=i0; i<=i1; i++) {
        for(int j=j0; j<=j1; j++) {
            if(check_line_crosses_square(s, e, ij(i, j)))
                squares_to_check.push_back(ij(i,j));
        }
    }
}

bool Intersection::check_if_square_blocks_view(const ij& s, const ij& e, const ij& sq, const Terrain& dem) {
    if(dem.height[sq.idxi()][sq.idxj()] < 0)
        return true;
    
    int res = dem.resolution;

    xyz no_h(res*(e.j() - s.j()), res*(e.i() - s.i()), 0);
    double view_angle = (dem.height[e.idxi()][e.idxj()] - dem.height[s.idxi()][s.idxj()]) / sqrt(no_h.dot(no_h));

    no_h.x = res*(sq.j() - s.j()); no_h.y = res*(sq.i() - s.i());
    double sq_angle = (dem.height[sq.idxi()][sq.idxj()] - dem.height[s.idxi()][s.idxj()]) / sqrt(no_h.dot(no_h));

    return view_angle >= sq_angle; // if true, square doesn't block view
}

bool Intersection::check_if_s_views_e(const ij& s, const ij& e, const Terrain& dem) {
    fill_squares_to_check(s,e);
    for(int i=0; i<squares_to_check.size(); i++)
        check_if_square_blocks_view(s, e, squares_to_check[i], dem);
    return true;
}

double Intersection::calculate_view_angle(const ij& s, const ij& e, const Terrain& dem) {
    int res = dem.resolution;
    if (dem.height[e.idxi()][e.idxj()] == -1 || dem.height[s.idxi()][s.idxj()] == -1) {
        view_angle[s.idxi()][s.idxj()][e.idxi()][e.idxj()] = 1.0; // arrumar esse caso
    }

    xyz no_h(res*(e.j() - s.j()), res*(e.i() - s.i()), 0);
    // view_angle[s.idxi()][s.idxj()][e.idxi()][e.idxj()] = (dem.height[e.idxi()][e.idxj()] - dem.height[s.idxi()][s.idxj()]) / sqrt(no_h.dot(no_h));
    return (dem.height[e.idxi()][e.idxj()] - dem.height[s.idxi()][s.idxj()]) / sqrt(no_h.dot(no_h));
}

void Intersection::fill_view_angle(const Terrain& dem) {
    // view_angle = std::vector<std::vector<std::vector<std::vector<double> > > >(dem.height.size(), 
    //     std::vector<std::vector<std::vector<double> > >(dem.height[0].size(), 
    //         std::vector<std::vector<double> >(dem.height.size(), 
    //             std::vector<double>(dem.height[0].size())
    //         )
    //     )
    // );
    for(int i=0; i<dem.height.size(); i+=30) {
        for(int j=0; j<dem.height[i].size(); j+=30) {
            for(int k=0; k<dem.height.size(); k+=30) {
                for(int l=0; l<dem.height.size(); l+=30) {
                    ij s(i,j), e(k,l);
                    std::cout << i << ',' << j << "->" << k << ',' << l << " - " << calculate_view_angle(s, e, dem) << "\n";
                }
            }
        }
    }
    int m = 0;
}
