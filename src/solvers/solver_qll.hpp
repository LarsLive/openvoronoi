/*
 *  Copyright (c) 2010-2012 Anders Wallin (anders.e.e.wallin "at" gmail.com).
 *
 *  This file is part of Openvoronoi
 *  (see https://github.com/aewallin/openvoronoi).
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

//#include <qd/qd_real.h> // http://crd.lbl.gov/~dhbailey/mpdist/

#include "solver.hpp"
#include "common/numeric.hpp"

using namespace ovd::numeric; // sq() chop() quadratic_roots()

namespace ovd {
namespace solvers {


/// \brief quadratic-linear-linear Solver
class QLLSolver : public Solver {
public:

int solve( Site* s1, double k1,
                Site* s2, double k2,
                Site* s3, double k3, std::vector<Solution>& slns ) {
    if (debug && !silent)
        std::cout << "QLLSolver.\n";

    std::vector< Eq<long double> > quads,lins; // equation-parameters, in quad-precision
    boost::array<Site*,3> sites = {{s1,s2,s3}};
    boost::array<double,3> kvals = {{k1,k2,k3}};
    for (unsigned int i=0;i<3;i++) {
        Eq<long double> eqn = sites[i]->eqp_qd( kvals[i] );
        if (sites[i]->is_linear() ) // store site-equations in lins or quads
            lins.push_back( eqn );
        else
            quads.push_back( eqn );
    }
    assert( !quads.empty() );

    if ( lins.size()==1 || lins.size() == 0 ) {
        assert( quads.size() == 3 || quads.size() == 2 );
        for (unsigned int i=1;i<quads.size();i++) {
            quads[i] = quads[i] - quads[0]; // subtract the first quad from the other one(s), to create new linear eqn(s)
            lins.push_back(quads[i]);
        }
    }
    assert( lins.size() == 2);  // At this point, we should have exactly two linear equations.

    // TODO:  pick the solution appraoch with the best numerical stability.
    // call all three permutations
    // index shuffling determines if we solve:
    // x and y in terms of t
    // y and t in terms of x
    // t and x in terms of y
    qll_solver( lins, 0, 1, 2, quads[0], k3, slns);
    qll_solver( lins, 2, 0, 1, quads[0], k3, slns);
    qll_solver( lins, 1, 2, 0, quads[0], k3, slns);

    return slns.size();
}


private:
/// \brief qll solver
// l0 first linear eqn
// l1 second linear eqn
// xi,yi,ti  indexes to shuffle around
// xk, yk, kk, rk = params of one ('last') quadratic site (point or arc)
// solns = output solution triplets (x,y,t) or (u,v,t)
// returns number of solutions found
int qll_solver( const std::vector< Eq<long double> >& lins, int xi, int yi, int ti,
      const Eq<long double>& quad, long double k3, std::vector<Solution>& solns) {
    assert( lins.size() == 2 );
    long double ai = lins[0][xi]; // first linear
    long double bi = lins[0][yi];
    long double ki = lins[0][ti];
    long double ci = lins[0].c;

    long double aj = lins[1][xi]; // second linear
    long double bj = lins[1][yi];
    long double kj = lins[1][ti];
    long double cj = lins[1].c;

    long double d = chop( ai*bj - aj*bi ); // chop! (determinant for 2 linear eqns (?))
    if (d == 0) // no solution can be found!
        return -1;
    // these are the w-equations for qll_solve()
    // (2) u = a1 w + b1
    // (3) v = a2 w + b2
    long double a0 =  (bi*kj - bj*ki) / d;
    long double a1 = -(ai*kj - aj*ki) / d;
    long double b0 =  (bi*cj - bj*ci) / d;
    long double b1 = -(ai*cj - aj*ci) / d;
    // based on the 'last' quadratic of (s1,s2,s3)
    long double aargs[3][2];
    aargs[0][0] = 1.0;
    aargs[0][1] = quad.a;
    aargs[1][0] = 1.0;
    aargs[1][1] = quad.b;
    aargs[2][0] = -1.0;
    aargs[2][1] = quad.k;

    long double isolns[2][3];
    // this solves for w, and returns either 0, 1, or 2 triplets of (u,v,t) in isolns
    // NOTE: indexes of aargs shuffled depending on (xi,yi,ti) !
    int scount = qll_solve( aargs[xi][0], aargs[xi][1],
                            aargs[yi][0], aargs[yi][1],
                            aargs[ti][0], aargs[ti][1],
                            quad.c, // xk*xk + yk*yk - rk*rk,
                            a0, b0,
                            a1, b1, isolns);
    double tsolns[2][3];
    for (int i=0; i<scount; i++) {
        tsolns[i][xi] = (isolns[i][0]);       // u       x
        tsolns[i][yi] = (isolns[i][1]);       // v       y
        tsolns[i][ti] = (isolns[i][2]);       // t       t  chop!
        solns.push_back( Solution( Point( tsolns[i][0], tsolns[i][1] ),
                         tsolns[i][2], (k3) ) );
    }
    //std::cout << " k3="<<kk3<<" qqq_solve found " << scount << " roots\n";
    return scount;
}

/// Solve a system of one quadratic equation, and two linear equations.
///
/// (1) a0 u^2 + b0 u + c0 v^2 + d0 v + e0 w^2 + f0 w + g0 = 0
/// (2) u = a1 w + b1
/// (3) v = a2 w + b2
/// solve (1) for w (can have 0, 1, or 2 roots)
/// then substitute into (2) and (3) to find (u, v, t)
int qll_solve( long double a0, long double b0, long double c0, long double d0,
                      long double e0, long double f0, long double g0,
                      long double a1, long double b1,
                      long double a2, long double b2,
                      long double soln[][3])
{
    //std::cout << "qll_solver()\n";
    // TODO:  optimize using abs(a0) == abs(c0) == abs(d0) == 1
    long double a = chop( (a0*(a1*a1) + c0*(a2*a2) + e0) );
    long double b = chop( (2*a0*a1*b1 + 2*a2*b2*c0 + a1*b0 + a2*d0 + f0) );
    long double c = a0*(b1*b1) + c0*(b2*b2) + b0*b1 + b2*d0 + g0;
    std::vector<long double> roots = quadratic_roots(a, b, c); // solves a*w^2 + b*w + c = 0
    if ( roots.empty() ) { // No roots, no solutions
        return 0;
    } else {
        for (unsigned int i=0; i<roots.size(); i++) {
            long double w = roots[i];
            soln[i][0] = a1*w + b1; // u
            soln[i][1] = a2*w + b2; // v
            soln[i][2] = w;         // t
        }
        return roots.size();
    }
}

};

} // solvers
} // ovd
