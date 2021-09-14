#include "../include_cheat.h"

void grenade_pred::predict()
{
	//	readability.
	constexpr float restitution = 0.45f;
	constexpr float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	constexpr float velocity = 403.0f * 0.9f;

	float step, gravity, new_velocity, unk01;
	int index{}, grenade_act{ 1 };
	Vector pos, thrown_direction, start, eye_origin;
	Vector angles;
	QAngle qangles, thrown;

	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	//	first time setup.
	__(sv_gravity_s, "sv_gravity");
	static auto sv_gravity = g_pCVar->FindVar(sv_gravity_s);

	//	calculate step and actual gravity value.
	gravity = sv_gravity->get_float() / 8.0f;
	step = g_pGlobals->interval_per_tick;

	//	get viewangles.
	qangles = misc::get().g_real_angles;

	//	copy current angles and normalise pitch.
	thrown = qangles;

	if (thrown[0] < 0)
		thrown[0] = -10 + thrown[0] * ((90 - 10) / 90.0f);
	else
		thrown[0] = -10 + thrown[0] * ((90 + 10) / 90.0f);

	//	find out how we're throwing the grenade.
	auto primary_attack = g_cmd->buttons & IN_ATTACK;
	auto secondary_attack = g_cmd->buttons & IN_ATTACK2;

	if (primary_attack && secondary_attack) {
		grenade_act = ACT_LOB;
	}
	else if (secondary_attack) {
		grenade_act = ACT_DROP;
	}

	//	apply 'magic' and modulate by velocity.
	unk01 = power[grenade_act];

	unk01 = unk01 * 0.7f;
	unk01 = unk01 + 0.3f;

	new_velocity = velocity * unk01;

	//	here's where the fun begins.
	math::get().angle_vectors_2(thrown, thrown_direction);

	eye_origin = g_pLocalPlayer->get_origin() + g_pLocalPlayer->get_view_offset();
	start = eye_origin + thrown_direction * 16.0f;
	thrown_direction = (thrown_direction * new_velocity) + g_pLocalPlayer->get_velocity();

	//	let's go ahead and predict
	for (auto time = 0.0f; index < 500; time += step) {
		pos = start + thrown_direction * step;

		//	setup trace
		trace_t trace;
		CTraceFilter filter;
		filter.pSkip = g_pLocalPlayer;

		Ray_t ray;
		ray.Init(start, pos);

		g_pTrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

		//	modify path if we have hit something.
		if (trace.fraction != 1.0f) {
			thrown_direction = trace.plane.normal * -2.0f * thrown_direction.Dot(trace.plane.normal) + thrown_direction;

			thrown_direction *= restitution;

			pos = start + thrown_direction * trace.fraction * step;

			time += (step * (1.0f - trace.fraction));
		}

		//	check for detonation.
		auto detonate = detonated(weapon, time, trace);

		//	emplace nade point.
		_points.at(index++) = c_nadepoint(start, pos, trace.fraction != 1.0f, true, trace.plane.normal, detonate);
		start = pos;

		//	apply gravity modifier.
		thrown_direction.z -= gravity * trace.fraction * step;

		if (detonate)
		{
			break;
		}
	}

	//	invalidate all empty points and finish prediction.
	for (auto n = index; n < 500; ++n)
	{
		_points.at(n).m_valid = false;
	}

	_predicted = true;
}

bool grenade_pred::detonated(C_BaseCombatWeapon* weapon, float time, trace_t& trace) {
	if (!weapon) {
		return true;
	}

	//	get weapon item index.
	const auto index = weapon->get_item_definiton_index();

	switch (index) {
		//	flash and HE grenades only live up to 2.5s after thrown.
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		if (time > 2.5f) {
			return true;
		}
		break;

		//	fire grenades detonate on ground hit, or 3.5s after thrown.
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		if (trace.fraction != 1.0f && trace.plane.normal.z > 0.7f || time > 3.5f) {
			return true;
		}
		break;

		//	decoy and smoke grenades were buggy in prediction, so i used this ghetto work-around.
	case WEAPON_DECOY:
	case WEAPON_SMOKEGRENADE:
		if (time > 2.5f)
		{
			return true;
		}
		break;
	}

	return false;
}

void grenade_pred::trace() {
	if (!vars.misc.grenade_trajectory.get<bool>())
		return;

	if (!(g_cmd->buttons & IN_ATTACK) && !(g_cmd->buttons & IN_ATTACK2))
	{
		_predicted = false;
		return;
	}

	const static std::vector< int > nades
	{
		WEAPON_FLASHBANG,
		WEAPON_SMOKEGRENADE,
		WEAPON_HEGRENADE,
		WEAPON_MOLOTOV,
		WEAPON_DECOY,
		WEAPON_INCGRENADE
	};

	//	grab local weapon.
	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	if (!weapon) {
		return;
	}

	if (std::find(nades.begin(), nades.end(), weapon->get_item_definiton_index()) != nades.end()) 
	{
		return predict();
	}

	_predicted = false;
}

void DrawBeamPaw(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS; //TE_BEAMPOINTS
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";//sprites/purplelaser1.vmt
	beamInfo.m_pszHaloName = "sprites/purplelaser1.vmt";//sprites/purplelaser1.vmt
	beamInfo.m_flHaloScale = 0;//0
	beamInfo.m_flWidth = 11;//11
	beamInfo.m_flEndWidth = 11;//11
	beamInfo.m_flFadeLength = 1.0f;
	beamInfo.m_flAmplitude = 2.3;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0.0;
	beamInfo.m_flFrameRate = 0.0;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;//40
	beamInfo.m_bRenderable = true;
	beamInfo.m_flLife = 0.03f;
	beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

	Beam_t* myBeam = g_pBeams->CreateBeamPoints(beamInfo);
	if (myBeam)
		g_pBeams->DrawBeam(myBeam);
}

void grenade_pred::draw() {
	if (!vars.misc.grenade_trajectory.get<bool>())
		return;

	if (!g_pEngine->IsInGame() || !g_pLocalPlayer || !g_pLocalPlayer->get_alive())
		return;

	auto draw_3d_dotted_circle = [](Vector position, float points, float radius) 
	{
		float step = M_PI * 2.0f / points;
		for (float a = 0; a < M_PI * 2.0f; a += step) {
			Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);

			Vector start2d;
			if (math::get().world_to_screen(start, start2d))
				render::get().line(Vector2D(start2d.x, start2d.y), Vector2D(start2d.x + 1, start2d.y + 1), Color(255, 255, 255, 255));
		}
	};

	Vector start, end;

	//	draw nade path.
	if (_predicted) 
	{
		for (auto& p : _points) 
		{
			if (!p.m_valid)
			{
				break;
			}
			if (math::get().world_to_screen(p.m_start, start) && math::get().world_to_screen(p.m_end, end)) {
				//	draw line.
				render::get().line(Vector2D(start.x, start.y), Vector2D(end.x + 1, end.y + 1), Color(0, 125, 255, 255));

				//	draw small box if detonated or hit a wall.
				if (p.m_detonate || p.m_plane)
				{
					render::get().rect(Vector2D(start.x - 2, start.y - 2), Vector2D(5, 5), p.m_detonate ? Color(255, 0, 0, 255) : Color(255, 255, 255, 255));
				}

				if (p.m_detonate)
					draw_3d_dotted_circle(p.m_end, 100, 150);
			}
		}
	}
}