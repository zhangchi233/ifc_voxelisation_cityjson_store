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
#include "marching_cubes.h"
#include <CGAL/Surface_mesh.h>
#include <CGAL/optimal_bounding_box.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
//#include <CGAL/point_generators_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/poisson_surface_reconstruction.h>



//-- https://github.com/nlohmann/json
//-- used to read and write (City)JSON

//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Triangulation_3<K> Tetrahedron;
namespace PMP = CGAL::Polygon_mesh_processing;
typedef CGAL::Surface_mesh<Point3> Surface_mesh;
typedef CGAL::Simple_cartesian<double> cartesian;


using namespace std;
void output_voxels_seperately(const std::string& cityobject_id,
const std::vector<int>& labells,const std::map<int,std::vector<std::vector<Point3>>> &mesh_points,const int& exterior_label);
std::vector<int> label_voxels(VoxelGrid& voxelgrid,const int& label_initial = 0,
const int& wall_face_label=1,const bool& check_width = true,const int& minimum_width = 2){
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
    if (current_label == wall_face_label) {
        labells.push_back(current_label);
        current_label+=1;
        }
  
    std::vector<int> current_label_width = {999999,999999,999999,0,0,0};
    std::map<int,std::vector<int>> label_width;
    
    while(queue.empty() == false){
        // pop the first voxel in the queue
        //std::cout << "current label 3 is " << current_label << std::endl;
        std::vector<int> current_voxel_coordinate = queue.front();
        queue.pop();
        int x = current_voxel_coordinate[0];
        int y = current_voxel_coordinate[1];
        int z = current_voxel_coordinate[2];
        // check x,y,z is true
        
        Voxel current_voxel = voxelgrid(x,y,z);
        if(current_voxel.label == unlabelled){

            // if the current voxel is unlabelled, we label it with the label_initial
            current_voxel.label = current_label;
            voxelgrid(x,y,z) = current_voxel;
            // modify the width of the current label
            if(check_width){

            if (x < current_label_width[0]) {current_label_width[0] = x;}
            if (y < current_label_width[1]) {current_label_width[1] = y;}
            if (z < current_label_width[2]) {current_label_width[2] = z;}
            if (x > current_label_width[3]) {current_label_width[3] = x;}
            if (y > current_label_width[4]) {current_label_width[4] = y;}
            if (z > current_label_width[5]) {current_label_width[5] = z;}
            label_width[current_label] = current_label_width;

        }
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
            // remeber, even this label is not recorded by labells, the voxel is still labelled
            if(check_width){
                int width_x = current_label_width[3] - current_label_width[0];
                int width_y = current_label_width[4] - current_label_width[1];
                int width_z = current_label_width[5] - current_label_width[2];
                if(width_x > minimum_width and width_y > minimum_width and width_z > minimum_width){
                    labells.push_back(current_label);
                    current_label += 1;
                    if (current_label == wall_face_label) {labells.push_back(current_label);current_label+=1;}
                    current_label_width ={99999,99999,99999,0,0,0};
                }
                else{
                    //std::cout<<"the"<<current_label<<"label is too small"<<std::endl;
                    current_label_width ={99999,99999,99999,0,0,0};
                    current_label+=1;
                    if (current_label == wall_face_label) {current_label+=1;}

                }
            }
            else{
                labells.push_back(current_label);
                current_label += 1;
                if (current_label == wall_face_label) {labells.push_back(current_label);current_label+=1;}
                }
            
            for(int x = 0; x < voxelgrid.max_x; x++){
                for(int y = 0; y < voxelgrid.max_y; y++){
                    for(int z = 0; z < voxelgrid.max_z; z++){
                        Voxel voxel = voxelgrid(x,y,z);
                        count_voxels+=1;
                        if(voxel.label == unlabelled){
                            std::vector<int> voxel_index = {x,y,z};
                            
                            queue.push(voxel_index);
                            
                            
                            // jump out of the three loops
                            x = voxelgrid.max_x + 1;
                            y = voxelgrid.max_y + 1;
                            z = voxelgrid.max_z + 1;
                            };
                            };
                            };
                            };    
        };
    }; 
    
return labells;
}; 

std::map<int,std::vector<std::vector<Point3>>> extract_envelope_voxel(VoxelGrid& voxels,
const int& wall_index,const int& exterior_index,const std::vector<int>& labells)
{
    int voxel_length = int(voxels.voxels.size());

    std::map<int,std::vector<vector<Point3>>> mesh_faces; // we use marching cubes, since it is exterior surface, we only need to extract the exterior surfaces of exterior voxels
    // unlabelled useless  label and interior wall
    for(int x =0; x<voxels.max_x-1; x++)
    {
        for(int y =0; y<voxels.max_y-1; y++)
        {
            for(int z =0; z<voxels.max_z-1; z++)
            {
            int label = voxels(x,y,z).label;
            if(label != exterior_index and std::find(labells.begin(),labells.end(),label) == labells.end()){

            voxels(x,y,z).label = unlabelled;
            // remove the unlabelled voxels from the mesh_points
            }
            else if(label == wall_index){
            std::vector<std::vector<int>> neighbours = voxels.get_neighbours(x,y,z,18); // return the neighbours
            bool not_exterior = true;
            for(auto& neighbor:neighbours){
                int x_neighbour = neighbor[0];
                int y_neighbour = neighbor[1];
                int z_neighbour = neighbor[2];
                int neighbour_label = voxels(x_neighbour,y_neighbour,z_neighbour).label;
                if(neighbour_label ==exterior_index) 
                {
                    not_exterior = false;
                    break;}
            }
            if(not_exterior){
                voxels(x,y,z).label = unlabelled;
            }


        }
        else if(std::find(labells.begin(),labells.end(),label) == labells.end()) 
        {
            voxels(x,y,z).label = unlabelled;
        }
        }
        }
        }
     voxels.out_put_voxels_seperately("voxels_labelled",labells);
     ofstream myfile;

     int face_count = 1;
 
     for(int x = 0; x < voxels.max_x; x++){
        for(int y = 0; y < voxels.max_y; y++){
            for(int z = 0; z < voxels.max_z; z++)
            {
                
                cell mesh_cell;
                mesh_cell.points.push_back(voxels(x,y,z).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x+1,y,z).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x+1,y,z+1).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x,y,z+1).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x,y+1,z).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x+1,y+1,z).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x+1,y+1,z+1).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                mesh_cell.points.push_back(voxels(x,y+1,z+1).corner+voxels.vx/2+voxels.vy/2+voxels.vz/2);
                bool homogenious = true;
                int iso_label;
                mesh_cell.values.push_back(voxels(x,y,z).label); //p0
                mesh_cell.values.push_back(voxels(x+1,y,z).label); //p1
                mesh_cell.values.push_back(voxels(x+1,y,z+1).label); //p2
                mesh_cell.values.push_back(voxels(x,y,z+1).label); //p3
                mesh_cell.values.push_back(voxels(x,y+1,z).label); //p4
                mesh_cell.values.push_back(voxels(x+1,y+1,z).label); //p5
                mesh_cell.values.push_back(voxels(x+1,y+1,z+1).label); //p6
                mesh_cell.values.push_back(voxels(x,y+1,z+1).label); //p7
                homogenious = std::adjacent_find(mesh_cell.values.begin(), mesh_cell.values.end(), std::not_equal_to<int>()) == mesh_cell.values.end();
                mesh_cell.homogenious = homogenious;
                std::pair<int,std::vector<vector<Point3>>> result;
                
                if(homogenious) continue; // interior
                //else if (std::find(mesh_cell.values.begin(), mesh_cell.values.end(), exterior_index) == mesh_cell.values.end()
                //&& std::find(mesh_cell.values.begin(), mesh_cell.values.end(), wall_index) != mesh_cell.values.end()) continue;
                else {
                    //myfile<<"v "<<mesh_cell.points[0].x()<<" "<<mesh_cell.points[0].y()<<" "<<mesh_cell.points[0].z()<<std::endl;
                    //myfile<<"v "<<mesh_cell.points[1].x()<<" "<<mesh_cell.points[1].y()<<" "<<mesh_cell.points[1].z()<<std::endl;
                    //myfile<<"v "<<mesh_cell.points[2].x()<<" "<<mesh_cell.points[2].y()<<" "<<mesh_cell.points[2].z()<<std::endl;
                    //myfile<<"v "<<mesh_cell.points[3].x()<<" "<<mesh_cell.points[3].y()<<" "<<mesh_cell.points[3].z()<<std::endl;
                    //myfile<<"v "<<mesh_cell.points[4].x()<<" "<<mesh_cell.points[4].y()<<" "<<mesh_cell.points[4].z()<<std::endl;
                    //myfile<<"v "<<mesh_cell.points[5].x()<<" "<<mesh_cell.points[5].y()<<" "<<mesh_cell.points[5].z()<<std::endl;
                    //myfile<<"v "<<mesh_cell.points[7].x()<<" "<<mesh_cell.points[7].y()<<" "<<mesh_cell.points[7].z()<<std::endl;
                    //myfile<<"f "<<face_count<<" "<<face_count+1<<" "<<face_count+2<<" "<<face_count+3<<std::endl;
                    //myfile<<"f "<<face_count+4<<" "<<face_count+5<<" "<<face_count+6<<" "<<face_count+7<<std::endl;
                    //myfile<<"f "<<face_count<<" "<<face_count+1<<" "<<face_count+5<<" "<<face_count+4<<std::endl;
                    //myfile<<"f "<<face_count+1<<" "<<face_count+2<<" "<<face_count+6<<" "<<face_count+5<<std::endl;
                    //myfile<<"f "<<face_count+2<<" "<<face_count+3<<" "<<face_count+7<<" "<<face_count+6<<std::endl;
                    //myfile<<"f "<<face_count<<" "<<face_count+3<<" "<<face_count+7<<" "<<face_count+4<<std::endl;
                    //face_count+=8;
                    
                    
                    extract_isosurface(mesh_cell,mesh_faces);
                    
                    
                    /*for(int other_label:mesh_cell.values){
                        if ((other_label != exterior_index) && (other_label !=unlabelled) && (other_label!=label_result))
                            {   std::cout<<"label "<<other_label<<" label_result "<<other_label<<std::endl;
                            mesh_faces[other_label].insert(mesh_faces[other_label].end(),faces.begin(),faces.end());}

                    }*/
                    //mesh_faces[label_result].insert(mesh_faces[label_result].end(),faces.begin(),faces.end());
                    //if(std::find(mesh_cell.values.begin(), mesh_cell.values.end(), wall_index) != mesh_cell.values.end()
                    //and label_result != wall_index and label_result !=wall_index){
                      //  mesh_faces[wall_index].insert(mesh_faces[wall_index].end(),faces.begin(),faces.end());
                    //}

                    }

            }
        }
     }
     myfile.close();


    

    return mesh_faces;
}

                                     
int main(int argc, const char * argv[]) {
    //-- will read the file passed as argument or 2b.city.json if nothing is passed
    const char* filename = (argc > 1) ? argv[1] : "../data/input.obj";
    const int  output_label  =1;
    const double  unit =  0.25;
    bool output_intermediate_components = true;

    
    std::vector<Point3> vertices; // store vertices from converted obj files
    
    auto obj_parse_result = parse_obj(filename,vertices); // parse the obj fies
    
    std::vector<Face> faces = obj_parse_result.first; // the faces we read in the whole building

    std::map<std::string,Object> objects=  obj_parse_result.second; // the object we read from ifc-converted obj file
    //stored_faces_obj("face_obj.obj",faces,vertices);// out put faces
    // oobb of building
    std::array<Point3, 8> oobb = generate_oobb_building(faces,vertices);

    // initialize voxel grid
    std::cout <<" true faces"<<faces.size()<<std::endl;
    VoxelGrid voxels = VoxelGrid(oobb,unit);

    // label to label the wall, floor roof surfaces
    int wall_face_label = 1;
    // initalize the whole voxels

    voxels.push_voxel();
     int shells_count = 0; // debug
     int total_faces = 0; // debug
     std::cout <<" true faces"<<faces.size()<<std::endl;
    // first label the walls, floor or other existed surfaces we can read from obj files
    /*for(auto& object: objects){
        std::string object_name = object.first;
        std::vector<Shell> object_shells = object.second.shells;
        for(auto& shell: object_shells){
            std::vector<Face> shell_faces = shell.faces;
            std::string material_name = shell.use_material;
            shells_count++;
            total_faces += shell_faces.size();
            //std::cout<<shell.use_material<<" "<<shells_count<<" "<<shell_faces.size()<<std::endl; // print the material name, which u can used as attribute in cityjson output
            
            voxels.push_voxel(shell_faces,vertices,wall_face_label);// push the faces into the voxel grid
        }}*/
    voxels.push_voxel(faces,vertices,wall_face_label);
    std::cout<<"total faces: "<<total_faces<<" true faces"<<faces.size()<<std::endl;
    std::vector<int> labells;
    int exterior_label = 0;
    int unit_width = 1/unit;  
    labells = label_voxels(voxels,exterior_label,wall_face_label,1,unit_width);
    // extract the exterior surface
    std::map<int,std::vector<std::vector<Point3>>> mesh_points = extract_envelope_voxel(voxels,wall_face_label,exterior_label,labells);
    
    for(auto& mesh: mesh_points){

         std::cout<<mesh.first<<" "<<mesh.second.size()<<std::endl;
         }
   
    //output_exterior_surface_to_obj("exterior_surface.obj",exterior_surface_index,voxels);
    if (output_intermediate_components){
    //voxels.out_put_all_voxel_to_obj("output_voxels.obj",output_label);
    //voxels.out_put_voxels_seperately("label_voxels",labells);
    std::cout<<"mesh points size: "<<mesh_points.size()<<std::endl;
    
    output_voxels_seperately("label_voxels",labells,mesh_points,exterior_label);
    }
    return 0;
}
// can be used for marching cube algorithm
// use cgal Possion reconstruction to reconstruct the mesh and output the mesh to cityjson
void output_voxels_seperately(const std::string& cityobject_id,const std::vector<int>& labells,const std::map<int,std::vector<std::vector<Point3>>> &mesh_points,const int& exterior_label){
    json j;
    std::vector<Pwn> points;
    j["type"] = "CityJSON";
    j["version"] = "1.1";
    j["transform"] = json::object();
    j["transform"]["scale"] = json::array();
    j["transform"]["translate"] = json::array();
    j["transform"]["scale"].push_back(1);j["transform"]["scale"].push_back(1);j["transform"]["scale"].push_back(1);
    j["transform"]["translate"].push_back(0);j["transform"]["translate"].push_back(0);j["transform"]["translate"].push_back(0);
    j["CityObjects"] = json::object();
    j["vertices"] = json::array();
    j["CityObjects"][cityobject_id] = json::object();
    j["CityObjects"][cityobject_id]["type"] = "Building";
    j["CityObjects"][cityobject_id]["children"] = json::array();
    j["vertices"] = json::array();
    int indices = 0;
    int count = 0;
    for(auto& label: labells){
       // std::cout<<"label: "<<label<<std::endl;
        int indice_obj = 1;

        if(label == exterior_label) continue;
        std::string obj_id = cityobject_id+"_part_"+std::to_string(label);
        j["CityObjects"][obj_id] = json::object();
        j["CityObjects"][obj_id]["type"] = "BuildingRoom";
        j["CityObjects"][cityobject_id]["children"].push_back(obj_id);
        j["CityObjects"][obj_id]["geometry"] = json::array();
        j["CityObjects"][obj_id]["attributes"] = json::object();
        j["CityObjects"][obj_id]["attributes"]["room number"] = "room number:"+std::to_string(count);
        j["CityObjects"][obj_id]["attributes"]["room label"] = std::to_string(label);
        if (count == 0) j["CityObjects"][obj_id]["attributes"]["is wall"]= "true";
        else j["CityObjects"][obj_id]["attributes"]["is wall"]= "false";
        count++;
        
        j["CityObjects"][obj_id]["parents"] = json::array();
        j["CityObjects"][obj_id]["parents"].push_back(cityobject_id);
        json geometry = json::object(); 
        geometry["type"] = "MultiSurface";
        geometry["lod"] = "1";
        geometry["boundaries"] = json::array();
        std::vector<std::vector<Point3>> faces = mesh_points.at(label);
        //std::cout<<"faces size: "<<faces.size()<<std::endl;
        //std::cout<<"label: "<<label<<" faces size: "<<faces.size()<<std::endl;
        // output to obj of faces

        std::ofstream output(obj_id+".obj");
        output<<"# OBJ file"<<std::endl;
        // output the vertices
        for(auto& face: faces){
            if (face.size()== 0) continue;
            
            std::array<int,3> ring;
            std::array<std::array<int,3>,1> surface;
            
                        
            json solid = json::array(); 
            for(Point3& pt: face){
                json vertices = json::array();
                vertices.push_back(pt.x());
                vertices.push_back(pt.y());
                vertices.push_back(pt.z());
                j["vertices"].push_back(vertices);
                ring[indices%3] = indices;
                indices +=1;

                output<<"v "<<pt.x()<<" "<<pt.y()<<" "<<pt.z()<<" "<<std::endl;
                //std::cout<<"pt: "<<pt.x()<<" "<<pt.y()<<" "<<pt.z()<<std::endl;
            }
            surface[0] = ring;
            geometry["boundaries"].push_back(surface);
            

            output<<"f "<<indice_obj<<" "<<indice_obj+1<<" "<<indice_obj+2<<" "<<std::endl;
            indice_obj += 3;   
        }
        j["CityObjects"][obj_id]["geometry"].push_back(geometry);

        output.close();
        
        
    }
    //output seperately

    if(1){
            for(auto& label:labells){
                if(label == exterior_label) continue;
                std::string obj_id = cityobject_id+"_part_"+std::to_string(label);
                // seperately output each child in building
                std::string output_file =  obj_id + ".json";
                std::ofstream out(output_file);
                json j_out = json::object();
                j_out["type"] = "CityJSON";
                j_out["version"] = "2.0";        
                j_out["CityObjects"] = json::object();
                j_out["CityObjects"][obj_id] = json::object();
                j_out["CityObjects"][obj_id]["type"] = "BuildingRoom";
                j_out["CityObjects"][obj_id]["geometry"] = j["CityObjects"][obj_id]["geometry"];
                j_out["CityObjects"][obj_id]["attributes"] = json::object();
                j_out["CityObjects"][obj_id]["attributes"]["label"] = label;

                j_out["vertices"] = j["vertices"];
                out << std::setw(4) << j_out << std::endl;
                out.close();}
        }
    std::ofstream o("building_final_result.json");
    o << std::setw(4) << j << std::endl;
    o.close();
        

}

    