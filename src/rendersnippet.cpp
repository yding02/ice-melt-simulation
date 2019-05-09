#include "ice.h"

//Have these parameters somewhere (could be hard coded)
int num_width_points, num_length_points, num_height_points;
float voxel_edge_length;
float initial_temperature;
float ambient_temerature;
float timestep;
float steps_per_frame;

//Initilization
Ice ice = Ice(voxel_edge_length, 
	num_width_points, 
	num_length_points, 
	num_height_points,
	initial_temperature,
	ambient_temerature,
	timestep)

//Extraction of voxel information from ice struct
vector<float> ice_to_voxels(Ice* ice) {
	vector<IceVoxel> ice_voxels = ice->ice_voxels;
	vector<float> voxel_data = vector<float>();
	for (int z = 0; z < num_height_points; z++) {
		for (int y = 0; y < num_length_points; y++) {
			for (int x = 0; x < num_width_points; x++) {
				int index = x + y * num_width_points + z * num_width_points * num_length_points;
				voxel_data.pushback(x);
				voxel_data.pushback(y);
				voxel_data.pushback(z);
				voxel_data.pushback(0.0); //R
				voxel_data.pushback(0.0); //G
				voxel_data.pushback(1.0); //B
				voxel_data.pushback(ice_voxels[index].state); //A
			}
		}
	}
	return voxel_data;
}

//Render loop
int i = 0;
while (1) {
	if (i % steps_per_frame == 0) {
		// TODO: wait time required to ensure fps cap

		// TODO: render
		vector<float> voxels = ice_to_voxels(ice);
	}
	ice.simulate();
	i++;
}

