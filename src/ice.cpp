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
	ice_voxels = vector<IceVoxel>();

	float volume = voxel_edge_length * voxel_edge_length * voxel_edge_length;

	ice_voxels = vector<IceVoxel>();
	for (int i = 0; i < num_width_points * num_length_points * num_height_points; i++) {
		ice_voxels.push_back(IceVoxel(initial_temperature, 1, volume));
	}
	set_surface();
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
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].prev_surface = true;					
				}
				else if (y - 1 < 0 || y +1 == num_length_points) {
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].surface = true;
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].prev_surface = true;
				}
				else if (z - 1 < 0 || z + 1 == num_height_points) {
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].surface = true;
					ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points].prev_surface = true;
				}					
			}
		}
	}
}

void Ice::simulate() {
	// TODO background heat

	// update previous states
	for (IceVoxel &v : ice_voxels) {
		v.prev_surface = v.surface;
		v.prev_state = v.state;
	}


	for (int z = 0; z < num_height_points; z++) {
		for (int y = 0; y < num_length_points; y++) {
			for (int x = 0; x < num_width_points; x++) {
				IceVoxel& v = ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points];
				if (v.state <= 0) {
					//v is already melted
					continue;
				}
				if (v.prev_surface) {
					//v is a surface voxel

					//calcuate surface area
					int num_neighbors = 0;
					neighbors.clear();
					//a voxel has 6 faces
					if (x >= 1) {
						int n_index = (x - 1) + y * num_width_points + z * num_width_points * num_length_points;
						IceVoxel& n = ice_voxels[n_index];
						if (n.prev_state >= 0) {
							num_neighbors++;
							neighbors.push_back(n);
						}
					}
					if (x < num_width_points - 1) {
						int n_index = (x + 1) + y * num_width_points + z * num_width_points * num_length_points;
						IceVoxel& n = ice_voxels[n_index];
						if (n.prev_state >= 0) {
							num_neighbors++;
							neighbors.push_back(n);
						}						
					}
					if (y >= 1) {
						int n_index = x + (y - 1) * num_width_points + z * num_width_points * num_length_points;
						IceVoxel& n = ice_voxels[n_index];
						if (n.prev_state >= 0) {
							num_neighbors++;
							neighbors.push_back(n);
						}						
					}
					if (y < num_length_points - 1) {
						int n_index = x + (y + 1) * num_width_points + z * num_width_points * num_length_points;
						IceVoxel& n = ice_voxels[n_index];
						if (n.prev_state >= 0) {
							num_neighbors++;
							neighbors.push_back(n);							
						}						
					}
					if (z >= 1) {
						int n_index = x + y * num_width_points + (z - 1) * num_width_points * num_length_points;
						IceVoxel& n = ice_voxels[n_index];
						if (n.prev_state >= 0) {
							num_neighbors++;
							neighbors.push_back(n);
						}						
					}
					if (z < num_height_points - 1) {
						int n_index = x + y * num_width_points + (z + 1) * num_width_points * num_length_points;
						IceVoxel& n = ice_voxels[n_index];
						if (n.prev_state >= 0) {
							num_neighbors++;
							neighbors.push_back(n);
						}						
					}

					float surface_area = (6 - num_neighbors) * voxel_edge_length * voxel_edge_length;
					float work = air_transfer_coeff *surface_area * (ambient_temerature - v.temperature);
					// apply work to surface voxels
					v.simulate(timestep, work);

					if (v.state <= 0) {
						//v completely melted after timestep

						//neighbors are now surface
						for (IceVoxel &n : neighbors) {
							n.surface = true;
						}
					}
				}
			}
		}
	}	


	// TODO: Point heat source
	// compute rays from surface voxels to heat point


	// apply work if no intersection


}


