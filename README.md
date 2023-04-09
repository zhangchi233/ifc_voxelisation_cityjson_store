# ifc_voxelisation_cityjson_store
vocalisation ifc files
# current progress
## 1. From IFC to OBJ done
## 2. From OBJ to memory done
## 3. The voxel grid done
## 4. Voxelisation of triangles done
current effect:
we can voxelisation all the voxels and output voxels intersected with building's faces:
<img width="807" alt="image" src="https://user-images.githubusercontent.com/60132725/229597556-8247f174-0dac-42ea-95f2-091e32e7c5ca.png">

<img width="803" alt="image" src="https://user-images.githubusercontent.com/60132725/229597660-fe129ea5-8009-4de6-a1b1-86fc802c0964.png">
inside the building: ladder:
<img width="1473" alt="image" src="https://user-images.githubusercontent.com/60132725/229598302-ce10067d-aac6-42f0-8199-4202125fb6f7.png">

# next jobs u can do:
## slightly change the voxel face intersect method: done
current method we stored the faces together and then intersect voxels with the whole faces,
change the push_voxel function in voxel class, change the vector<faces> to map<string, object>, and label it according to the targeted labels
## label and voxels which is surface and room,  done
iterate the voxels and according to its connected voxel label to label un labelled voxel: label is0:  done
## speed up voxelisation: done
calculate bounding box of one face and extract sourrounding voxels to speed up, not like current method iterate the whole voxels
## write an function output voxelgrid class to cityjson unfinished
with label, object and building according to format and preserve building id, building parts and so on
# current effect:
we can clearly see the exterior surface and different components of room, one thin is that isolated small sapce emerge, im not sure it is a problem or not
## effect of interior room and exterior boundary:
<img width="899" alt="image" src="https://user-images.githubusercontent.com/60132725/229857400-5461e8c2-fc51-4d50-80b7-1e824cff74fd.png">
the effect of resolution 0.5 is really bad, lets try 0.25 voxels
now the effect is much better, the default resolution is 0.25, u can have a try, but the issue of silver room is much moreserious
<img width="685" alt="image" src="https://user-images.githubusercontent.com/60132725/229858177-ed225108-7bbf-4be7-8b60-fb665558a573.png">
u can see the size of different rooms, (those small size files)
<img width="1357" alt="image" src="https://user-images.githubusercontent.com/60132725/229858421-28ea5cfe-72a9-409a-a2a7-294d9bfd04fb.png">
## possible job u can boost: label the wall according to the object id and merge it seperately(but consider the shell number is 214) it will make task much more worse, maybe filtered and don't read those faces that is not walls is better(except windows, doors, wall,floor, we don't read any other object like chair and table)

# newly updated progress and how to run: fix the small size room issues, and the wall which is not exterior surface will not be labelled, and current out put oupt the room evnvelope and exterior surface in building_test.cityjson, where each room and exterior surface is building part with label
<img width="1132" alt="image" src="https://user-images.githubusercontent.com/60132725/230770232-acfebe9b-21ec-4119-896e-a91f7399ddbf.png">
each room inside the building:
<img width="768" alt="image" src="https://user-images.githubusercontent.com/60132725/230770287-59bc3c07-7b2a-4a01-8688-f15b573f4be9.png">
i also output the obj version, including wall, room so that it is convienent to check

## parameters and setting:
<img width="735" alt="image" src="https://user-images.githubusercontent.com/60132725/230770335-b02f3136-5c62-400d-b3a2-8294492b199d.png">
output_label: which labelled part u want to output
unit: resolution of voxel
intermediate: whether output intermediate obj files
## initialize the voxel grid and select label to label wall/floow(we assume that in obj files all faces remained are walls/floors)
### wall_surface: the label value u want to choose to be as wall/floor must be > 0

<img width="940" alt="image" src="https://user-images.githubusercontent.com/60132725/230770413-91c0e345-ead5-4e79-81e0-d6e49c1703c6.png">

<img width="940" alt="image" src="https://user-images.githubusercontent.com/60132725/230770494-22f1c5dd-b291-4f6f-b823-12decf1c3d1a.png">
<img width="657" alt="image" src="https://user-images.githubusercontent.com/60132725/230770523-01762c51-e4ca-42a7-a881-511740ff5737.png">
## label_voxels(VoxelGrid& voxelgrid,const int& label_initial = 0,
const int& wall_face_label=1,const bool& check_width = true,const int& minimum_width = 2)
exterior_label, label to be exterior surface
wall index: the wall has been labeled
check_width: whether to filter small space, minimum_width: the smallest length of h, w, l of cubic room
<img width="865" alt="image" src="https://user-images.githubusercontent.com/60132725/230770622-eda67fa7-d678-47f8-899d-ddc5078b45c5.png">
## label_wall: whether not label interior wall which is not exterior surface, if true, not labeled
<img width="805" alt="image" src="https://user-images.githubusercontent.com/60132725/230770682-32bbd09b-b6d2-45aa-8558-562b0f5c0783.png">
output_exterior_surface_to_obj: exterior surface of building envelope only
voxels.out_put_all_voxel_to_obj("output_voxels.obj",output_label): output selected label 
    voxels.out_put_voxels_seperately("label_voxels",labells): output all labelled part
    }
json j = voxels.initialized_voxels_grid_tojson("building_test",labells, exterior_label): output the whole building and seperately parts
    






# new update progress and how to run: fix the small size room issues, and the wall which is not exterior surface will not be labelled, and current out put oupt the room evnvelope and exterior surface in building_test.cityjson, where each room and exterior surface is building part with label
