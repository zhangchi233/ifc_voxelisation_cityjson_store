#ifndef __parse_obj__
#define __parse_obj__

#include "definitions.h"
#include "geomtools.h"
#include <iostream>
#include <fstream>
#include <string>
#include <CGAL/Surface_mesh.h>
#include <CGAL/optimal_bounding_box.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
namespace PMP = CGAL::Polygon_mesh_processing;
typedef CGAL::Surface_mesh<Point3> Surface_mesh;

std::pair<std::vector<Face>,std::map<std::string, Object>> parse_obj(const std::string& filename,std::vector<Point3>& vertices);
std::array<Point3, 8> generate_oobb_building(const std::vector<Face>& faces,const std::vector<Point3>& vertices);
void stored_faces_obj(std::string filename,const std::vector<Face>& faces,const std::vector<Point3>& vertices);

#endif 
