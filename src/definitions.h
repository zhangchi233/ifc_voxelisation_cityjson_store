#ifndef DEFS_H
#define DEFS_H

// CGAL kernel
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/Segment_3.h>
#include <CGAL/intersections.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/Triangulation_vertex_base_with_id_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/poisson_surface_reconstruction.h>

#include "json.hpp" //-- it is in the /include/ folder
using json = nlohmann::json;
struct Face {
  std::vector<int> indices; // indices in vector of points
};

struct Shell {
  std::vector<Face> faces;
  std::string use_material;

};

struct Object {
  std::string id;
  std::vector<Shell> shells;
};
//-- for mark_domain()
struct FaceInfo2
{
  FaceInfo2() {}
  int nesting_level;
  bool in_domain() {
    return nesting_level % 2 == 1;
  }
};  

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Vector_3                 Vector3;

typedef K::Point_2                  Point2;
typedef K::Point_3                  Point3;
typedef CGAL::Polygon_2<K>          Polygon2;
typedef K::Plane_3                  Plane;

typedef K::Segment_3                Segment3;
typedef K::Triangle_3               Triangle3;
typedef CGAL::Triangulation_vertex_base_with_id_2 <K>             Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K>   Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb>       Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_intersections_tag                             Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CT;
typedef std::pair<Point3, Vector3> Pwn;
typedef CGAL::Polyhedron_3<K> Polyhedron;
using Facet_handle = Polyhedron::Facet_handle;
using Halfedge_handle = Polyhedron::Halfedge_handle;
using Facet_indices = std::vector<std::size_t>;
typedef std::pair<Point3, Vector3> Pwn;
typedef CGAL::Polyhedron_3<K> Polyhedron;
Point3 operator+(const Point3& p1, const Point3& p2);
Point3 operator/(const Point3& p1, const double& scale);
//Point3 operator-(const Point3& p1, const Point3& p2);






#endif
