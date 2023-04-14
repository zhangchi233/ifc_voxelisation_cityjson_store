# include "marching_cubes.h"
using namespace std;
std::pair<int,std::vector<vector<Point3>>> get_isosurfaces(const cell onecell,const int isovalue);
void extract_isosurface(const cell onecell,
std::map<int,std::vector<vector<Point3>>>& mesh_faces){
   
    int cubeIndex = 0;
    int label = -1;
    // smallest in vector<int> values
    auto iso = std::max_element(onecell.values.begin(),onecell.values.end());
    label = *iso;
    int isovalue = *iso;

    for(auto isovalue:onecell.values){
        if (isovalue > 0){
            auto result = get_isosurfaces(onecell,isovalue);
            int label_result = result.first;
            std::vector<std::vector<Point3>> faces = result.second;
            if(faces.size() == 0) continue;
            mesh_faces[label_result].insert(mesh_faces[label_result].end(),faces.begin(),faces.end());
            }
    };
}
std::pair<int,std::vector<vector<Point3>>> get_isosurfaces(const cell onecell,const int isovalue){
    int label = isovalue;
    int cubeIndex = 0;
    
    for (int i = 0; i < 8; i++)
        {
        

        
        if (onecell.values[i] < isovalue) 
        {cubeIndex |= (1 << i);
        }
        }
        
    int intersectionsKey = edgeTable[cubeIndex];
    int idx = 0;
    std::vector<Point3> intersections (12);
    while (intersectionsKey)
    {
        if (intersectionsKey&1)
        {
            int v1 = edgeToVertices[idx].first, v2 = edgeToVertices[idx].second;
            Point3 intersectionPoint = (onecell.points[v1]+onecell.points[v2])/2;
            intersections[idx] = intersectionPoint;
        }
        idx++;
        intersectionsKey >>= 1;
    
    }
    std::vector<std::vector<Point3>> triangles;
    for (int i = 0; triangleTable[cubeIndex][i] != -1; i += 3)
    {
        std::vector<Point3> triangle (3);
        for (int j = 2; j > -1; j--)
            triangle[2-j] = intersections[triangleTable[cubeIndex][i + j]];
        triangles.push_back(triangle);

    }
    return std::make_pair(label,triangles);

}










/*
void (const std::string filename, const std::vector<Point3> &points,const std::vector<Vector3> &normals,const int & label){
    int cubeIndex = 0;
    for (int i = 0; i < 8; i++)
        if (cell.value[i] < isovalue) cubeIndex |= (1 << i);
    int intersectionsKey = edgeTable[cubeIndex];
    std::vector<std::vector<Point>> triangles;
    for (int i = 0; triangleTable[cubeIndex][i] != -1; i += 3)
    {
        std::vector<Point> triangle (3);
        for (int j = 0; j < 3; j++)
            triangle[j] = intersections[triangleTable[cubeIndex][i + j]];
        triangles.push_back(triangle);
    }
};
*/

