
// OpenVoronoi polygon example

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

#include "voronoidiagram.hpp"
#include "version.hpp"
#include "common/point.hpp"
#include "utility/vd2svg.hpp"

#include <boost/foreach.hpp>


int main(int /*argc*/,char** /*argv[]*/) {

    ovd::VoronoiDiagram* vd = new ovd::VoronoiDiagram(1);
    std::string filename = "output.svg";
    std::cout << "OpenVoronoi version: " << ovd::version() << "\n";

    std::vector<int> id;
    std::vector<ovd::Point> vertices;
    /*       -----------------
     *      /      arc        \
     *     1                   0
     *     |                   |
     *     l3                  l1
     *     |                   |
     *     3-------l2----------2
     * */
    vertices.push_back( ovd::Point(0.1,0.1) );  // 0
    vertices.push_back( ovd::Point(-0.1,0.1) ); // 1
    vertices.push_back( ovd::Point(0.1,-0.1) ); // 2
    vertices.push_back( ovd::Point(-0.1,-0.1) ); // 3

    //vertices.push_back( ovd::Point(-0.03,-0.03) ); // 4?

    // point-sites must be inserted first.
    // insert_point_site() returns an int-handle that is used when inserting line-segments
  //  BOOST_FOREACH(ovd::Point p, vertices ) {
  //      id.push_back( vd->insert_point_site(p) );
  //  }
  //  if (!vd->check()) return -1;

    /*
    // now we insert line-segments
    vd->insert_line_site(id[0],id[2]); // l1
    if (!vd->check()) return -1;
    vd->insert_line_site(id[3],id[2]); // l2
    if (!vd->check()) return -1;
    vd->insert_line_site(id[3],id[1]);
    if (!vd->check()) return -1;

    // then the arc
    ovd::Point center(0,0);
    vd->insert_arc_site( id[1], id[0] , center, true ); // ccw arc
*/


    //;circle ccw:1 radius:0.3103 s:-90 e:-9.46232 w:80.5377
    //g0 x0.0808 y0.4821 ;Point
    //g0x0.0808 y0.1718
    //g3 x0.3869 y0.4311 i0 j0.3103

    /*
    int id1 = vd->insert_point_site(ovd::Point(0.0808,0.1718));
    int id2 = vd->insert_point_site(ovd::Point(0.3869,0.4311));
    // then the arc
    ovd::Point center(0.0808,0.4821);
    vd->debug_on();
    vd->insert_arc_site( id1, id2 , center, false ); // ccw arc
*/
    /*
    ;circle ccw:1 radius:0.0743773 s:27.3791 e:28.8791 w:1.5
    g0 x0.0595 y0.1627 ;Point
    g0x0.1255 y0.1969
    g3 x0.1246 y0.1986 i-0.066 j-0.0342
	*/
    //int id0 = vd->insert_point_site(ovd::Point(0, 0));
    double s=1;

    ovd::Point p1(0.1255*s, 0.1969*s);
    ovd::Point p2(0.1246*s, 0.1986*s);
    ovd::Point p3(0, 0);
    int id1 = vd->insert_point_site(p1);
    int id2 = vd->insert_point_site(p2);
    int id3 = vd->insert_point_site(p3);


    // then the arc
    ovd::Point center(0.0595*s, 0.1627*s);
    vd->debug_on();
    //vd->insert_line_site( id0, id1);
    //vd->insert_line_site( id1, id2);
    vd->insert_line_site( id1, id2);
    vd->insert_line_site( id2, id3);
    //vd->insert_arc_site( id1, id2 , center, true ); // cw arc
    return 0;
    std::cout << vd->print();
    ovd::HEGraph& g = vd->get_graph_reference();
    BOOST_FOREACH( ovd::HEEdge e, g.edges() ) {

    	ovd::HEVertex src = g.source(e);
    	ovd::HEVertex trg = g.target(e);
    	ovd::Point src_p  = g[src].position;
    	ovd::Point trg_p  = g[trg].position;

    	if(p1==src_p && p2==trg_p) {
    		std::cout << src_p<<trg_p;
    		std::cout<<"huhu"<<std::endl;
    		//g[e].type = ovd::ARCSITE;
    	}

    }

    std::cout << " Correctness-check: " << vd->check() << "\n";
    std::cout << vd->print();
    vd2svg(filename, vd);

    delete vd;
    return 0;
}
