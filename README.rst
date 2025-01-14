.. image:: https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg
    :target: https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html

OpenVoronoi
===========

Updates: 2018-07 (change to LGLP), 2015-02-12.

The OpenVoronoi project aims to produce an algorithm for calculating
the 2D voronoi-diagram for point, line-segment, and circular-arc sites.
Currently point-sites and line-segment sites work. Arc-sites are being worked
on. The incremental topology-oriented (Sugihara-Iri and/or Held) 
algorithm is used (see References).

The core algorithm is in C++. 

OpenVoronoi is written by Anders Wallin (anders.e.e.wallin "at" gmail.com)
and initially released under GPLv3. In July 2018, license was changed to LGPL2.1 (see COPYING) with permission and cooperation of all contributors (Issue #35).

In February 2015 Rogach published a Java port called jopenvoronoi (https://github.com/Rogach/jopenvoronoi)

Voronoi diagrams are used for many purposes in computational geometry,
but the motivation for OpenVoronoi has mainly been 2D offset-generation
(see offset.hpp) for cnc mill toolpath calcuations. An experimental 
approximate medial-axis filter (medial_axis.hpp) has been added.

The OpenVoronoi project is at 
https://github.com/aewallin/openvoronoi

Launchpad PPA (not updated regularly)
https://launchpad.net/~anders-e-e-wallin/+archive/cam

The mailing-list for OpenVoronoi is at
https://groups.google.com/forum/?hl=en#!forum/opencamlib

There is a gallery of voronoi diagrams produced with OpenVoronoi at
https://picasaweb.google.com/106188605401091280402/OpenVoronoiExamples

Required Dependencies:

- cmake
- Boost graph library   

Optional Dependencies:

- git            (required only for the version-string)
- doxygen        (for building documentation)

Build/Install instructions
==========================

From source::

$ git clone https://github.com/LarsLive/openvoronoi.git
$ cd openvoronoi
$ ./build.sh
$ make

Documentation
=============

Doxygen documentation can be built with "make doc"
A white-paper on the algorithm and solvers in LyX format is located in /doc. 
It has its own CMakeLists.txt file which builds a PDF file.

Tests
=====

c++ tests are found in src/test/. These are run with CTest.
In the build-directory either "make test" or "ctest" will run all tests. 
You can run only tests that have e.g. "ttt" in the test-name with
"ctest -R ttt"
Currently the tests do not produce any output (png or svg output could be an option?)

Organization
============

- doc/        has documentation in lyx format, with figures in asymptote format. 
            Build a PDF with the CMakeLists.txt in this directory.
- src/        has the source for the main algorithm
- src/solvers has vd-vertex solver code
- src/py      has python wrapping code
- src/common  has common classes not specific to voronoi diagrams
- src/utility input and output from OpenVoronoi to/from various formats

Contributing
============

See the TODO file. Fork the github repo, create a feature branch, commit your 
changes, test. Make a short description of your changes and create a pull request.
Follow the coding-style of the existing code. One fix/feature per pull request.
Contributed code must comply with the LGPL. Provide short doxygen-formatted 
documentation in the code.

Other voronoi-diagram codes
===========================

- CGAL, http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Voronoi_diagram_2/Chapter_main.html
- LEDA, http://www.algorithmic-solutions.info/leda_guide/geo_algs/voronoi.html
- Boost.Polygon.Voronoi, http://www.boost.org/doc/libs/1_52_0/libs/polygon/doc/voronoi_main.htm
- VRONI/Martin Held. This code is commercial and not available, as far as we know. http://www.cosy.sbg.ac.at/~held/projects/vroni/vroni.html
- Voro++, BSD-licensed code for 3D voronoi cell computation. May not be useful for 2D toolpath generation? http://math.lbl.gov/voro++/
- Triangle http://www.cs.cmu.edu/~quake/triangle.html Really a mesh-generator for e.g. finite-element analysis. A constrained Delaunay triangulation could be used to generate a Voronoi diagram for point and line inputs.



Boost.Polygon.Voronoi was a Google Summer of Code project in 2010.
Integer input coordinates. Exact geometric predicates through geometric filtering. 
Uses Fortune's sweepline algorithm.
Boostcon video:
"Sweep-Line Algorithm for Voronoi Diagrams of Points, Line Segments and Medial Axis of Polygons in the Plane"
http://blip.tv/boostcon/sweep-line-algorithm-for-voronoi-diagrams-of-points-line-segments-and-medial-axis-of-polygons-in-the-plane-5368229

Patel (see References) seems to have independently implemented the VRONI/Held algorithm, 
bu we don't know where this code is or under what license it is.

References
==========

Sugihara and Iri, (1992) "construction of the voronoi diagram for one 
million generators in single-precision arithmetic" 
http://dx.doi.org/10.1109/5.163412

Imai (1996) "A Topology-Oriented Algorithm for the Voronoi Diagram 
of Polygons" http://www.cccg.ca/proceedings/1996/cccg1996_0019.pdf

Sugihara, Iri, Inagaki, Imai, (2000) "topology oriented implementation 
- an approach to robust geometric algorithms" 
http://dx.doi.org/10.1007/s004530010002

Held, (1991) "On the Computational Geometry of Pocket Machining"
Lecture notes in computer science, vol 500
http://www.amazon.com/Computational-Geometry-Machining-Lecture-Computer/dp/3540541039/

Held, (2001) "VRONI: an engineering approach to the reliable and 
efficient computation of Voronoi diagrams of points and line 
segments" http://dx.doi.org/10.1016/S0925-7721(01)00003-7

Martin Held, Stefan Huber, (2009) "Topology-oriented incremental 
computation of Voronoi diagrams of circular arcs and straight-line 
segments", Computer-Aided Design, Volume 41, Issue 5, May 2009, Pages 327-338
http://dx.doi.org/10.1016/j.cad.2008.08.004

Nirav B. Patel (2005), "Voronoi diagrams, robust and efficient implementation", Binghamton
University, State University of New York, 2005, MSc thesis. (this thesis is not
accompanied by code, or much implementation detail)

Kim D-S, (1998), "Polygon offsetting using a Voronoi diagram and two stacks"
Computer Aided Design, Vol. 30, No. 14, pp 1069-1076
http://dx.doi.org/10.1016/S0010-4485(98)00063-3

Chen, Fu
"An optimal approach to multiple tool selection and their numerical control path generation for 
aggressive rough machining of pockets with free-form boundaries"
Computer Aided Design 43 (2011) 651-663
http://dx.doi.org/10.1016/j.cad.2011.01.020


todo: Burnikel-papers? 

References, HSM or Trochoidal paths:

Martin Held, Christian Spielberger (2009). "A smooth spiral tool path for 
high speed machining of 2D pockets", Computer-Aided Design, Volume 41, 
Issue 7, July 2009, Pages 539-550
http://dx.doi.org/10.1016/j.cad.2009.04.002
See also www.cosy.sbg.ac.at/~cspiel/projects/hsm/isvd08.pdf 
and www.cosy.sbg.ac.at/~held/teaching/seminar/seminar_2010-11/hsm.pdf

Gershon Elber, Elaine Cohen, Sam Drake, "MATHSM: medial axis trasform toward high speed machining
of pockets", Computer Aided Design 37 (2004) 241-250
http://dx.doi.org/10.1016/j.cad.2004.05.008

Rauch et al. (2009) "Improving trochoidal tool paths generation and implementation using process constraints modelling"
http://dx.doi.org/10.1016/j.ijmachtools.2008.12.006
This paper has formulas for maximum depth of cut for circular and trochoidal clearing paths

Ibaraki (2010) "On the removal of critical cutting regions by trochoidal grooving"
http://dx.doi.org/10.1016/j.precisioneng.2010.01.007
