#ifndef __geomtools__
#define __geomtools__
#define unlabelled -1
#include "definitions.h"


Plane                 get_best_fitted_plane(const std::vector<Point3> &lspts);
void                  mark_domains(CT& ct);
void                  mark_domains(CT& ct, CT::Face_handle start, int index, std::list<CT::Edge>& border);

std::vector<std::vector<int>>
construct_ct_one_face(const std::vector<std::vector<int>>& lsRings, 
                      const std::vector<Point3>& lspts);




struct Voxel{
public:
    Voxel(const Point3& corner,const Vector3& vx,const Vector3& vy,const Vector3& vz):corner(corner){
        label = unlabelled;}
    // label the Voxel during label
    Voxel(const Point3& corner,const Vector3& vx,const Vector3& vy,const Vector3& vz,const int& label_num,const std::vector<Face>& faces,const std::vector<Point3>& vertices)
    :Voxel(corner,vx,vy,vz){
        Point3 p0 = corner;
        Point3 p1 = corner+vx;
        Point3 p2 = corner+vx+vy;
        Point3 p3 = corner+vy;
        Point3 p4 = corner+vy+vz;
        Point3 p5 = corner+vz;
        Point3 p6 = corner+vx+vz;
        Point3 p7 = corner+vx+vy+vz;
        std::array<Point3,8> cube_vertices ={p0,p1,p2,p3,p4,p5,p6,p7}; 

        for(auto face: faces){
            bool intersect_or_not_with_this_face = intersect_faces_cubes(face,cube_vertices,vertices);
            if (intersect_or_not_with_this_face) {
                this->label = label_num;
                
                break;
            };

        }

    }
    bool intersect_faces_cubes(const Vector3& vx,const Vector3& vy,const Vector3& vz,const Face& face,const std::vector<Point3>& vertices){
        Point3 p0 = this->corner;
        Point3 p1 = p0+vx;
        Point3 p2 = p0+vx+vy;
        Point3 p3 = p0+vy;
        Point3 p4 = p0+vy+vz;
        Point3 p5 = p0+vz;
        Point3 p6 = p0+vx+vz;
        Point3 p7 = p0+vx+vy+vz;
       
        Point3 tri0 = vertices[face.indices[0]];
        Point3 tri1 = vertices[face.indices[1]];
        Point3 tri2 = vertices[face.indices[2]];
        Triangle3 triangle(tri0, tri1, tri2);
        Point3 end1 =  (p0+p1+p2+p3)/4;
        Point3 start1 =  (p4+p5+p6+p7)/4;
        Point3 end2 =  (p1+p2+p6+p7)/4;
        Point3 start2 =  (p3+p4+p5+p0)/4;
        Point3 end3 =  (p1+p6+p0+p5)/4;
        Point3 start3 =  (p2+p3+p4+p7)/4;
        Segment3 segment1(end1,start1);
        Segment3 segment2(end2,start2);

        Segment3 segment3(end3,start3);
        bool condition1 = CGAL::do_intersect(segment1, triangle);
        bool condition2 = CGAL::do_intersect(segment2, triangle);
        bool condition3 = CGAL::do_intersect(segment3, triangle);
        if (condition1 || condition2 || condition3){
            return true;
        }
        else
            return false;

    };

    bool intersect_faces_cubes(const Face& face,const std::array<Point3,8>& cube_vertices,const std::vector<Point3>& vertices){
        
        Point3 p0 = vertices[face.indices[0]];
        Point3 p1 = vertices[face.indices[1]];
        Point3 p2 = vertices[face.indices[2]];
        Triangle3 triangle(p0, p1, p2);
        Point3 end1 =  (cube_vertices[0]+cube_vertices[1]+cube_vertices[2]+cube_vertices[3])/4;
        Point3 start1 =  (cube_vertices[4]+cube_vertices[5]+cube_vertices[6]+cube_vertices[7])/4;
        Point3 end2 =  (cube_vertices[1]+cube_vertices[2]+cube_vertices[7]+cube_vertices[6])/4;
        Point3 start2 =  (cube_vertices[0]+cube_vertices[3]+cube_vertices[4]+cube_vertices[5])/4;
        Point3 end3 =  (cube_vertices[0]+cube_vertices[1]+cube_vertices[6]+cube_vertices[5])/4;
        Point3 start3 =  (cube_vertices[2]+cube_vertices[3]+cube_vertices[4]+cube_vertices[7])/4;
        Segment3 segment1(end1,start1);
        Segment3 segment2(end2,start2);

        Segment3 segment3(end3,start3);
        bool condition1 = CGAL::do_intersect(segment1, triangle);
        bool condition2 = CGAL::do_intersect(segment2, triangle);
        bool condition3 = CGAL::do_intersect(segment3, triangle);
        if (condition1 || condition2 || condition3){
            return true;
        }
        else
           return false;}
    Point3 corner;
    int label;
};

struct VoxelGrid {

  
  
  public:
    std::vector<Voxel> voxels;
    unsigned int max_x, max_y, max_z;
    double unit;
    Point3 initial_point;
    Vector3 vx,vy,vz;

    //
    VoxelGrid(const std::array<Point3, 8> & oobb, const double& unit) {
       
        vx = oobb[1]-oobb[0];
        vy = oobb[3]-oobb[0];
        vz = oobb[5]-oobb[0];
        // normalize it and calculate length
        double lengthx = CGAL::sqrt(vx.squared_length());
        double lengthy = CGAL::sqrt(vy.squared_length());
        double lengthz = CGAL::sqrt(vz.squared_length());
        max_x = int(lengthx/unit)+3;
        max_y = int(lengthy/unit)+3;
        max_z = int(lengthz/unit)+3;
        vx =  vx*unit/lengthx;
        vy =  vy*unit/lengthy;
        vz =  vz*unit/lengthz;
        initial_point = oobb[0]-vx;
        initial_point -=vy;
        initial_point -=vz;
        this->unit = unit;
        int total_voxels = max_x*max_y*max_z;
        //push_voxel(max_x, max_y,max_z);
        //std::cout<<total_voxels<<"total"<<std::endl;
        /*for(int x = 0;x<max_x;++x){
            for(int y= 0;y<max_y;++y){
                for(int z = 0;z<max_z;++z){
                    Point3 corner = initial_point+x*vx+y*vy+z*vz;
        //            std::cout<<corner<<" a corner of voxel is "<<std::endl;
                    voxels.push_back(Voxel(corner,vx,vy,vz));
                }
            }
        }*/


           }
    VoxelGrid(const std::array<Point3, 8> & oobb, const double& unit,const std::vector<Face>& faces, std::vector<Point3> vertices)
    :VoxelGrid(oobb, unit){

    }

    void push_voxel(){
        for(int x = 0;x<max_x;++x){
            for(int y= 0;y<max_y;++y){
                for(int z = 0;z<max_z;++z){
                    Point3 corner = initial_point+x*vx+y*vy+z*vz;
        //            std::cout<<corner<<" a corner of voxel is "<<std::endl;
                    voxels.push_back(Voxel(corner,vx,vy,vz));
                }
            }
        }

    }
    void push_voxel(const std::vector<Face>& faces, const std::vector<Point3>& vertices){
        for(int x = 0;x<max_x;++x){
            for(int y= 0;y<max_y;++y){
                for(int z = 0;z<max_z;++z){
                    Point3 corner = initial_point+x*vx+y*vy+z*vz;
        //            std::cout<<corner<<" a corner of voxel is "<<std::endl;
                    voxels.push_back(Voxel(corner,vx,vy,vz,1,faces,vertices));
                }
            }
        }

    }
    void push_voxel(const std::vector<Face>& faces, const std::vector<Point3>& vertices,const int& label_num)
    {
        // excecute this function, first make sure that all voxels have been pushed, if not push them first
       
        if(voxels.size()==0){
            this->push_voxel(faces,vertices);
            }
       
        for(auto face:faces){
            Point3 p0 = vertices[face.indices[0]];
            Point3 p1 = vertices[face.indices[1]];
            Point3 p2 = vertices[face.indices[2]];
            
            std::vector<std::vector<int>> xyz_range = extent_triangule(p0,p1,p2);
            int min_x_values = xyz_range[0][0];
            int max_x_values = xyz_range[0][1];

            if (min_x_values <0) min_x_values=0;
            if (max_x_values >max_x) max_x_values = max_x;

            int min_y_values = xyz_range[1][0];
            int max_y_values = xyz_range[1][1];

            if (min_y_values <0) min_y_values=0;
            if (max_y_values >max_y) max_y_values = max_y;

            int min_z_values = xyz_range[2][0];
            int max_z_values = xyz_range[2][1];

            if (min_z_values <0) min_z_values=0;
            if (max_z_values >max_z) max_z_values = max_z;
           
            
            
            
           for(int x = min_x_values;x<=max_x_values;++x){
                for(int y = min_y_values;y<=max_y_values;++y){
                    for(int z = min_z_values;z<=max_z_values;++z){
                        if(voxels[z + y*max_z + x*max_z*max_y].intersect_faces_cubes(vx,vy,vz,face,vertices)){
                            
                        voxels[z + y*max_z + x*max_z*max_y].label = label_num;}
                        

                    

                    }
                }
            }


        }
    }



    
    // coordinate conversion, convert arbitory point into initial_point with addition of 3 unit vectors
    std::vector<double> converstion(const Point3& pt){
        
        Vector3 decentralized =  pt - initial_point;


        
        double value_vx = (vx.x()*decentralized.x()+vx.y()*decentralized.y()+vx.z()*decentralized.z())/(unit*unit);
        double value_vy = (vy.x()*decentralized.x()+vy.y()*decentralized.y()+vy.z()*decentralized.z())/(unit*unit);
        double value_vz = (vz.x()*decentralized.x()+vz.y()*decentralized.y()+vz.z()*decentralized.z())/(unit*unit);
        std::vector<double> new_coord ={value_vx,value_vy,value_vz};
        
        return new_coord;
    }
    std::vector<std::vector<int>> extent_triangule(const Point3& p0,const Point3& p1,const Point3& p2){
        
        std::vector<double> pt_0 = converstion(p0);
        std::vector<double> pt_1 = converstion(p1);
        std::vector<double> pt_2 = converstion(p2);
        std::vector<double> xvalues = {pt_0[0],pt_1[0],pt_2[0]};
        double min_value = *std::min_element(xvalues.begin(), xvalues.end());
        int min_x = int(min_value)-2;
        double max_value = *std::max_element(xvalues.begin(), xvalues.end());
        int maxx = int(max_value)+2;
        std::vector<double> yvalues = {pt_0[1],pt_1[1],pt_2[1]};
        min_value = *std::min_element(yvalues.begin(), yvalues.end());
        int min_y = int(min_value)-2;
        max_value = *std::max_element(yvalues.begin(), yvalues.end());
        int maxy = int(max_value)+2;
        std::vector<double> zvalues = {pt_0[2],pt_1[2],pt_2[2]};
        min_value = *std::min_element(zvalues.begin(), zvalues.end());
        int min_z = int(min_value)-2;
        max_value = *std::max_element(zvalues.begin(), zvalues.end());
        int maxz = int(max_value)+2;
        std::vector<std::vector<int>> xyz_range = {{min_x,maxx},{min_y,maxy},{min_z,maxz}};
        return xyz_range;

    }
    void out_put_all_voxel_to_obj(const std::string& filename,const int& selected_label=1){
        std::ofstream out(filename);
        if (!out.is_open()) {std::cerr << "Error: could not open file \"" << filename << "\" for writing." << std::endl; return;}
        out << "# OBJ file generated by output_voxels_as_obj()" << std::endl;
        int vertex_index = 1;
        for(auto voxel:voxels){
            if (voxel.label!= selected_label) continue;
            Point3 p0 = voxel.corner;
            Point3 p1 = p0 + this->vx;
            Point3 p2 = p0 + this->vx+this->vy;
            Point3 p3 = p0+this->vy;
            Point3 p4 = p0+this->vy+this->vz;
            Point3 p5 = p0+this->vz;
            Point3 p6 = p0+this->vx+this->vz;
            Point3 p7 = p0+this->vx+this->vz+this->vy;


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
            vertex_index+=8;
             }
        out.close();

}
    void out_put_voxels_seperately(const std::string& filename,
    const std::vector<int>& labels){
        std::string full_filename;
        for(auto label:labels){
            std::cout<<"output"<<label<<std::endl;
            full_filename = filename+std::to_string(label)+".obj";
            out_put_all_voxel_to_obj(full_filename,label);
        };
        

    };
    // this function is used to ouput voxels to cityjson 1.1, after we labelled it
    json initialized_voxels_grid_tojson(const std::string& cityobject_id,
    const std::vector<int>& labels,const int& exterior_label, bool output_seperately = true){
        assert(voxels.size()>0);
        json j;
        // initialize the cityjson file
        j["type"] = "CityJSON";
        j["version"] = "1.1";
        j["transform"] = json::object();
        j["transform"]["scale"] = json::array();
        j["transform"]["translate"] = json::array();
        j["transform"]["scale"].push_back(1);j["transform"]["scale"].push_back(1);j["transform"]["scale"].push_back(1);
        j["transform"]["translate"].push_back(0);j["transform"]["translate"].push_back(0);j["transform"]["translate"].push_back(0);
        j["metadata"] = json::object();
        j["CityObjects"] = json::object();
        j["vertices"] = json::array();
        j["appearance"] = json::object();
        j["CityObjects"][cityobject_id] = json::object();
        j["CityObjects"][cityobject_id]["type"] = "Building";
        j["CityObjects"][cityobject_id]["children"] = json::array();
        for(auto label:labels){
            if(label == exterior_label) continue;
            std::string obj_id = cityobject_id+"_part_"+std::to_string(label);
            j["CityObjects"][obj_id] = json::object();
            j["CityObjects"][obj_id]["type"] = "BuildingRoom";
            j["CityObjects"][cityobject_id]["children"].push_back(obj_id);
            j["CityObjects"][obj_id]["geometry"] = json::array();
            j["CityObjects"][obj_id]["attributes"] = json::object();
            j["CityObjects"][obj_id]["attributes"]["label"] = label;
            j["CityObjects"][obj_id]["parents"] = json::array();
            j["CityObjects"][obj_id]["parents"].push_back(cityobject_id);
            
            }
        // store the vertices in voxels into cityjson   
        for(int x = 0;x <max_x;++x){
            for(int y = 0;y<max_y;++y ){
                for(int z = 0;z <max_z;++z){

                    json vertices = json::array();
                    Point3 pt_corner = voxels[z + y*max_z + x*max_z*max_y].corner;
                    double pt_x = pt_corner.x();
                    double pt_y = pt_corner.y();
                    double pt_z = pt_corner.z();
                    vertices.push_back(pt_x);
                    vertices.push_back(pt_y);
                    vertices.push_back(pt_z);
                    j["vertices"].push_back(vertices);
                    int voxel_label = voxels[z + y*max_z + x*max_z*max_y].label;
                    if(std::find(labels.begin(),labels.end(),voxel_label) == labels.end()) continue;
                    if(voxel_label != exterior_label && voxel_label != unlabelled){
                        std::string obj_id = cityobject_id+"_part_"+std::to_string(voxel_label);
                        json geometry;
                        geometry["type"] = "Solid";
                        geometry["lod"] = "2";
                        
                        json solid = json::array();    
                        int p0 = z + y*max_z + x*max_z*max_y;
                        int p1 = z + y*max_z + (x+1)*max_z*max_y;
                        int p2 = z + (y+1)*max_z + (x+1)*max_z*max_y;
                        int p3 = z + (y+1)*max_z + x*max_z*max_y;
                        int p4 = (z+1) + (y+1)*max_z + x*max_z*max_y;
                        int p5 = (z+1) + y*max_z + x*max_z*max_y;
                        int p6 = (z+1) + y*max_z + (x+1)*max_z*max_y;
                        int p7 = (z+1) + (y+1)*max_z + (x+1)*max_z*max_y;
                        
                        std::array<int,4> ring1 = {p3,p2,p1,p0};
                        std::array<int,4> ring2 = {p0,p1,p6,p5};
                        std::array<int,4> ring3 = {p3,p0,p5,p4};
                        std::array<int,4> ring4 = {p4,p5,p6,p7};
                        std::array<int,4> ring5 = {p7,p6,p1,p2};
                        std::array<int,4> ring6 = {p4,p7,p2,p3};
                        std::array<std::array<int,4>,1> face1;
                        std::array<std::array<int,4>,1> face2;
                        std::array<std::array<int,4>,1> face3;
                        std::array<std::array<int,4>,1> face4;
                        std::array<std::array<int,4>,1> face5;
                        std::array<std::array<int,4>,1> face6;
                        face1[0] = ring1;
                        face2[0] = ring2;
                        face3[0] = ring3;
                        face4[0] = ring4;
                        face5[0] = ring5;
                        face6[0] = ring6;
                        
                        solid.push_back(face1);
                        solid.push_back(face2);
                        solid.push_back(face3);
                        solid.push_back(face4);
                        solid.push_back(face5);
                        solid.push_back(face6);
                        geometry["boundaries"] = json::array();
                        geometry["boundaries"].push_back(solid);
                        j["CityObjects"][obj_id]["geometry"].push_back(geometry);
                        
                    }


                }
            }
            
        }
        if(output_seperately){
            for(auto& label:labels){
                if(label == exterior_label) continue;
                std::string obj_id = cityobject_id+"_part_"+std::to_string(label);
                // seperately output each child in building
                std::string output_file =  obj_id + ".json";
                std::ofstream out(output_file);
                json j_out = json::object();
                j_out["type"] = "CityJSON";
                j_out["version"] = "1.0";        
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
        //  according to the label of voxels, we store the boundary of each voxel into cityjson
        return j;
        }
    
    
    /*VoxelGrid(unsigned int x, unsigned int y, unsigned int z) {
    max_x = x;
    max_y = y;
    max_z = z;
    Ve
    unsigned int total_voxels = x*y*z;
    voxels.reserve(total_voxels);
    for (unsigned int i = 0; i < total_voxels; ++i) voxels.push_back(0);
  }*/

  std::vector<std::vector<int>> get_neighbours(int x, int y, int z, const int connectivity=6){
        // we only return 6 neighbors of a voxel
        std::vector<std::vector<int>> neighbours;
        if (x-1>=0) neighbours.push_back({x-1,y,z});
        if (x+1<this->max_x) neighbours.push_back({x+1,y,z});
        if (y-1>=0) neighbours.push_back({x,y-1,z});
        if (y+1<this->max_y) neighbours.push_back({x,y+1,z});
        if (z-1>=0) neighbours.push_back({x,y,z-1});
        if (z+1<this->max_z) neighbours.push_back({x,y,z+1});
        // another 12 edge neighbored voxels
        if (connectivity==18){
        if (x-1>=0 && y-1>=0) neighbours.push_back({x-1,y-1,z});
        if (x-1>=0 && y+1<this->max_y) neighbours.push_back({x-1,y+1,z});
        if (x+1<this->max_x && y-1>=0) neighbours.push_back({x+1,y-1,z});
        if (x+1<this->max_x && y+1<this->max_y) neighbours.push_back({x+1,y+1,z});
        if (x-1>=0 && z-1>=0) neighbours.push_back({x-1,y,z-1});
        if (x-1>=0 && z+1<this->max_z) neighbours.push_back({x-1,y,z+1});
        if (x+1<this->max_x && z-1>=0) neighbours.push_back({x+1,y,z-1});
        if (x+1<this->max_x && z+1<this->max_z) neighbours.push_back({x+1,y,z+1});
        if (y-1>=0 && z-1>=0) neighbours.push_back({x,y-1,z-1});
        if (y-1>=0 && z+1<this->max_z) neighbours.push_back({x,y-1,z+1});
        if (y+1<this->max_y && z-1>=0) neighbours.push_back({x,y+1,z-1});
        if (y+1<this->max_y && z+1<this->max_z) neighbours.push_back({x,y+1,z+1});}
        // another 8 corner neighbored voxels
        if (connectivity==26){
            if (x-1>=0 && y-1>=0 && z-1>=0) neighbours.push_back({x-1,y-1,z-1});
            if (x-1>=0 && y-1>=0 && z+1<this->max_z) neighbours.push_back({x-1,y-1,z+1});
            if (x-1>=0 && y+1<this->max_y && z-1>=0) neighbours.push_back({x-1,y+1,z-1});
            if (x-1>=0 && y+1<this->max_y && z+1<this->max_z) neighbours.push_back({x-1,y+1,z+1});
            if (x+1<this->max_x && y-1>=0 && z-1>=0) neighbours.push_back({x+1,y-1,z-1});    
            if (x+1<this->max_x && y-1>=0 && z+1<this->max_z) neighbours.push_back({x+1,y-1,z+1});
            if (x+1<this->max_x && y+1<this->max_y && z-1>=0) neighbours.push_back({x+1,y+1,z-1});
            if (x+1<this->max_x && y+1<this->max_y && z+1<this->max_z) neighbours.push_back({x+1,y+1,z+1});
            }
        return neighbours;

  };
  Voxel &operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) {
    assert(x >= 0 && x < max_x);
    assert(y >= 0 && y < max_y);
    assert(z >= 0 && z < max_z);
    return voxels[z + y*max_z + x*max_z*max_y];
  }
  
  Voxel operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) const {
    assert(x >= 0 && x < max_x);
    assert(y >= 0 && y < max_y);
    assert(z >= 0 && z < max_z);
    return voxels[z + y*max_z + x*max_z*max_y];
  }
};

#endif 

