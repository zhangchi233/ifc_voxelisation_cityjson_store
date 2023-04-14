# ifc_voxelisation_cityjson_store
vocalisation ifc files
# current progress
## 1. From IFC to OBJ done
use ifcconvert , we don't remove any furniture or windows or door
## 2. From OBJ to memory done
we parse the outputed surface into memory, if u want to check, u can call voxelgrid.out_put_all_voxel_to_obj to output voxel in selected label
## 4. Voxelisation of triangles done
# how to run the file
cd build
make
hw03 + the obj file from converted ifc file
u can generate several result:
1. a cityjson file with building exterior wall and building parts's reconstructed surface
2. seperate cityjson files with different building part
3. the obj form of reconstruction surface
4. the voxel of the exterior surface of wall and each building rooms named "voxellabelled"
exterior wall:
<img width="1014" alt="image" src="https://user-images.githubusercontent.com/60132725/232029937-f1881a12-fbcd-4af3-be13-912c54ca1b93.png">
each room:
<img width="635" alt="image" src="https://user-images.githubusercontent.com/60132725/232030103-c1ee5b01-674c-40a7-87e3-99956bb37ecd.png">
cityjson file: 
<img width="1186" alt="image" src="https://user-images.githubusercontent.com/60132725/232030310-78b8d0f7-7438-4799-891d-aa06848a93e5.png">

<img width="1234" alt="image" src="https://user-images.githubusercontent.com/60132725/232030410-02b113c2-22e4-4509-9c83-a1774403c98c.png">
<img width="1209" alt="image" src="https://user-images.githubusercontent.com/60132725/232030497-6023d2b1-2b5e-4a74-9140-253ce0276af9.png">
generated file list:
<img width="1358" alt="image" src="https://user-images.githubusercontent.com/60132725/232030626-7a03f244-d7f1-47e1-a46c-29a1ef30caf2.png">
# meaning of different output
voxels_labelled: the intermediate voxel of labelled room and wall envelope(voxels_labelled.obj. the obj file of voxels labelled as different building part, 1 is wall, 0 is exterior, bigger than 1 is different rooms, -1 is unlabelled meaning wall in the interior of building or small room that is too small to reach x,y,z threhold in threhold, the -1 labelled is not oupt and remained as vaccum, different labels is outputted differently)
voxel_label_part: result of marching cube(label_voxel_part_label.obj, the isosurface of marching suce)
cityjson file: the output result on cityjson(building_final_result.json is assembled cityjson including all building part, label_voxel_part_num.json is different parts/rooms of building)
# how to change resolution
the default resolution is 0.25, u can open main.cpp and change variable unit to adjust resolution
but we strongly dont recommend u to do that, because we dont implement octree method to merge those adjacent voxels, so the output result consist of many small meshes and take loads of space, that's why we only use 0.25, but u can have a try to do that, as long as ur computer has enough memory
