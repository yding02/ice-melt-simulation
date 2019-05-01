#include "ice.h"

Ice::Ice(	float voxel_edge_length, int num_width_points, 
			int num_length_points, int num_height_points, 
			float initial_temperature, float ambient_temerature,
			float timestep) {

	this->voxel_edge_length = voxel_edge_length;
	this->num_width_points = num_width_points;
	this->num_length_points = num_length_points;
	this->num_height_points = num_height_points;
	this->ambient_temerature = ambient_temerature;
	this->timestep = timestep;
	width = voxel_edge_length * num_width_points;
	length = voxel_edge_length * num_length_points;
	height = voxel_edge_length * num_height_points;


	float volume = voxel_edge_length * voxel_edge_length * voxel_edge_length;

	ice_voxels = vector<IceVoxel>();
	for (int i = 0; i < num_width_points * num_length_points * num_height_points; i++) {
		ice_voxels.push_back(IceVoxel(initial_temperature, 1, volume));
	}
}

Ice::~Ice() {
	ice_voxels.clear();
}

void Ice::set_surface() {
	for (int z = 0; z < num_height_points; z++) {
		for (int y = 0; y < num_length_points; y++) {
			for (int x = 0; x < num_width_points; x++) {
				if (x - 1 < 0 || x + 1 == num_width_points) {
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].surface = true;
				}
				else if (y - 1 < 0 || y +1 == num_length_points) {
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].surface = true;
				}
				else if (z - 1 < 0 || z + 1 == num_height_points) {
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].surface = true;
				}					
			}
		}
	}
}

void Ice::simulate() {
	// TODO background heat
	// apply work to surface voxels


	// TODO: Point heat source
	// compute rays from surface voxels to heat point


	// apply work if no intersection


}


