#include "ice.h"
#include "stdio.h"
Ice::Ice(	float voxel_edge_length, int num_width_points, 
			int num_length_points, int num_height_points, 
			float initial_temperature, float ambient_temerature,
			float timestep) {

	this->voxel_edge_length = voxel_edge_length;
	this->num_width_points = num_width_points;
	this->num_length_points = num_length_points;
	this->num_height_points = num_height_points;
	this->ambient_temerature = ambient_temerature;
	this->initial_temperature = initial_temperature;
	this->timestep = timestep;
	width = voxel_edge_length * num_width_points;
	length = voxel_edge_length * num_length_points;
	height = voxel_edge_length * num_height_points;
	surface_voxels = new LinkedList();
	ice_voxels = vector<IceVoxel>();

	float volume = voxel_edge_length * voxel_edge_length * voxel_edge_length;

	ice_voxels = vector<IceVoxel>();
	for (int i = 0; i < num_width_points * num_length_points * num_height_points; i++) {
		ice_voxels.push_back(IceVoxel(initial_temperature, 1, volume));
	}
	set_surface();
	set_neighbors();
}




void Ice::reset() {
	/*
	this->voxel_edge_length = voxel_edge_length;
	this->num_width_points = num_width_points;
	this->num_length_points = num_length_points;
	this->num_height_points = num_height_points;
	this->ambient_temerature = ambient_temerature;
	this->initial_temperature = initial_temperature;
	this->timestep = timestep;
	*/
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
	set_neighbors();	
}



Ice::~Ice() {
	ice_voxels.clear();
}

void Ice::set_surface() {
	for (int z = 0; z < num_height_points; z++) {
		for (int y = 0; y < num_length_points; y++) {
			for (int x = 0; x < num_width_points; x++) {
				int i = x + y * num_width_points + z * num_width_points * num_length_points;
				if (x - 1 < 0 || x + 1 == num_width_points) {
					if (ice_voxels[i].surface) {
						continue;
					}
					ice_voxels[i].surface = true;
					ice_voxels[i].prev_surface = true;
					surface_voxels->insert(&ice_voxels[i]);					
				}
				else if (y - 1 < 0 || y +1 == num_length_points) {
					if (ice_voxels[i].surface) {
						continue;
					}
					ice_voxels[i].surface = true;
					ice_voxels[i].prev_surface = true;
					surface_voxels->insert(&ice_voxels[i]);					
				}
				else if (z - 1 < 0 || z + 1 == num_height_points) {
					if (ice_voxels[i].surface) {
						continue;
					}					
					ice_voxels[i].surface = true;
					ice_voxels[i].prev_surface = true;
					surface_voxels->insert(&ice_voxels[i]);					
				}					
			}
		}
	}
}

void Ice::set_neighbors() {
	for (int z = 0; z < num_height_points; z++) {
		for (int y = 0; y < num_length_points; y++) {
			for (int x = 0; x < num_width_points; x++) {
				IceVoxel& v = ice_voxels[x + y * num_width_points + z * num_width_points * num_length_points];
				if (x >= 1) {
					int n_index = (x - 1) + y * num_width_points + z * num_width_points * num_length_points;
					v.neighbors->push_back( &ice_voxels[n_index]);
				}
				if (x < num_width_points - 1) {
					int n_index = (x + 1) + y * num_width_points + z * num_width_points * num_length_points;
					v.neighbors->push_back( &ice_voxels[n_index]);
				}
				if (y >= 1) {
					int n_index = x + (y - 1) * num_width_points + z * num_width_points * num_length_points;
					v.neighbors->push_back( &ice_voxels[n_index]);						
				}
				if (y < num_length_points - 1) {
					int n_index = x + (y + 1) * num_width_points + z * num_width_points * num_length_points;
					v.neighbors->push_back( &ice_voxels[n_index]);					
				}
				if (z >= 1) {
					int n_index = x + y * num_width_points + (z - 1) * num_width_points * num_length_points;
					v.neighbors->push_back( &ice_voxels[n_index]);					
				}
				if (z < num_height_points - 1) {
					int n_index = x + y * num_width_points + (z + 1) * num_width_points * num_length_points;
					v.neighbors->push_back( &ice_voxels[n_index]);					
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

	surface_voxels->clean();

	vector<IceVoxel*> neighbors = vector<IceVoxel*>();

	Node* cur = surface_voxels->get_first();
	int count = 0;
	while (cur != 0) {
		count++;
		//printf("count %i\n", count);
		int num_neighbors = 0;
		neighbors.clear();

		if (cur->val->prev_state <= 0) {
			cur = cur->get_next();
			continue;
		}

		if (cur->val->prev_surface == false) {
			break;
		}

		//printf("current neighor size %i\n", cur->val->neighbors->size());
		for (IceVoxel* neighbor : *cur->val->neighbors) {
			if (neighbor->state > 0 ) {
				num_neighbors++;
				neighbors.push_back(neighbor);
			}
		}


		float surface_area = (6 - num_neighbors) * voxel_edge_length * voxel_edge_length;
		float work = air_transfer_coeff *surface_area * (ambient_temerature - cur->val->temperature);
		// apply work to surface voxels
		cur->val->simulate(timestep, work);

		if (cur->val->state <= 0.0) {
			//v completely melted after timestep

			//neighbors are now surface
			for (IceVoxel* n : neighbors) {
				if (n->surface || n->prev_surface) {
					continue;
				}
				n->surface = true;
				surface_voxels->insert(n);
			}
		}
		cur = cur->get_next();
	}


	// TODO: Point heat source
	// compute rays from surface voxels to heat point


	// apply work if no intersection


}


