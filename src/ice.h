#ifndef ICE_H
#define ICE_H

#include "iceVoxel.h"
#include <vector>

using namespace std;

struct Ice {
	//assumes ice starts from (0,0,0) and grows in positive direction for now
	Ice() {}
	Ice(float voxel_edge_length, int num_width_points, 
		int num_length_points, int num_height_points, 
		float initial_temperature, float ambient_temerature,
		float timestep);
	~Ice();
	void reset();

	void set_surface();

	void simulate();

	//Ice properties
	vector<IceVoxel> ice_voxels;
	//vector<Vector3D> point_heat_sources;
	float voxel_edge_length;

	float width;
	float length;
	float height;
	float ambient_temerature;
	float initial_temperature;
	float timestep;
	int num_width_points;
	int num_length_points;
	int num_height_points;
	const float air_transfer_coeff = 100;

	vector<IceVoxel> neighbors;
};


#endif
