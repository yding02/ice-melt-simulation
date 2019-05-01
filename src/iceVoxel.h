#ifndef ICEVOXEL_H
#define ICEVOXEL_H

using namespace std;

struct IceVoxel {
	IceVoxel() {}
	IceVoxel(float temperature, float state, float volume);
	~IceVoxel();

	void simulate(float timestep, float power);

	//Ice properties
	float temperature;
	float state;
	float volume;
	const float melting_point = 273; // K
	const float fusion_enthalpy = 333.55; // J/g
	const float density = 0.9167; // g/cm^3
	const float specific_heat = 2.108; // J/gK
}

#endif
