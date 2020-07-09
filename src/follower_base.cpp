
#include "follower_base.hpp"

float FollowerBase::sensitivity(Param& dampParam, Input* dampInput, int c) {
	float response = clamp(dampParam.getValue(), 0.0f, 1.0f);
	if (dampInput && dampInput->isConnected()) {
		response *= clamp(dampInput->getPolyVoltage(c) / 10.f, 0.0f, 1.0f);
	}
	response = 1.0f - response;
	response *= response;
	response *= response;
	return response;
}

float FollowerBase::gain(Param& gainParam, Input* gainInput, int c) {
	float db = clamp(gainParam.getValue(), -1.0f, 1.0f);
	if (gainInput && gainInput->isConnected()) {
		db *= clamp(gainInput->getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	if (db < 0.0f) {
		db = -db * efGainMinDecibels;
	}
	else {
		db *= std::min(12.0f, efGainMaxDecibels);
	}
	return db;
}
