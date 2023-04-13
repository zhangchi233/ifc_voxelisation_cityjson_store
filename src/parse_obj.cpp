#include "parse_obj.h"

std::map<std::string, Object> objects;


using namespace std;

std::pair<std::vector<Face>,std::map<std::string, Object>> parse_obj(const std::string& filename,std::vector<Point3>& vertices){
	
	
	// parse obj file and store it into Object, Shell, vertex and face
	
	std::vector<Face> faces;
	std::ifstream input_stream;
	input_stream.open(filename, std::ios::in);
	std::string line;
	Object new_object;
	Shell new_shell;
	std::map<std::string, Object> object_map;
	int total_objecct = 0;
	while (getline(input_stream, line)) {
		std::istringstream iss(line);
		string token;
		iss>>token;
		//std::cout<<line<<std::endl;
		if (line.substr(0,2) == "g "){
			total_objecct+=1;
			string id;
			iss>>id;
			Object another_new_object;
			another_new_object.id = id;
			if (new_shell.faces.size() != 0){
				//std::cout<<"id address of new shell"<<&new_shell<<std::endl;
				new_object.shells.push_back(new_shell);
				object_map.insert(std::make_pair(new_object.id, new_object));
				new_object = another_new_object;}
			else {
				new_object = another_new_object;
			}
		}
		//read shell
		else if (line[0] == 's'){
			
			//new_object.shells.push_back(new_shell);
			Shell another_new_shell;
			//std::cout<<"id address of anotehr new shell"<<&another_new_shell<<std::endl;
			new_shell = another_new_shell;
			//std::cout<<"id address of updated new shell"<<&new_shell<<std::endl;
			
		}
		else if (line.substr(0, 2) == "us"){
			string use_material;
			iss>>use_material;
			new_shell.use_material = use_material;
			}
		
		// read faces and store the whole faces into a vector, 
		else if (line.substr(0,2) == "f "){
			Face new_face;
			int v1,v2,v3;
            char c;
			try{
            iss>>v1>>c>>c>>v1>>v2>>c>>c>>v2>>v3>>c>>c>>v3;}
			catch(const std::exception& ex){
				
				iss>>v1>>v2>>v3;
				}
			new_face.indices.push_back(v1-1);
			new_face.indices.push_back(v2-1);
			new_face.indices.push_back(v3-1);
			// append them to current shells
			faces.push_back(new_face);
			new_shell.faces.push_back(new_face);
			
			
			}
		// read vertices, just store them into the verteices vector
		else if (line.substr(0,2) == "v "){
			double ptx,pty,ptz;
			iss>>ptx>>pty>>ptz;
			Point3 new_point(ptx,pty,ptz);
			vertices.push_back(new_point);
		}
		// remember to push the last object and shell into object_map
		
	}
	
	new_object.shells.push_back(new_shell);
	object_map.insert(std::make_pair(new_object.id, new_object));
		
	//std::cout<<"object_map size: "<<object_map.size()<<" "<<total_objecct<<" faces is"<<faces.size()<<" total vertices"<<vertices.size()<<std::endl;
	input_stream.close();
    
    return std::make_pair(faces,object_map);
}
void stored_faces_obj(std::string filename,const std::vector<Face>& faces,const std::vector<Point3>& vertices){
	std::ofstream out(filename);
	for(auto pt:vertices){
		Point3 p0 = pt;
		out << "v " <<  p0.x()<< " " << p0.y() << " " << p0.z()<< std::endl;
	}
	for(auto face:faces){
		std::vector<int> indices = face.indices;
		out << "f " << indices[0]+1<<"//"<< indices[0]+1<<" " << indices[1]+1<<"//"<< indices[1]+1<<" " << indices[2]+1<<"//"<< indices[2]+1<<std::endl;

	}
	
	out.close();
}
std::array<Point3, 8> generate_oobb_building(const std::vector<Face>& faces,const std::vector<Point3>& vertices){
	std::array<Point3, 8> obb_points;
	vector<Point3> BBD_pt;
	for(auto face:faces){
		BBD_pt.push_back(vertices[face.indices[0]]);
		BBD_pt.push_back(vertices[face.indices[1]]);
		BBD_pt.push_back(vertices[face.indices[2]]);

	}
	CGAL::oriented_bounding_box(BBD_pt, obb_points);
	return obb_points;
}
