#include "../include_cheat.h"

void aimbot::no_recoil()
{
	if ( !vars.aim.enabled.get<bool>() )
		return;

	_( weapon_recoil_scale_s, "weapon_recoil_scale" );
	static auto weapon_recoil_scale = g_pCVar->FindVar( weapon_recoil_scale_s );

	g_cmd->viewangles -= g_pLocalPlayer->get_aim_punch() * weapon_recoil_scale->get_float();
}

void aimbot::no_visual_recoil( CViewSetup& v_view )
{
	if ( !g_pLocalPlayer || !vars.aim.enabled.get<bool>() || g_pInput->m_fCameraInThirdPerson || !g_pLocalPlayer->get_alive() )
		return;

	v_view.angles -= g_pLocalPlayer->get_aim_punch() * 0.9f + g_pLocalPlayer->get_view_punch();
}

void aimbot::run()
{
	targets.clear();

	if ( !vars.aim.enabled.get<bool>() || ( *g_ppGameRules )->m_bFreezePeriod() )
		return;

	if ( !g_pLocalPlayer->get_alive() )
		return;

	if ( !can_shoot() )
		return;

	if (!g_tickbase.m_shift_data.m_did_shift_before && !g_tickbase.m_shift_data.m_should_be_ready)
		m_shoot_next_tick = false;

	select_targets();

	scan_targets();

	if (!m_shoot_next_tick && m_goal_shift == 13 && g_tickbase.m_shift_data.m_should_attempt_shift && !(g_tickbase.m_shift_data.m_prepare_recharge || g_tickbase.m_shift_data.m_did_shift_before && !g_tickbase.m_shift_data.m_should_be_ready))
	{
		m_shoot_next_tick = true;
	}
	if (m_shoot_next_tick)
	{
		g_cmd->tick_count = INT_MAX;
		g_tickbase.m_shift_data.m_ticks_to_shift = 13;
	}
}

void aimbot::select_targets()
{
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || player->IsDormant() || !player->is_enemy() || player == g_pLocalPlayer || player->get_immunity() )
			continue;

		targets.emplace_back( player, i );
	}
}

lag_record_t* aimbot::find_first_record(player_log_t& log) 
{
	lag_record_t* current;

	for (auto i = 1; i < log.record.size(); i++)
	{
		current = &log.record[i];
		if (current->valid) return current;
		else continue;
	}
	return nullptr;
}

lag_record_t* aimbot::find_last_record(player_log_t& log) 
{
	lag_record_t* current;

	for (int i = log.record.size() - 1; i >= 0; i--)
	{
		current = &log.record[i];
		if (current->valid) return current;
		else continue;
	}
	return nullptr;
}

void aimbot::scan_targets()
{
	auto best_hp = std::numeric_limits< float >::max();
	auto best_damage = 0.f;

	lag_record_t* best_record = nullptr;
	aimpoint_t best_point(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f), -1);
	C_CSPlayer* best_player = nullptr;

	player_log::get().filter_records(true);

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return;

	for (size_t i = 0; i < enabled_hitboxes.size(); i++) {
		switch (i)
		{
		case HITBOX_HEAD:
		case HITBOX_NECK:
		{
			if (GetAsyncKeyState(vars.key.baim.get<int>()) && vars.key.baim.get<bool>())
				enabled_hitboxes[i] = false;
			break;
		}
		case HITBOX_UPPER_CHEST:
		case HITBOX_RIGHT_HAND:
		case HITBOX_LEFT_HAND:
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_CALF:
		case HITBOX_RIGHT_CALF:
			enabled_hitboxes[i] = false;
			break;
		default:
			enabled_hitboxes[i] = true;
			break;
		}
	}

	if (vars.aim.hsonly.get<bool>())
	{
		for (size_t i = 0; i < enabled_hitboxes.size(); i++)
		{
			switch (i)
			{
			case HITBOX_HEAD:
				enabled_hitboxes[i] = true;
				break;
			default:
				enabled_hitboxes[i] = false;
				break;
			}
		}
	}

	for (auto& target : targets) {
		auto& log = player_log::get().get_log(target.index);
		if (!log.player)
			continue;

		std::deque<lag_record_t> flick_records;
		std::deque<lag_record_t> shot_records;

		// гетаем рекорды.
		auto first_record = find_first_record(log);
		auto last_record = find_last_record(log);

		for (auto& record : log.record) {
			// чекаем валидный ли наш рекорд.
			if (!record.valid)
				continue;

			// добавляем рекорды в которых петух фликает лбу.
			if (record.m_lby_flick)
				flick_records.push_front(record);

			// добавляем рекорды в которых петух фликает стреляет.
			if (record.m_shot)
				shot_records.push_front(record);
		}

		aimpoint_t flick_point(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f), -1);
		aimpoint_t shot_point(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f), -1);
		lag_record_t* flick_record = nullptr;
		lag_record_t* shot_record = nullptr;
		auto flick_damage = 0.f;
		auto shot_damage = 0.f;

		// я бы выделил скан в отдельную функцию, но мне лень - так что похуй.
		// сканим рекорды в которых петух стреляет.
		if (vars.aim.backtrack.get<bool>())
		{
			for (auto& record : flick_records)
			{
				float damage;
				int   hp;
				const aimpoint_t point = get_best_damage(record, damage);

				if (damage > best_damage)
				{
					// для хуесосов, чтобы стилить килы.
					hp = target.player->get_health();

					if (hp < best_hp) {
						best_hp = hp;
						flick_point = point;
						flick_record = &record;
						flick_damage = damage;
						best_player = target.player;
					}
				}
			}
			for (auto& record : shot_records)
			{
				float damage;
				int   hp;
				const aimpoint_t point = get_best_damage(record, damage);

				if (damage > best_damage)
				{
					// для хуесосов, чтобы стилить килы.
					hp = target.player->get_health();

					if (hp < best_hp) {
						best_hp = hp;
						shot_point = point;
						shot_record = &record;
						shot_damage = damage;
						best_player = target.player;
					}
				}
			}
		}

		aimpoint_t last_point(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f), -1);
		auto last_damage = 0.f;

		// сканим ласт рекорд.
		if (vars.aim.backtrack.get<bool>() && last_record && last_record != first_record) {
			float damage;
			int   hp;
			const aimpoint_t point = get_best_damage(*last_record, damage);

			last_point = point;
			last_damage = damage;

			if (damage > best_damage) {
				// для хуесосов, чтобы стилить килы.
				hp = target.player->get_health();

				if (hp < best_hp) {
					best_hp = hp;
					best_player = target.player;
				}
			}
		}

		aimpoint_t first_point(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f), -1);
		auto first_damage = 0.f;

		// сканим реальный рекорд.
		if (first_record) 
		{
			float damage, dist, fov, height;
			int   hp;
			const aimpoint_t point = get_best_damage(*first_record, damage);

			first_point = point;
			first_damage = damage;

			if (damage > best_damage)
			{
				// для хуесосов, чтобы стилить килы.
				hp = target.player->get_health();

				if (hp < best_hp) {
					best_hp = hp;
					best_player = target.player;
				}
			}
		}

		// сравниваем дамаг во всех рекордах хуилы которые сканили, и выбираем самый охуенный.
		if (best_player == target.player) 
		{
			if (flick_record && flick_point.point != Vector(0.f, 0.f, 0.f) && flick_point.hitbox != -1 && flick_damage > last_damage && flick_damage > first_damage && flick_damage > shot_damage)
			{
				best_damage = flick_damage;
				best_point = flick_point;
				best_record = flick_record;
			} // note - AkatsukiSun: 979$$$
			else if (shot_record && shot_point.point != Vector(0.f, 0.f, 0.f) && shot_point.hitbox != -1 && shot_damage > last_damage && shot_damage > first_damage)
			{
				best_damage = shot_damage;
				best_point = shot_point;
				best_record = shot_record;
			} // note - AkatsukiSun: Aimware v4$$$
			else 
			{
				if (first_record && first_point.point != Vector(0.f, 0.f, 0.f) && first_point.hitbox != -1 && first_damage > last_damage) 
				{
					best_damage = first_damage;
					best_point = first_point;
					best_record = first_record;
				} // note - AkatsukiSun: Supremacy$$$
				else if (last_record && last_point.point != Vector(0.f, 0.f, 0.f) && last_point.hitbox != -1) 
				{
					best_damage = last_damage;
					best_point = last_point;
					best_record = last_record;
				} // Note - AkatsukiSun: GameSense$$$
			}
		}
	}

	if (!best_record)
		return;

	const auto inair = !(g_pLocalPlayer->get_flags() & FL_ONGROUND);
	const auto aim_angles = math::get().calc_angle(/*prediction::get().get_unpred_eyepos()*/g_pLocalPlayer->get_shoot_pos(), best_point.point);// -(g_pLocalPlayer->get_aim_punch() * 2.f);
	const auto hc_angles = math::get().calc_angle(/*prediction::get().get_unpred_eyepos()*/g_pLocalPlayer->get_shoot_pos(), best_point.center); 
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;

	lag_record_t backup(best_player);
	best_record->apply(best_player);
	auto hitchance = calculate_hitchance(hc_angles, best_point, best_player);
	backup.apply(best_player, true);

	const auto do_autostop = [&]() -> int
	{
		if (!get_config(weapon)->autostop->get<int>())
			return 0;

		if (get_config(weapon)->autostop->get<int>() == 1 && (!get_config(weapon)->autostop_key->get<bool>() || GetAsyncKeyState(vars.key.autostop.get<int>())))
			return 1;

		if (get_config(weapon)->autostop->get<int>() == 2 && (!get_config(weapon)->autostop_key->get<bool>() || GetAsyncKeyState(vars.key.autostop.get<int>())))
			return 2;

		return 0;
	};

	if (!antiaim::get().get_fake_walk_state())
	{
			switch (do_autostop())
			{
			case 1:
				quick_stop();
				break;
			case 2:
				full_stop();
				break;
			}
	}

	if (!can_shoot())
		return;

	if (!hitchance && !is_knife && vars.aim.autofire.get<bool>() && !is_zeus)
	{
		if ((get_config(weapon)->index == 0 || get_config(weapon)->index == 1 || get_config(weapon)->index == 2) && get_config(weapon)->autoscope->get<int>())
		{
			if (!g_pLocalPlayer->get_scoped())
			{
				if (g_cmd->buttons & IN_ATTACK && do_autostop())
					g_cmd->buttons &= ~IN_ATTACK;

				g_cmd->buttons |= IN_ATTACK2;
			}
		}

		if (!do_autostop() || antiaim::get().get_fake_walk_state())
			return;

		if ((best_damage >= best_player->get_health() || !get_config(weapon)->autostop_lethal->get<bool>()) && !inair)
		{
			switch (do_autostop())
			{
			case 1:
				quick_stop();
				break;
			case 2:
				full_stop();
				break;
			}
		}

		return;
	}

	//if (vars.aa.prevent_onshot.get<bool>() && !g_pClientState->m_nChokedCommands && weapon->get_weapon_id() != WEAPON_REVOLVER)
	//	return;

	if (g_cmd->buttons & IN_ATTACK2 && !is_knife) // scope fix
		g_cmd->buttons &= ~IN_ATTACK;

	if ( weapon->get_weapon_id() == WEAPON_REVOLVER )
	{
		float ready_time = weapon->get_postpone_fire_ready_time();
		if (ready_time == FLT_MAX) return;

		float time_to_shoot = ready_time - prediction::get().get_curtime();// +ticks_to_time(antiaim::get().get_rest_choke());
		if (time_to_ticks(time_to_shoot) > 1)
			return;
	}

	if (vars.aim.autofire.get<bool>()) 
	{
		if (is_knife) 
		{
			if (is_knife && g_pLocalPlayer->get_shoot_pos().Dist(best_point.point) <= 48.f)
				g_cmd->buttons |= IN_ATTACK2;
			else
				g_cmd->buttons |= IN_ATTACK;
		}
		else
		{
			g_cmd->buttons |= IN_ATTACK;
			{
				if (vars.aa.prevent_onshot.get<bool>())
					*g_send_packet = false;

				g_cmd->viewangles = aim_angles;

				g_cmd->tick_count = lagcomp::get().fix_tickcount(best_record->m_sim_time);

				g_tick_shot = g_pGlobals->tickcount;

				resolver::get().add_shot(g_pLocalPlayer->get_shoot_pos(), best_record, best_player->EntIndex());

				if (vars.misc.impacts.get<bool>())
					g_pDebugOverlay->AddBoxOverlay(best_point.point, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 255, 0, 0, 127, 6);

				best_player->draw_hitboxes(*best_record, best_record->matrix, vars.aim.aimbot_record.get<D3DCOLOR>(), 6);
			}	
		}
	}

	aimbot_called = prediction::get().get_unpred_curtime();
}

bool aimbot::can_shoot( const float time, bool check )
{
	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( !weapon )
		return false;

	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;

	if ( !check )
	{
		if ( !vars.aim.knife_bot.get<bool>() && is_knife )
			return false;

		if ( !vars.aim.zeus_bot.get<bool>() && is_zeus )
			return false;
	}

	const auto weapontype = weapon->get_weapon_type();
	if ( weapontype == WEAPONTYPE_C4 || weapon->is_grenade() )
		return false;

	if ( weapon->get_clip1() < 1 && !is_knife )
		return false;

	if ( weapon->in_reload() )
		return false;

	if ( weapon->get_next_primary_attack() > time )
		return false;

	if ( g_pLocalPlayer->get_next_attack() > time )
		return false;

	if (weapon->get_weapon_id() == WEAPON_REVOLVER)
	{
		float ready_time = weapon->get_postpone_fire_ready_time() + ticks_to_time(antiaim::get().get_rest_choke());
		if (ready_time == FLT_MAX) return false;

		float time_to_shoot = ready_time - time;
		return time_to_ticks(time_to_shoot) < 1;
	}
	return true;
}

void aimbot::automatic_revolver()
{
	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon || weapon->get_item_definiton_index() != WEAPON_REVOLVER)
		return;

	static auto last_checked = 0;
	static auto last_spawn_time = 0.f;
	static auto tick_cocked = 0;
	static auto tick_strip = 0;

	const auto max_ticks = time_to_ticks(.25f) - 1;
	const auto tick_base = time_to_ticks(g_pGlobals->curtime);

	if (g_pLocalPlayer->get_spawn_time() != last_spawn_time) 
	{
		last_spawn_time = g_pLocalPlayer->get_spawn_time();
		tick_cocked = tick_base;
		tick_strip = tick_base - max_ticks - 1;
	}

	if (weapon->get_next_primary_attack() > g_pGlobals->curtime)// + ticks_to_time(antiaim::get().get_rest_choke()/*TESTING*/)) // One tick ahead of the game (L3D451R7)
	{
		g_cmd->buttons &= ~IN_ATTACK;
		g_revolver_fire = false;
		return;
	}

	if (last_checked == tick_base)
		return;

	last_checked = tick_base;
	g_revolver_fire = false;

	if (tick_base - tick_strip > 2 && tick_base - tick_strip < 14)
		g_revolver_fire = true;

	if (g_cmd->buttons & IN_ATTACK && g_revolver_fire)
		return;

	g_cmd->buttons |= IN_ATTACK;

	if (weapon->get_next_secondary_attack() >= g_pGlobals->curtime)// + ticks_to_time(antiaim::get().get_rest_choke()/*TESTING*/)) // One tick ahead of the game (L3D451R7)
		g_cmd->buttons |= IN_ATTACK2;

	if (tick_base - tick_cocked > max_ticks * 2 + 1) {
		tick_cocked = tick_base;
		tick_strip = tick_base - max_ticks - 1;
	}

	const auto cock_limit = tick_base - tick_cocked >= max_ticks;
	const auto after_strip = tick_base - tick_strip <= max_ticks;

	if (cock_limit || after_strip)
	{
		tick_cocked = tick_base;
		g_cmd->buttons &= ~IN_ATTACK;

		if (cock_limit)
			tick_strip = tick_base;
	}
}

void aimbot::quick_stop()
{
	if (!can_shoot)
		return;

	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return;

	quick_stop_called = prediction::get().get_unpred_curtime();
	g_cmd->viewangles.Clamp();

	Vector vel = prediction::get().get_unpred_vel();
	float speed = vel.Length2D();

	if (speed < 1.f) {
		g_cmd->forwardmove = 0.f;
		g_cmd->sidemove = 0.f;
		return;
	}

	__(sv_accelerate_s, "sv_accelerate");
	static auto sv_accelerate = g_pCVar->FindVar(sv_accelerate_s);
	float accel = sv_accelerate->get_float();
	float max_speed = weapon->get_wpn_data()->flMaxPlayerSpeed;
	if ((weapon->get_weapon_id() == WEAPON_SCAR20 || weapon->get_weapon_id() == WEAPON_G3SG1) && g_pLocalPlayer->get_scoped())
	{
		max_speed = weapon->get_wpn_data()->flMaxPlayerSpeedAlt;
	}

	// thx ida\\ // note - AkatsukiSun: thx nave <3.
	max_speed = std::min< float >(max_speed, 250.f);

	float surf_friction = 1.f;
	float max_accelspeed = accel * g_pGlobals->interval_per_tick * max_speed * surf_friction;

	float wishspeed{ };

	if (speed - max_accelspeed <= -1.f) {
		wishspeed = max_accelspeed / (speed / (accel * g_pGlobals->interval_per_tick));
	}
	else {
		wishspeed = max_accelspeed;
	}

	// Calculate the negative movement of our velocity, relative to our viewangles
	Vector ndir = (vel * -1.f);
	QAngle angle; angle = QAngle(ndir.x, ndir.y, ndir.z);
	math::get().vector_angles(ndir, angle);
	ndir.y = g_cmd->viewangles.y - angle.y; // Relative to local view
	math::get().angle_vectors(angle, &ndir);

	g_cmd->forwardmove = ndir.x * wishspeed;
	g_cmd->sidemove = ndir.y * wishspeed;
}
void aimbot::full_stop()
{
	if (!can_shoot)
		return;

	const auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;

	C_BaseCombatWeapon* weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return;

	quick_stop_called = prediction::get().get_unpred_curtime();
	g_cmd->viewangles.Clamp();

	float flMaxSpeed = g_pLocalPlayer->get_scoped() ? weapon->get_wpn_data()->flMaxPlayerSpeedAlt : weapon->get_wpn_data()->flMaxPlayerSpeed;
	float flDesiredSpeed = flMaxSpeed * .34f;

	if (animstate->m_flSpeedNormalized <= flDesiredSpeed)
	{
		g_cmd->forwardmove = 0.f;
		g_cmd->sidemove = 0.f;
	}
}

bool aimbot::get_quick_stop_state() const
{
	return quick_stop_called == prediction::get().get_unpred_curtime();
}

aimpoint_t aimbot::get_best_damage(lag_record_t& log, float& damage, Vector* pos)
{
	const auto player = get_entity(log.index);
	const auto& player_log = player_log::get().get_log(log.index);
	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	lag_record_t backup(player);
	log.apply(player);

	std::vector<aimpoint_t> points;
	const auto fakelag_hitscan = [&]()
	{
		const auto model = player->get_model();
		if (!model)
			return;

		const auto studio_hdr = g_pModelInfo->GetStudioModel(model);
		if (!studio_hdr)
			return;

		for (auto i = 0; i < HITBOX_MAX; i++)
		{
			if (!(i == HITBOX_HEAD || i == HITBOX_PELVIS || i == HITBOX_CHEST))
				continue;

			auto hitbox = studio_hdr->pHitbox(i, 0);
			if (!hitbox)
				continue;

			auto& mat = log.matrix[hitbox->bone];
			const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;

			Vector max;
			Vector min;
			math::get().vector_transform(hitbox->bbmax + mod, mat, max);
			math::get().vector_transform(hitbox->bbmin - mod, mat, min);

			const auto center = (min + max) * 0.5f;

			const auto cur_angles = math::get().calc_angle(center, /*prediction::get().get_unpred_eyepos()*/g_pLocalPlayer->get_shoot_pos());

			Vector forward;
			math::get().angle_vectors(cur_angles, &forward);

			const auto right = forward.Cross(Vector(0, 0, 1));
			const auto left = Vector(-right.x, -right.y, right.z);
			const auto top = Vector(0, 0, 1);

			if (i == HITBOX_HEAD)
				points.emplace_back(center + top * hitbox->radius, center, i);
			points.emplace_back(center + right * hitbox->radius, center, i);
			points.emplace_back(center + left * hitbox->radius, center, i);
		}
	};

	if (pos)
		fakelag_hitscan();
	else
		multipoint_hitboxes(log, points);

	auto was_repeated = false;

	auto best_damage = 0.f;
	auto prevhitbox = -1;
	aimpoint_t best_point(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f), -1);

	if (!weapon)
		return best_point;

	const auto info = weapon->get_wpn_data();

	if (!info)
		return best_point;

	const auto player_weapon = get_weapon(player->get_active_weapon());
	if (!player_weapon)
		return best_point;

	const auto player_info = player_weapon->get_wpn_data();

	if (!player_info)
		return best_point;

	const auto player_animstate = player->get_anim_state();
	if (!player_animstate)
		return best_point;

	const auto is_zeus = get_weapon(g_pLocalPlayer->get_active_weapon())->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && get_weapon(g_pLocalPlayer->get_active_weapon())->get_weapon_type() == WEAPONTYPE_KNIFE;
	const auto baim_when = !(log.m_flags & FL_ONGROUND) && get_config(weapon)->baim.air->get<bool>()
		|| (player_log.m_iMode != RMODE_WALL && player_log.m_iMode != RMODE_DELTA) && get_config(weapon)->baim.resolver_unsure->get<bool>()
		|| best_damage && get_config(weapon)->baim_mode->get<int>() == 1 && player_log.m_nSpreadShots >= 1
		|| get_config(weapon)->baim.lethal->get<bool>() && best_damage >= player->get_health() + 4;
repeat:

	for (auto& point : points)
	{
		// нахуй нам сканить поинты, по которым мы не сможем стрелять?
		if ((g_pLocalPlayer->get_shoot_pos() - point.point).Length() > info->flRange)
			continue;

		// охуенный адаптив аимбот, чтобы умешьшить вероятность миссов до минимума.
		if (!is_knife && !is_zeus && get_config(weapon)->adaptive_scale->get<bool>()) 
		{
			 if (point.hitbox >= HITBOX_RIGHT_THIGH && point.hitbox <= HITBOX_LEFT_FOREARM)
			 {
				// если дуралей фейкволкает - не сканим конечности.
				if (log.m_fake_walk)
					continue;
			}
		}

		const auto dist = pos ? pos->Dist(point.point) : g_pLocalPlayer->get_shoot_pos().Dist(point.point);

		//g_pDebugOverlay->AddBoxOverlay( point.point, Vector( 1.f, 1.f, 1.f ), Vector( -1.f, -1.f, -1.f ), Vector( 0.f, 0.f, 0.f ), random_float( 0.f, 255.f ), random_float( 0.f, 255.f ), random_float( 0.f, 255.f ), 255, g_pGlobals->interval_per_tick );

		if (is_knife && dist > 64.0f)
			continue;

		float point_damage;

		if (!is_knife && !penetration::get().can_hit(point.point, point_damage, g_pLocalPlayer->get_shoot_pos() /*TESTING  prediction::get().get_unpred_eyepos()*/))
			continue;

		if (!is_knife && point_damage <= /*GetAsyncKeyState(vars.aim.override_damage.get<int>()) ? get_config(weapon)->ov_mindamage->get<float>() : */get_config(weapon)->mindmg->get<float>() && point_damage <= player->get_health() + 1)
			continue;

		if (is_zeus && point_damage < player->get_health() + 4 && dist > 180.f)
			continue;

		if (is_knife)
			point_damage = 100.f / dist;

		if ((point_damage > best_damage && prevhitbox != point.hitbox || point_damage > best_damage + 30.f) || baim_when)
		{
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
	}

	if (!was_repeated && !baim_when)
	{
		points.clear();
		multipoint_hitboxes(log, points, true);
		was_repeated = true;
		goto repeat;
	}

	backup.apply(player, true);

	damage = best_damage;
	return best_point;
};

void aimbot::multipoint_hitboxes(lag_record_t& log, std::vector<aimpoint_t>& points, const bool second_go)
{
	const auto player = get_entity(log.index);

	auto& player_log = player_log::get().get_log(log.index);

	const auto model = player->GetModel();
	if (!model)
		return;

	const auto studio_hdr = g_pModelInfo->GetStudioModel(model);
	if (!studio_hdr)
		return;

	mstudiohitboxset_t* set = studio_hdr->pHitboxSet(player->get_hitbox_set());
	if (!set)
		return;

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;
	const auto moving = log.m_velocity.Length() > 0.1f;

	for (auto i = 0; i < HITBOX_MAX; i++) 
	{
		if (is_knife && i != HITBOX_UPPER_CHEST)
			continue;

		if (!is_knife && !is_zeus && !enabled_hitboxes[i])
			continue;

		auto hitbox = studio_hdr->pHitbox(i, 0);
		if (!hitbox)
			continue;

		if (second_go && i > HITBOX_NECK)
			continue;

		if (!second_go && i <= HITBOX_NECK)
			continue;

		// щитаем скейлы.
		auto scale = get_config(weapon)->scale->get<float>();
		if (i == HITBOX_HEAD)
			scale = get_config(weapon)->scale_head->get<float>();

		scale = scale / 100.f;

		auto& mat = log.matrix[hitbox->bone];
		const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;
		Vector min, max;
		math::vector_transform(hitbox->bbmax + mod, mat, max);
		math::vector_transform(hitbox->bbmin - mod, mat, min);

		const auto center = (min + max) * 0.5f;
		const auto cur_angles = math::get().calc_angle(center, g_pLocalPlayer->get_shoot_pos());

		Vector forward;
		math::get().angle_vectors(cur_angles, &forward);

		auto rs = hitbox->radius * scale;

		// для ножа а чо.
		if (is_knife || is_zeus)
		{
			const auto back = forward * hitbox->radius;
			points.emplace_back(center + back, center, i);
			continue;
		}

		// если поц летит - то он прилетит ток на залупу, так что нахуй мульты.
		if (!(log.m_flags & FL_ONGROUND))
			rs = .65f;

		// на эти хитбоксы мульты нахуй не нужны.
		if (i >= HITBOX_RIGHT_THIGH && i <= HITBOX_LEFT_FOREARM)
			continue;

		// миссаем по сприду - умешьшаем скейлы.
		if (player_log.m_nShots < 4)
			rs *= 0.95f - (std::min(player_log.m_nShots, 3) * 0.25);

		if (log.m_lby_flick)
			rs *= 0.65f;

		// какой смысл блять от скейлов < 0.05 если оно ничего кроме нагрузки пк сканами кучи безполезных точек не даст?
		if (rs < .2f)
			return;

		const auto right = forward.Cross(Vector(0, 0, 1)) * rs;
		const auto left = Vector(-right.x, -right.y, right.z);
		const auto top = Vector(0, 0, 1) * rs;

		if(i == HITBOX_HEAD)
			points.emplace_back(center + top, center, i);
		points.emplace_back(center + right, center, i);
		points.emplace_back(center + left, center, i);
	}
}

//bool CanHitHitbox(const Vector start, const Vector end, matrix3x4_t* matrix, C_CSPlayer* player, uint32_t box)
//{
//	if (!player)
//		return;
//
//	const model_t* model = player->get_model();
//
//	if (!model)
//		return;
//
//	const auto hdr = g_pModelInfo->GetStudioModel(model);
//
//	if (!hdr)
//		return;
//
//	const auto studio_box = hdr->pHitboxSet(0);
//
//	if (!studio_box)
//		return false;
//
//	const auto hitbox = studio_box->pHitbox(box);
//
//	Vector min, max;
//
//	const auto is_capsule = hitbox->radius != -1.f;
//
//	if (is_capsule)
//	{
//		math::vector_transform(hitbox->bbmin, matrix[hitbox->bone], min);
//		math::vector_transform(hitbox->bbmax, matrix[hitbox->bone], max);
//		const auto dist = math::segment_to_segment(start, end, min, max);
//
//		if (dist < hitbox->radius)
//			return true;
//	}
//
//	if (!is_capsule)
//	{
//		math::vector_transform(math::get().vector_rotate(hitbox->bbmin, hitbox->rotation), matrix[hitbox->bone], min);
//		math::vector_transform(math::get().vector_rotate(hitbox->bbmax, hitbox->rotation), matrix[hitbox->bone], max);
//
//		math::vector_i_transform(start, matrix[hitbox->bone], min);
//		math::vector_i_rotate(end, matrix[hitbox->bone], max);
//
//		if (math::intersect_line_with_bb(min, max, hitbox->bbmin, hitbox->bbmax))
//			return true;
//	}
//
//	return false;
//}

static std::vector<std::tuple<float, float, float>> precomputed_seeds = {};

typedef void(*RandomSeed_t)(UINT);
RandomSeed_t m_RandomSeed = 0;
void Rand_Seed(uint32_t seed) {
	if (m_RandomSeed == nullptr)
		m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed");
	m_RandomSeed(seed);
}

typedef float(*RandomFloat_t)(float, float);
RandomFloat_t m_RandomFloat;
float Rand_Float(float flLow, float flHigh)
{
	if (m_RandomFloat == nullptr)
		m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat");

	return m_RandomFloat(flLow, flHigh);
}

static const int total_seeds = 255;

void build_seed_table()
{
	if (!precomputed_seeds.empty())
		return;

	for (auto i = 0; i < total_seeds; i++) 
	{
		Rand_Seed(i + 1);

		const auto pi_seed = Rand_Float(0.f, PI * 2);

		precomputed_seeds.emplace_back(Rand_Float(0.f, 1.f),
			sin(pi_seed), cos(pi_seed));
	}
}

bool aimbot::calculate_hitchance(QAngle vangles, const aimpoint_t& point, C_CSPlayer* player)
{
	build_seed_table();

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	if (!weapon)
		return false;

	const auto info = weapon->get_wpn_data();

	if (!info)
		return false;

	const auto studio_model = g_pModelInfo->GetStudioModel(player->GetModel());

	if (!studio_model)
		return false;

	__(weapon_accuracy_nospread, "weapon_accuracy_nospread");
	static auto nospread = g_pCVar->FindVar(weapon_accuracy_nospread);
	if (nospread->get_bool())
		return true;

	// setup calculation parameters.
	const auto id = weapon->get_item_definiton_index();
	auto chance = id == WEAPON_TASER ? vars.aim.zeus_hc.get<float>() / 100.f : get_config(weapon)->hitchance->get<float>() / 100.f;
	const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };
	weapon->update_accuracy();
	// calculate inaccuracy.
	const auto weapon_inaccuracy = weapon->get_inaccuracy();

	if (int(weapon_inaccuracy * 1000.f) == 0)
		return true;

	const auto weap_spread = weapon->get_spread();

	// calculate start and angle.
	auto start = g_pLocalPlayer->get_shoot_pos();
	Vector forward, right, up;
	const auto angles = vangles.Clamp();
	math::get().angle_vectors(angles, &forward, &right, &up);
	auto endpoint = point.center;

	// keep track of all traces that hit the enemy.
	auto current = 0;

	// setup calculation parameters.
	Vector total_spread, spread_angle, end;
	float inaccuracy, spread_x, spread_y;
	std::tuple<float, float, float>* seed;

	for (auto i = 0u; i < total_seeds; i++)
	{
		// get seed.
		seed = &precomputed_seeds[i];

		// calculate spread.
		inaccuracy = std::get<0>(*seed) * weapon_inaccuracy;
		spread_x = std::get<2>(*seed) * inaccuracy;
		spread_y = std::get<1>(*seed) * inaccuracy;
		total_spread = (forward + right * spread_x + up * spread_y).NormalizeInPlace();

		// calculate angle with spread applied.
		math::get().vector_angles_2(total_spread, spread_angle);

		// calculate end point of trace.
		math::get().angle_vectors(spread_angle, &end);
		end = start + end.NormalizeInPlace() * endpoint.Dist(start);

		// did we hit the hitbox?
		trace_t tr;
		Ray_t ray;
		ray.Init(start, end);
		g_pTrace->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr);

		if (tr.m_pEnt == player)
			current++;

		// abort if hitchance is already sufficent.
		if ((float)current / (float)total_seeds >= chance)
			return true;

		// abort if we can no longer reach hitchance.
		if ((float)(current + total_seeds - i) / (float)(total_seeds) < chance)
			return false;
	}
	return (float)current / (float)total_seeds >= chance;
}