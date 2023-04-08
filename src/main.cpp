  /*
  geo1004.2023
  hw02 help code
  Hugo Ledoux <h.ledoux@tudelft.nl>
  2023-03-01
*/
//#include "voxel_processing.h"
#include <CGAL/Triangulation_3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <typeinfo>
#include "definitions.h"
#include "geomtools.h"
#include "parse_obj.h"
#include <CGAL/Surface_mesh.h>
#include <CGAL/optimal_bounding_box.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
//#include <CGAL/point_generators_3.h>
#include <CGAL/Simple_cartesian.h>



//-- https://github.com/nlohmann/json
//-- used to read and write (City)JSON

//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_3<K> Tetrahedron;
namespace PMP = CGAL::Polygon_mesh_processing;
typedef CGAL::Surface_mesh<Point3> Surface_mesh;
typedef CGAL::Simple_cartesian<double>                           cartesian;
std::vector<Point3> get_coordinates(const json& j, bool translate = true);
void                save2obj(std::string filename, const json& j);
void                enrich_and_save(std::string filename, json& j);


using namespace std;

std::vector<int> label_voxels(VoxelGrid& voxelgrid,const int& label_initial = 0,const int& wall_face_label=1){
    // we start from the first voxel, which must be the original point of voxel grid and is on the exterior of the voxelised building
    std::vector<int> initial_voxel_index = {0,0,0};
    std::vector<int> exterior_voxels;
    std::vector<int> labells;
    // define a queue to store the voxels to be processed
    std::queue<std::vector<int>> queue;
    // push the initial voxel into the queue
    queue.push(initial_voxel_index);
    int total_voxels = voxelgrid.voxels.size();
    
    int current_label = label_initial;
    if (current_label == wall_face_label) {labells.push_back(current_label);current_label+=1;}

    
    while(queue.empty() == false){
        // pop the first voxel in the queue
        //std::cout << "current label 3 is " << current_label << std::endl;
        std::vector<int> current_voxel_coordinate = queue.front();
        queue.pop();
        int x = current_voxel_coordinate[0];
        int y = current_voxel_coordinate[1];
        int z = current_voxel_coordinate[2];
        Voxel current_voxel = voxelgrid(x,y,z);
        if(current_voxel.label == unlabelled){

            // if the current voxel is unlabelled, we label it with the label_initial
            current_voxel.label = current_label;
            voxelgrid(x,y,z) = current_voxel;
            // we check the neighbours of the current voxel
            std::vector<std::vector<int>> neighbours = voxelgrid.get_neighbours(x,y,z);
            for(auto& neighbour: neighbours){
                int x_neighbour = neighbour[0];
                int y_neighbour = neighbour[1];
                int z_neighbour = neighbour[2];
                Voxel neighbour_voxel = voxelgrid(x_neighbour,y_neighbour,z_neighbour);
                if(neighbour_voxel.label == unlabelled){
                    // if the neighbour voxel is unlabelled, we push it into the queue
                    queue.push(neighbour);
                }
                
        }

        // since the voxel grid size is bigger than building, so we can make sure the 0,0,0 voxel is on the exterior of the building
        // if there are still unlabelled voxels but the queue is empty, it means that we have labelled an empty continueous space
        }
    int count_voxels = 0;
    if(queue.empty() and (count_voxels < total_voxels)){
            //std::cout<<"the"<<current_label<<"label"<<std::endl;
            
            labells.push_back(current_label);
            current_label += 1;
            if (current_label == wall_face_label) {labells.push_back(current_label);current_label+=1;}
            for(int x = 0; x < voxelgrid.max_x; x++){
                for(int y = 0; y < voxelgrid.max_y; y++){
                    for(int z = 0; z < voxelgrid.max_z; z++){
                        Voxel voxel = voxelgrid(x,y,z);
                        count_voxels+=1;
                        if(voxel.label == unlabelled){
                            std::vector<int> voxel_index = {x,y,z};
                            
                            queue.push(voxel_index);
                            //std::cout<<current_label<<" "<<x<<" "<<y<<" "<<z<<std::endl;
                            //std::cout<<queue.size()<<" after squeue size"<<std::endl;
                            
                            //std::cout<<"we have found an unlabelled voxel"<<std::endl;
                            
                            // jump out of the three loops
                            x = voxelgrid.max_x + 1;
                            y = voxelgrid.max_y + 1;
                            z = voxelgrid.max_z + 1;
                           
                            
                            };
                            };
                            };
                            };
                            //std::cout<<"current label is "<<current_label<<"   "<<queue.size()<<"queue size"<<std::endl; 
                            
                            };
    

    //if(queue.empty()) {std::cout<<"the last label is "<<current_label<<std::endl;};
    }; 
    
return labells;
}; 

std::vector<std::vector<int>> extract_exterior_surface_voxel_index(const int& exterior_labell, VoxelGrid voxelgrid){
    std::vector<std::vector<int>> exterior_voxels;
    for(int x = 0; x < voxelgrid.max_x; x++){
        for(int y = 0; y < voxelgrid.max_y; y++){
            for(int z = 0; z <voxelgrid.max_z; z++){
                std::vector<std::vector<int>> neighbours = voxelgrid.get_neighbours(x,y,z);
                
                for(auto& neighbour: neighbours){
                    int x_neighbour = neighbour[0];
                    int y_neighbour = neighbour[1];
                    int z_neighbour = neighbour[2];
                    Voxel neighbour_voxel = voxelgrid(x_neighbour,y_neighbour,z_neighbour);
                    if(neighbour_voxel.label == exterior_labell and voxelgrid(x,y,z).label != exterior_labell){
                        std::vector<int> voxel_index = {x,y,z};
                        exterior_voxels.push_back(voxel_index);
                        break;
                    };
                };
            };
        }
    }
    return exterior_voxels;
}

void output_exterior_surface_to_obj(const std::string &output_filename, const std::vector<std::vector<int>> exterior_voxels,
                                const VoxelGrid& voxelgrid){
    std::ofstream out;
    out.open(output_filename);
    int vertex_index = 1;
    for(auto& voxel_index: exterior_voxels){
        int x = voxel_index[0];
        int y = voxel_index[1];
        int z = voxel_index[2];
        Voxel voxel = voxelgrid(x,y,z);
        Point3 p0 = voxel.corner;
        Point3 p1 = p0 + voxelgrid.vx;
        Point3 p2 = p0 + voxelgrid.vx+voxelgrid.vy;
        Point3 p3 = p0+voxelgrid.vy;
        Point3 p4 = p0+voxelgrid.vy+voxelgrid.vz;
        Point3 p5 = p0+voxelgrid.vz;
        Point3 p6 = p0+voxelgrid.vx+voxelgrid.vz;
        Point3 p7 = p0+voxelgrid.vx+voxelgrid.vz+voxelgrid.vy;
        out << "v " <<  p0.x()<< " " << p0.y() << " " << p0.z()<< std::endl; //p0
        out << "v " <<  p1.x()<< " " << p1.y() << " " << p1.z()<< std::endl; //p1
        out << "v " <<  p2.x()<< " " << p2.y() << " " << p2.z()<< std::endl; //p2
        out << "v " <<  p3.x()<< " " << p3.y() << " " << p3.z()<< std::endl; //p3
        out << "v " <<  p4.x()<< " " << p4.y() << " " << p4.z()<< std::endl; //p4
        out << "v " <<  p5.x()<< " " << p5.y() << " " << p5.z()<< std::endl; //p5
        out << "v " <<  p6.x()<< " " << p6.y() << " " << p6.z()<< std::endl; //p6
        out << "v " <<  p7.x()<< " " << p7.y() << " " << p7.z()<< std::endl; //p7
        out << "f " << vertex_index << " " << vertex_index + 1 << " " << vertex_index + 2 << " " << vertex_index + 3 << std::endl;
        out << "f " << vertex_index << " " << vertex_index + 1 << " " << vertex_index + 6 << " " << vertex_index + 5 << std::endl;
        out << "f " << vertex_index+1 << " " << vertex_index + 2 << " " << vertex_index + 7 << " " << vertex_index + 6 << std::endl;
        out << "f " << vertex_index +3<< " " << vertex_index + 2 << " " << vertex_index + 7 << " " << vertex_index + 4 << std::endl;
        out << "f " << vertex_index << " " << vertex_index + 3 << " " << vertex_index + 4 << " " << vertex_index + 5 << std::endl;
        out << "f " << vertex_index+4 << " " << vertex_index + 7 << " " << vertex_index + 6 << " " << vertex_index + 5 << std::endl;
        vertex_index+=8;}
        out.close();
        }

                                     
int main(int argc, const char * argv[]) {
    //-- will read the file passed as argument or 2b.city.json if nothing is passed
    const char* filename = (argc > 1) ? argv[1] : "../data/input.obj";
    const int  output_label = (argc > 2) ? int(*argv[2]) : 1;
    const double  unit = (argc > 3) ? double(*argv[3]) : 0.25;
    bool output_intermediate_components = (argc > 4) ? bool(*argv[4]) : true;

    
    std::vector<Point3> vertices; // store vertices from converted obj files
    auto obj_parse_result = parse_obj(filename,vertices); // parse the obj fies
    std::vector<Face> faces = obj_parse_result.first; // the faces we read in the whole building
    std::map<std::string,Object> objects=  obj_parse_result.second; // the object we read from ifc-converted obj file
    //stored_faces_obj("face_obj.obj",faces,vertices);// out put faces
    // oobb of building
    std::array<Point3, 8> oobb = generate_oobb_building(faces,vertices);
    // initialize voxel grid
    VoxelGrid voxels = VoxelGrid(oobb,unit);
    // label to label the wall, floor roof surfaces
    int wall_face_label = 1;
    // initalize the whole voxels
    voxels.push_voxel();
     int shells_count = 0; // debug
     int total_faces = 0; // debug
    // first label the walls, floor or other existed surfaces we can read from obj files
    for(auto& object: objects){
        std::string object_name = object.first;
        std::vector<Shell> object_shells = object.second.shells;
        for(auto& shell: object_shells){
            std::vector<Face> shell_faces = shell.faces;
            std::string material_name = shell.use_material;
            shells_count++;
            total_faces += shell_faces.size();
            std::cout<<shell.use_material<<" "<<shells_count<<" "<<shell_faces.size()<<std::endl; // print the material name, which u can used as attribute in cityjson output
            
            voxels.push_voxel(shell_faces,vertices,wall_face_label);// push the faces into the voxel grid
        }}
    std::cout<<"total faces: "<<total_faces<<" true faces"<<faces.size()<<std::endl;
    std::vector<int> labells;
    int exterior_label = 0;
    labells = label_voxels(voxels,exterior_label,wall_face_label);
    

   
    std::vector<std::vector<int>> exterior_surface_index;
    exterior_surface_index = extract_exterior_surface_voxel_index(0,voxels);
    std::cout<<"exterior surface index size: "<<exterior_surface_index.size()<<std::endl;
  
    output_exterior_surface_to_obj("exterior_surface.obj",exterior_surface_index,voxels);
    if (output_intermediate_components){
    voxels.out_put_all_voxel_to_obj("output_voxels.obj",output_label);
    voxels.out_put_voxels_seperately("label_voxels",labells);
    }

    // tell the cityjson to output the room, wall, with exception of exterior
    
    json j = voxels.initialized_voxels_grid_tojson("building_test",labells, exterior_label);
    std::ofstream o("building_test.json");
    o << std::setw(4) << j << std::endl;
    o.close();

    
    //for (const auto& point : oobb) {
    //std::cout << point << " ";
    //}
    //std::cout << std::endl;












    /*std::cout << "Processing: " << filename << std::endl;
    std::ifstream input(filename);
    cout<<"json"<<endl;
    json j;
    input >> j; //-- store the content of the file in a nlohmann::json object

    input.close();

    //-- convert each City Object in the file to OBJ and save to a file
    save2obj("out.obj", j);

    //-- enrich with some attributes and save to a new CityJSON
    enrich_and_save("out.city.json", j);*/

    return 0;
}



