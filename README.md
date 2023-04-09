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

