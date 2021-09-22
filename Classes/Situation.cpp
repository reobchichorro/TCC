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
    x = -1; y = -1;
    angle = -30;
}

Allocation::Allocation(const int angle, GuardType& guard_type, const Observer& position) {
    this->guard = &guard_type;
    x = position.x;
    y = position.y;
    this->angle = angle;
}

Allocation::Allocation(const int angle, GuardType* guard_type, const int x, const int y) {
    this->guard = guard_type;
    this->x = x;
    this->y = y;
    this->angle = angle;
}

GuardPos::GuardPos(GuardType& guard_type, const Observer& position, const std::vector<std::vector<bool> >& covered) {
    this->guard = &guard_type;
    this->x = position.x; this->y = position.y;
    sector_covered_points = {0, 0, 0, 0, 0, 0, 0, 0}; //0, 45, 90, 135, 180, 225, 270, 315

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
            if(!covered[i][j] && position.shed[i][j]) {
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

NewAlloc::NewAlloc(const int angle, GuardPos& guardPos, const std::vector<std::vector<bool> >& covered) {
    alloc = Allocation(angle, guardPos.guard, guardPos.x, guardPos.y);
    OF_inc = 0;

    // int radius_index = guard_type.radius/5 - 1;

    // position.limits_row[radius_index]
    // position.limits_col[radius_index]

    int angle_min = angle/45;
    int angle_max = ((angle + guardPos.guard->angle)%360)/45;

    if(angle_min < angle_max) {
        for(int i = angle_min; i<angle_max; i++)
            OF_inc += guardPos.sector_covered_points[i];
    } else {
        for(int i = angle_min; i<8; i++)
            OF_inc += guardPos.sector_covered_points[i];
        for(int i = 0; i<angle_max; i++)
            OF_inc += guardPos.sector_covered_points[i];
    }

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

long long int NewAlloc::operator-(const NewAlloc& other) {
    return this->OF_inc - other.OF_inc;
}

Situation::Situation(Terrain& dem) {
    this->dem = &dem;
    covered = std::vector<std::vector<bool> >(this->dem->nrows, std::vector<bool>(this->dem->nrows, false));

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

void Situation::calculate_possibilities(std::vector<GuardType>& guard_types) {
    for(const Observer& position: dem->best_observers) {
        for(GuardType& guard_type: guard_types) {
            GuardPos possibility(guard_type, position, covered);
            if(guard_type.angle == 360)
                possibilities.push_back(NewAlloc(0, possibility, covered));
            else {
                for(int angle: angles)
                    possibilities.push_back(NewAlloc(angle, possibility, covered));
            }
        }
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
