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

// notes from the Okabe-Boots-Sugihara book, page 171->:
/* 
 * Distance-function.
 * R1 - region of endpoint pi1
 * R2 - region of endpoint pi2
 * R3 - region of line-segment Li
 *               dist(p,pi1) if  p in R1
 * dist(p,Li) =  dist(p,pi2) if  p in R2
 *               dist(p,Li)  if p in R3
 * 
 * dist(p,Li) = distance from p to L, along perpendicular line to L
 * 
 * = norm(  (x-xi1)   -  dotprod( (x-xi1), (xi2-xi1) ) / ( norm_sq(xi2-xi1) ) * (xi2,xi1) )
 * 
 * 
 * 
 * Vertex - LineSegment
 * Bisectors:
 *  B1: point-point: line
 *  B2: point-line: parabola
 *  B3: line-line: line
 * 
 *  Voronoi Edges:
 *  E1: point pi - point pj. straight line bisecting pi-pj
 *  E2: edge generated by line-segment L's endpoint pi. perpendicular to L, passing through pi
 *  E3: point pi - segment Lj. dist(E3, p) == dist(E3,Lj). Parabolic arc
 *  E4: line Li - Line Lj. straight line bisector
 *  (G): generator segment edge
 * 
 *  Voronoi vertices (see p177 of Okabe book):
 *  V1: generators(pi, pj, pk). edges(E1, E1, E1)
 *     - compute using detH. This is also the circumcenter of the pi,pj,pk triangle
 *  V2: generators(pi, Lj, pj1) point, segment, segment's endpoint. edges(E1, E2, E3)   E1 and E3 are tangent at V2
 *     - ? compute by mirroring pi in the separator and use detH
 *  V3: generators(Li, pj, pk) edges(E1, E3, E3)   E3-edges have common directrix(Li)
 *  V4: generators(Li, Lj, pi1)  edges(E2, E3, E4)  E3-E4 tangent at V4
 *  V5: generators(pi, Lj, Lk) edges (E3, E3, E4)
 *  V6: generators(Li, Lj, Lk) edges(E4, E4, E4)
 *    - this is the incenter of a incircle inscribed by triangle Li,Lj,Lk (or sometimes excenter of excircle if V6 outside triangle?)
 *    - The Cartesian coordinates of the incenter are a weighted average of the coordinates of the three vertices using the side 
 *       lengths of the triangle as weights. (The weights are positive so the incenter lies inside the triangle as stated above.) 
 *      If the three vertices are located at (xa,ya), (xb,yb), and (xc,yc), and the sides opposite these vertices have corresponding 
 *      lengths a, b, and c, then the incenter is at   
 *      (a x_a + b x_b + c x_c)/ (a+b+c) 
 * 
 * bisector formulas
 * x = x1 - x2 - x3*t +/- x4 * sqrt( square(x5+x6*t) - square(x7+x8*t) )
 * (same formula for y-coordinate)
 * line (line/line)
 * parabola (circle/line)
 * hyperbola (circle/circle)
 * ellipse (circle/circle)
 * 
 * line: a1*x + b1*y + c + k*t = 0  (t is amount of offset) k=+1 offset left of line, k=-1 right of line
 * with a*a + b*b = 1
 * 
 * circle: square(x-xc) + square(y-yc) = square(r+k*t)  k=+1 for enlarging circle, k=-1 shrinking
 */
 

#pragma once

#include <boost/graph/adjacency_list.hpp>


//#include <qd/qd_real.h>
#include <sstream>

#include "common/point.hpp"
#include "common/numeric.hpp"

namespace ovd {

#define OUT_EDGE_CONTAINER boost::listS 
#define VERTEX_CONTAINER boost::listS
#define EDGE_LIST_CONTAINER boost::listS

// type of edge-descriptors in the graph
// FIXME, we whould really define these only once, somewhere else..
typedef boost::adjacency_list_traits<OUT_EDGE_CONTAINER, 
                                     VERTEX_CONTAINER, 
                                     boost::bidirectionalS, 
                                     EDGE_LIST_CONTAINER >::edge_descriptor HEEdge;
typedef boost::adjacency_list_traits<OUT_EDGE_CONTAINER, 
                                     VERTEX_CONTAINER, 
                                     boost::bidirectionalS, 
                                     EDGE_LIST_CONTAINER >::vertex_descriptor HEVertex;                                     

/// \brief Offset equation parameters of a Site
///
/// the offset in direction k by a distance t of a general site (point,line,circle) can be expressed as
/// q ( x*x + y*y - t*t ) + a x + b y + c + k t = 0
/// the parameters (q,a,b,k,c) are set as:
/// line:   (0,   a,   b,    k, c          )    line ax+by+c=0  where a*a+b*b=1
/// circle: (1, -2x, -2y, -2kr, x*x+y*y-r*r)    circle center at (x,y) and radius r
/// point:  (1, -2x, -2y,    0, x*x+y*y    )    point at (x,y)
template<class Scalar>
struct Eq {
    
    bool q; ///< true for quadratic, false for linear
    Scalar a; ///< a parameter of line-equation
    Scalar b; ///< b parameter of line equation
    Scalar c; ///< c parameter of line equation
    Scalar k; ///< offset direction parameter
    
    /// default ctor
    Eq<Scalar>() {
        a = Scalar(0);
        b = Scalar(0);
        c = Scalar(0);
        k = Scalar(0);
        q = false;
    }
    /// assignment
    template<class Scalar2>
    Eq<Scalar>& operator=(const Eq<Scalar2>& other) {
        q = other.q;
        a = other.a;
        b = other.b;
        c = other.c;
        k = other.k;
        return *this;
    }
    /// subtract two equations from each other
    Eq<Scalar>& operator-=(const Eq<Scalar>& other) {
        a-=other.a;
        b-=other.b;
        c-=other.c;
        k-=other.k;
        return *this;
    }
    /// subtraction
    const Eq<Scalar> operator-(const Eq<Scalar>& other) const {
        return Eq<Scalar>(*this) -= other;
    }
    
    /// equality
    template <class TScalar>
    bool operator==(const Eq<TScalar>& other) {
        return ( a==other.a && b==other.b && c==other.c );
    }
    /// access parameters through operator[]
    Scalar operator[](int idx) const {
        switch (idx) {
            case 0:
                return a;
            case 1:
                return b;
            case 2:
                return k;
            default:
                assert(0);
                return Scalar(0);
        }
    }
    
};

/// \brief base-class for offset-elements
///
/// preliminary offset-prerensentations. experiental...
class Ofs {
public:
	virtual ~Ofs() {}
    /// string
    virtual std::string str() = 0;
    /// radius, -1 if line
    virtual double radius() = 0; // {return -1;}
    /// center (for arc)
    virtual Point center() =0; //{return Point(0,0);}
    /// start point
    virtual Point start() =0;//{return Point(0,0);}
    /// end point
    virtual Point end() =0; //{return Point(0,0);}
};
/// \brief offset-element of LineSite
class LineOfs : public Ofs {
public:
    /// \param p1 start point
    /// \param p2 end point
    LineOfs(Point p1, Point p2) : _start(p1), _end(p2) {}
    virtual ~LineOfs() {}
    virtual std::string str() {
        std::ostringstream o;
        o << "LineOfs from:"<<_start<<" to " << _end << "\n";
        return o.str();        
    }
    virtual double radius()  {return -1;}
    virtual Point center() {return Point(0,0);}
    virtual Point start() {return _start;}
    virtual Point end() {return _end;}
protected:
    Point _start; ///< start point
    Point _end;  ///< end point
};
/// \brief offset-element of PointSite or ArcSite
class ArcOfs : public Ofs {
public:
    /// \param p1 start Point
    /// \param p2 end Point
    /// \param cen center Point
    /// \param rad radius
    ArcOfs(Point p1, Point p2, Point cen, double rad) : _start(p1), _end(p2), c(cen), r(rad) {}
    virtual ~ArcOfs() {};
    virtual std::string str() {
        std::ostringstream o;
        o << "ArcOfs  from:"<<_start<<" to " << _end << " r="<<r<<"\n";
        return o.str();
    }
    virtual double radius() {return r;}
    virtual Point center() {return c;}
    virtual Point start() {return _start;}
    virtual Point end() {return _end;}
protected:
    Point _start; ///< start
    Point _end;   ///< end
    Point c;      ///< center
    double r;     ///< radius
};

/// Base-class for a voronoi-diagram site, or generator.
class Site {
public:
    /// ctor
    Site(): face(0) {}
    /// dtor
    virtual ~Site() {}
    /// return closest point on site to given point p
    virtual Point apex_point(const Point& p) = 0;
    /// return offset of site
    virtual Ofs* offset(Point, Point) = 0;
    /// position of site for PointSite
    inline virtual const Point position() const {assert(0); return Point(0,0);}
    /// start point of site (for LineSite and ArcSite)
    virtual const Point start() const {assert(0); return Point(0,0);}
    /// end point of site (for LineSite and ArcSite)
    virtual const Point end() const {assert(0); return Point(0,0);}
    /// return equation parameters
    Eq<double> eqp() {return eq;} 
    /// return equation parameters
    Eq<double> eqp(double kk) {
        Eq<double> eq2(eq);
        eq2.k *= kk;
        return eq2;
    } 
    /// return equation parameters
    Eq<long double> eqp_qd(double kk) const {
        Eq<long double> eq2;
        eq2=eq;
        eq2.k *= kk;
        return eq2;
    }
    /// true for LineSite
    bool is_linear() {return isLine(); }
    /// true for PointSite and ArcSite
    bool is_quadratic() {return isPoint();}
    /// x position
    virtual double x() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }
    /// y position
    virtual double y() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }
    /// radius (zero for PointSite)
    virtual double r() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }
    /// offset direction
    virtual double k() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }
    /// LineSite a parameter
    virtual double a() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }
    /// LineSite b parameter
    virtual double b() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }
    /// LineSite c parameter
    virtual double c() const {
        std::cout << " WARNING: never call Site !\n";
        assert(0); 
        return 0;
    }

    /// string output
    virtual std::string str() const =0; //{assert(0); return "Site";}
    /// alternative string output
    virtual std::string str2() const =0; //{assert(0); return "Site";}
    /// true for PointSite
    inline virtual bool isPoint() const { return false;}
    /// true for LineSite
    inline virtual bool isLine() const  { return false;}
    /// true for ArcSite
    inline virtual bool isArc() const  { return false;}
    /// true for CW oriented ArcSite
    virtual bool cw() {return false;}
    /// is given Point in_region ?
    virtual bool in_region(const Point& ) const =0; 
    /*{
        std::cout << " WARNING: never call Site !\n";
        return false;
    }*/
    /// is given Point in region?
    virtual double in_region_t(const Point& ) const { std::cout << " WARNING: never call Site !\n"; return 0; } 
    /// in-region t-valye
    virtual double in_region_t_raw(const Point&) const { assert(0); return -99; }
    /// return edge (if this is a LineSite or ArcSite
    virtual HEEdge edge() {return HEEdge();}
    /// return vertex, if this is a PointSite
    virtual HEVertex vertex() {
        std::cout << " DON'T call Site::vertex() !! \n";
        exit(-1); 
        return HEVertex();
    }
    /// face descriptor type
    typedef unsigned int HEFace;
    /// the HEFace of this Site
    HEFace face;
protected:
    /// equation parameters
    Eq<double> eq;
};

/// vertex Site
class PointSite : public Site {
public:
    /// ctor
    PointSite( const Point& p, HEFace f=0):v(0), _p(p)  {
        face = f;
        eq.q = true;
        eq.a = -2*p.x;
        eq.b = -2*p.y;
        eq.k = 0;
        eq.c = p.x*p.x + p.y*p.y;
    }
    /// ctor
    PointSite( const Point& p, HEFace f, HEVertex vert):  v(vert), _p(p) {
        face = f;
        eq.q = true;
        eq.a = -2*p.x;
        eq.b = -2*p.y;
        eq.k = 0;
        eq.c = p.x*p.x + p.y*p.y;
    }
    ~PointSite() {}
    virtual Point apex_point(const Point& ) { return _p; }
    virtual Ofs* offset(Point p1,Point p2) {
        double rad = (p1-_p).norm();
        return new ArcOfs(p1, p2, _p, rad); 
    }
    inline virtual const Point position() const { return _p; }
    virtual double x() const {return _p.x;}
    virtual double y() const {return _p.y;}
    virtual double r() const {return 0;}
    virtual double k() const {return 0;}
    inline virtual bool isPoint() const {return true;}
    virtual std::string str() const {return "PointSite";}
    virtual std::string str2() const {
        std::string out = "PointSite: ";
        out.append( _p.str() );
        return out;
    }
    virtual bool in_region(const Point& ) const {return true;}
    virtual double in_region_t(const Point& ) const {return -1;}
    virtual HEVertex vertex() {return v;}
    HEVertex v; ///< vertex descriptor of this PointSite
private:
    PointSite():v(0) {} // don't use!
    Point _p; ///< position
};

/// line segment Site
class LineSite : public Site {
public:
    /// create line-site between start and end Point.
    LineSite( const Point& st, const Point& en, double koff, HEFace f = 0): _start(st), _end(en) {
        face = f;
        eq.q = false;
        eq.a = _end.y - _start.y;
        eq.b = _start.x - _end.x;
        eq.k = koff; // ??
        eq.c = _end.x*_start.y - _start.x*_end.y;
        // now normalize
        double d = sqrt( eq.a*eq.a + eq.b*eq.b );
        eq.a /= d;
        eq.b /= d;
        eq.c /= d;
        e = HEEdge();
        assert( fabs( eq.a*eq.a + eq.b*eq.b -1.0 ) < 1e-5);
    }
    /// copy ctor
    LineSite( Site& s ) { // "downcast" like constructor? required??
        eq = s.eqp();
        face = s.face;
        _start = s.start();
        _end = s.end();
    }
    ~LineSite() {}
    virtual Ofs* offset(Point p1,Point p2) {return new LineOfs(p1, p2); }
    
    /// closest point on start-end segment to given point.
    /// project onto line and return either the projected point
    /// or one endpoint of the linesegment
    virtual Point apex_point(const Point& p) {
        Point s_p = p-_start;
        Point s_e = _end - _start;
        double t = s_p.dot(s_e) / s_e.dot(s_e);
        if (t<0)
            return _start;
        if (t>1)
            return _end;
        else {
            return _start + t*(_end-_start);
        }
    }
    virtual std::string str() const {return "LineSite";}
    virtual std::string str2() const {
        std::string out = "LineSite: ";
        out.append( _start.str() );
        out.append( " - " );
        out.append( _end.str() );
        return out;
    }
    virtual bool in_region(const Point& p) const{
        double t = in_region_t(p);
        return ( (t>=0) && (t<=1) );
    }
    virtual double in_region_t(const Point& p) const {
        Point s_p = p-_start;
        Point s_e = _end - _start;
        double t = s_p.dot(s_e) / s_e.dot(s_e);
        double eps = 1e-7;
        if (fabs(t) < eps)  // rounding... UGLY
            t = 0.0;
        else if ( fabs(t-1.0) < eps )
            t = 1.0;
        return t;
    }
    virtual double in_region_t_raw(const Point& p) const {
        Point s_p = p-_start;
        Point s_e = _end - _start;
        double t = s_p.dot(s_e) / s_e.dot(s_e);
        return t;
    }
    inline virtual bool isLine() const {return true;}
    virtual double a() const { return eq.a; }
    virtual double b() const { return eq.b; }
    virtual double c() const { return eq.c; }
    virtual double k() const {
        assert( eq.k==1 || eq.k==-1 );
        return eq.k;
    }
    virtual const Point start() const {return _start;}
    virtual const Point end() const {return _end;}
    virtual HEEdge edge() {return e;}
    
    HEEdge e; ///< edge_descriptor to the ::LINESITE pseudo-edge
private:
    LineSite() {} // don't use!
    Point _start; ///< start Point of LineSite
    Point _end; ///< end Point of LineSite
};

/// circular arc Site
class ArcSite : public Site {
public:
    /// create arc-site
    ArcSite( const Point& startpt, const Point& endpt, const Point& centr, bool dir) : 
        _start(startpt), _end(endpt), _center(centr), _dir(dir) {
        _radius = (_center - _start).norm();
        eq.q = true;
        eq.a = -2*_center.x;
        eq.b = -2*_center.y;
        _k = 1;
        eq.k = -2*_k*_radius; 
        eq.c = _center.x*_center.x + _center.y*_center.y - _radius*_radius;
    }
    ~ArcSite() {}
    virtual Ofs* offset(Point p1,Point p2) {return new ArcOfs(p1,p2,_center,-1); } //FIXME: radius
    
    virtual bool in_region(const Point& p) const {
        /*
        if (_dir) {
            return p.is_right(_center,_start) && !p.is_right(_center,_end);
        } else {
            return !p.is_right(_center,_start) && p.is_right(_center,_end);
        }
        */
        if (p==_center)
            return true;
            
        double t = in_region_t(p);
        return ( (t>=0) && (t<=1) );
        
    }
    
    /// \todo fix arc-site in_region_t test!!
    virtual double in_region_t(const Point& pt) const {
        // projection onto circle
        /*
        Point cen_start = _start - _center;
        Point cen_end   = _end - _center;
        Point cen_pt = pt - _center;
        Point proj = _center + ( _radius/cen_pt.norm())*cen_pt;
        
        double diangle_min,diangle_max;
        if (!_dir) {
            diangle_min = numeric::diangle( cen_start.x, cen_start.y );
            diangle_max = numeric::diangle( cen_end.x, cen_end.y );
        } else {
            diangle_max = numeric::diangle( cen_start.x, cen_start.y );
            diangle_min = numeric::diangle( cen_end.x, cen_end.y );
        }
        //double fuzz(0.01);
        double diangle_pt = numeric::diangle(cen_pt.x, cen_pt.y);
        */
        double t = in_region_t_raw(pt); //(diangle_pt - diangle_min) / (diangle_max-diangle_min);
        double eps = 1e-7;
        if (fabs(t) < eps)  // rounding... UGLY
            t = 0.0;
        else if ( fabs(t-1.0) < eps )
            t = 1.0;
        return t;
        
        /// find t-value along arc
        //return 0.5; // FIXME
    }
    virtual double in_region_t_raw(const Point& pt) const {
        // projection onto circle
        Point cen_start = _start - _center;
        Point cen_end   = _end - _center;
        Point cen_pt = pt - _center;
        Point proj = _center + ( _radius/cen_pt.norm())*cen_pt;
        
        double diangle_min,diangle_max;
        if (!_dir) {
            diangle_min = numeric::diangle( cen_start.x, cen_start.y );
            diangle_max = numeric::diangle( cen_end.x, cen_end.y );
        } else {
            diangle_max = numeric::diangle( cen_start.x, cen_start.y );
            diangle_min = numeric::diangle( cen_end.x, cen_end.y );
        }
        //double fuzz(0.01);
        double diangle_pt = numeric::diangle(cen_pt.x, cen_pt.y);
        
        double t = (diangle_pt - diangle_min) / (diangle_max-diangle_min);
        return t;
    }
    Point apex_point(const Point& p) {
        if (in_region(p))
            return projection_point(p);
        else
            return closer_endpoint(p);
    }
    
    virtual double x() const {return _center.x;}
    virtual double y() const {return _center.y;}
    virtual double r() const {return _radius;}
    virtual double k() const {return _k;} // ?

    
    virtual std::string str() const {return "ArcSite";}
    virtual std::string str2() const {
        std::string out = "ArcSite: ";
        out.append( _start.str() );
        out.append( " - " );
        out.append( _end.str() );
        out.append( " c=" );
        out.append( _center.str() );
        out.append( " cw=" );
        out.append( (_dir ? "1" : "0" ) );

        return out;
    }
    HEEdge e; ///< edge_descriptor to ::ARCSITE pseudo-edge
    /// return start Point of ArcSite
    const Point start() const {return _start;}
    /// return end Point of ArcSite
    const Point end() const {return _end;}
    /// return center Point of ArcSite
    Point center() {return _center;}
    /// return radius of ArcSite
    double radius() {return _radius;}
    /// return true for CW ArcSite and false for CCW
    bool cw() {return _dir;}
    inline virtual bool isArc() const  { return true;}

private:
    /// projection of given Point onto the ArcSite
    Point projection_point(const Point& p) const {
        if ( p == _center ) {
            return _start;
        } else {
            Point dir = (p-_center);
            dir.normalize();
            return _center + _radius*dir; // this point should lie on the arc
        }
    }
    /// return the end Point (either _start or _end) that is closest to the given Point 
    Point closer_endpoint(const Point& p) const {
        double d_start = (_start-p).norm();
        double d_end = (_end-p).norm();
        if (d_start < d_end)
            return _start;
        else
            return _end;
    }
    ArcSite() {} // don't use!
    Point _start;  ///< start Point of arc
    Point _end;    ///< end Point of arc
    Point _center; ///< center Point of arc
    bool _dir;     ///< CW or CCW direction flag
    double _radius;///< radius of arc
    double _k; ///< offset-direction. +1 for enlarging, -1 for shrinking circle
};


} // end namespace
