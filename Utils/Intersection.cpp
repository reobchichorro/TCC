#ifndef __INTERSECTION_
#define __INTERSECTION_
class Intersection {
public:
    std::vector<ij> squares_to_check;
    std::vector<std::vector<std::vector<std::vector<double> > > > view_angle;

    int orientation(const ij& p, const ij& q, const ij& r);

    bool checkIntersection(const ij& as, const ij& af, const ij& bs, const ij& bf);

    bool check_line_crosses_square(const ij& s, const ij& e, const ij& sq);

    void fill_squares_to_check(const ij& s, const ij& e);

    bool check_if_square_blocks_view(const ij& s, const ij& e, const ij& sq, const Terrain& dem);

    bool check_if_s_views_e(const ij& s, const ij& e, const Terrain& dem);

    double calculate_view_angle(const ij& s, const ij& e, const Terrain& dem);

    void fill_view_angle(const Terrain& dem);
};
#endif

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
