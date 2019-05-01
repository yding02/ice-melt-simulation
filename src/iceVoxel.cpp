#include "iceVoxel.h"
#include <math.h>

IceVoxel::IceVoxel(float temperature, float state, float volume) {
	this->temperature = temperature;
	this->state = state;
	this->volume = volume;
}

IceVoxel::~IceVoxel() {
	//deconstructor for later use
}


void IceVoxel::simulate(float timestep, float power) {
	if (state <= 0) {
		return;
	}

	float energy = timestep * power;
	if (temperature < melting_point) {
		float temp_change = energy / specific_heat / (volume * density);
		float max_temp_change = melting_point - temperature;
		temp_change = fminf(temp_change, max_temp_change);
		float consumed_energy = temp_change * specific_heat * volume * density;
		energy -= consumed_energy;
		temperature += temp_change;
	}

	float melted_volume = energy / fusion_enthalpy / density;
	state = fmaxf(0, state - melted_volume/volume);
}
