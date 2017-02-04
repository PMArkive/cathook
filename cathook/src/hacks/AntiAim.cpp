/*
 * AntiAim.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: nullifiedcat
 */

#include "AntiAim.h"

#include "../common.h"
#include "../sdk.h"

DEFINE_HACK_SINGLETON(AntiAim);

const char* AntiAim::GetName() {
	return "ANTI-AIM";
}

AntiAim::AntiAim() {
	this->v_bEnabled = new CatVar(CV_SWITCH, "aa_enabled", "0", "Enable AntiAim", NULL, "Master AntiAim switch");
	this->v_flPitch = new CatVar(CV_FLOAT, "aa_pitch", "-89.0", "Pitch", NULL, "Static pitch (up/down)", true, 89.0, -89.0);
	this->v_flYaw = new CatVar(CV_FLOAT, "aa_yaw", "0.0", "Yaw", NULL, "Static yaw (left/right)", true, 360.0);
	this->v_flSpinSpeed = new CatVar(CV_FLOAT, "aa_spin", "10.0", "Spin speed", NULL, "Spin speed (in deg/sec)");
	this->v_PitchMode = new CatVar(CV_ENUM, "aa_pitch_mode", "1", "Pitch mode", new CatEnum({ "KEEP", "STATIC", "RANDOM" }), "Pitch mode");
	this->v_YawMode = new CatVar(CV_ENUM, "aa_yaw_mode", "3", "Yaw mode", new CatEnum({ "KEEP", "STATIC", "RANDOM", "SPIN" }), "Yaw mode");
}

float yaw = -180;
float pitch = -89;

bool AntiAim::CreateMove(void*, float, CUserCmd* cmd) {
	if (!this->v_bEnabled->GetBool()) return true;
	if (cmd->buttons & IN_USE) {
		return true;
	}
	if (cmd->buttons & IN_ATTACK) {
		if (CanShoot()) return true;
	}
	if ((cmd->buttons & IN_ATTACK2) && g_pLocalPlayer->weapon()->m_iClassID == g_pClassID->CTFLunchBox) return true;
	if (g_pLocalPlayer->bAttackLastTick) return true;
	if (GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_melee ||
			GetWeaponMode(g_pLocalPlayer->entity) == weaponmode::weapon_throwable) return true;
	float p = cmd->viewangles.x;
	float y = cmd->viewangles.y;
	switch (this->v_YawMode->GetInt()) {
	case 1: // FIXED
		y = this->v_flYaw->GetFloat();
		break;
	case 2: // RANDOM
		y = RandFloatRange(-180.0f, 180.0f);
		break;
	case 3: // SPIN
		yaw += v_flSpinSpeed->GetFloat();
		if (yaw > 180) yaw = -180;
		y = yaw;
		break;
	}
	switch (this->v_PitchMode->GetInt()) {
	case 1:
		p = this->v_flPitch->GetFloat();
		break;
	case 2:
		p = RandFloatRange(-89.0f, 89.0f);
		break;
	}

	Vector angl = Vector(p, y, 0);
	fClampAngle(angl);
	//angl.z = 180;
	cmd->viewangles = angl;
	g_pLocalPlayer->bUseSilentAngles = true;
	return false;
}

void AntiAim::PaintTraverse(void*, unsigned int, bool, bool) {}

void AntiAim::LevelInit(const char*) {}
void AntiAim::LevelShutdown() {}
