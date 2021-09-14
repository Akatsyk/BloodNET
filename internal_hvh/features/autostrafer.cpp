#include "../include_cheat.h"

float speed, ideal, old_yaw;
float  switch_value = 1.f;

void autostrafer::strafe()
{
	Vector velocity;
	float  delta, abs_delta, velocity_delta, correct;
	if (!vars.misc.autostrafe.get<bool>())
		return;

	if (g_pLocalPlayer->get_flags() & FL_ONGROUND)
		return;

	if (g_pLocalPlayer->get_move_type() != MOVETYPE_WALK || !g_pLocalPlayer->get_alive())
		return;

	if (g_cmd->buttons & IN_SPEED)
		return;

	__(cl_sidespeed_s, "cl_sidespeed");
	static ConVar* cl_sidespeed = g_pCVar->FindVar(cl_sidespeed_s);

	// get networked velocity ( maybe absvelocity better here? ).
	// meh, should be predicted anyway? ill see.
	velocity = g_pLocalPlayer->get_abs_velocity();

	// get the velocity len2d ( speed ).
	speed = velocity.Length2D();

	// compute the ideal strafe angle for our velocity.
	ideal = (speed > 0.f) ? RAD2DEG(asin(15.f / speed)) : 90.f;

	// some additional sanity.
	clamp(ideal, 0.f, 90.f);

	// for changing direction.
	// we want to change strafe direction every call.
	switch_value *= -1.f;

	float wish_dir{ };

	// get our key presses.
	bool holding_w = g_cmd->buttons & IN_FORWARD;
	bool holding_a = g_cmd->buttons & IN_MOVELEFT;
	bool holding_s = g_cmd->buttons & IN_BACK;
	bool holding_d = g_cmd->buttons & IN_MOVERIGHT;

	// move in the appropriate direction.
	if (holding_w) {
		if (holding_a)
			wish_dir += 45.f;
		else if (holding_d)
			wish_dir -= 45.f;
		else
			wish_dir += 0.f;
	}
	else if (holding_s) {
		if (holding_a)
			wish_dir += 135.f;
		else if (holding_d)
			wish_dir -= 135.f;
		else
			wish_dir += 180.f;
	}
	else if (holding_a)
		wish_dir += 90.f;
	else if (holding_d)
		wish_dir -= 90.f;

	autostrafer::get().m_strafe_angles.y = math::get().normalize_float(autostrafer::get().m_strafe_angles.y + wish_dir);

	// cancel out any forwardmove values.
	g_cmd->forwardmove = 0.f;

	// get our viewangle change.
	delta = math::get().normalize_float(autostrafer::get().m_strafe_angles.y - old_yaw);

	// convert to absolute change.
	abs_delta = abs(delta);

	// save old yaw for next call.
	old_yaw = autostrafer::get().m_strafe_angles.y;

	// set strafe direction based on mouse direction change.
	if (delta > 0.f)
		g_cmd->sidemove = -cl_sidespeed->get_float();
	else if (delta < 0.f)
		g_cmd->sidemove = cl_sidespeed->get_float();

	// we can accelerate more, because we strafed less then needed
	// or we got of track and need to be retracked.
	if (abs_delta <= ideal || abs_delta >= 30.f) {
		// compute angle of the direction we are traveling in.
		QAngle velocity_angle;
		math::get().vector_angles(velocity, velocity_angle);

		// get the delta between our direction and where we are looking at.
		velocity_delta = math::get().normalize_float(autostrafer::get().m_strafe_angles.y - velocity_angle.y);

		// correct our strafe amongst the path of a circle.
		correct = ideal;

		if (velocity_delta <= correct || speed <= 15.f) {
			// not moving mouse, switch strafe every tick.
			if (-correct <= velocity_delta || speed <= 15.f) {
				autostrafer::get().m_strafe_angles.y += (ideal * switch_value);
				g_cmd->sidemove = cl_sidespeed->get_float() * switch_value;
			}

			else {
				autostrafer::get().m_strafe_angles.y = velocity_angle.y - correct;
				g_cmd->sidemove = cl_sidespeed->get_float();
			}
		}

		else {
			autostrafer::get().m_strafe_angles.y = velocity_angle.y + correct;
			g_cmd->sidemove = -cl_sidespeed->get_float();
		}
	}
}
