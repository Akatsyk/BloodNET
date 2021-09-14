#include "../include_cheat.h"

void antiaim::run()
{
	if ( !g_pLocalPlayer->get_alive() || ( *g_ppGameRules )->m_bFreezePeriod() )
	{
		freestanding::get().last_fs_time = 0.f;
		return;
	}

	if ( !vars.aa.enabled.get<bool>() && !vars.aa.fakelag_when_disabled.get<bool>() )
		return;

	if ( !vars.aa.enabled.get<bool>() )
		fake_lag();

	if ( !vars.aa.enabled.get<bool>() )
		return;

	fake_walk();
	update_animstate();
	choose_fk();
	choose_real();


	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( !weapon )
		return;

	const auto revert_aa = []()
	{
		QAngle viewangles;
		g_pEngine->GetViewAngles( viewangles );
		g_cmd->viewangles.x = viewangles.x;
		g_cmd->viewangles.y = viewangles.y;
	};

	const auto movetype = g_pLocalPlayer->get_move_type();

	if ( g_cmd->buttons & IN_ATTACK && !weapon->is_grenade() && aimbot::can_shoot() && weapon->get_weapon_id() != WEAPON_REVOLVER || movetype == MOVETYPE_LADDER || movetype == MOVETYPE_NOCLIP || g_cmd->buttons & IN_USE )
		revert_aa();


	if ( weapon->is_being_thrown() )
	{
		revert_aa();
		*g_send_packet = true;
	}
}

void antiaim::fake_lag()
{
	static auto unchoke = false;
	static auto onpeek_called = 0.f;

	to_choke = get_antiaim( type )->choke->get<int>();

	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );

	if ( weapon )
	{
		if ( vars.aa.choke_reload.get<bool>() && weapon->in_reload() )
			to_choke = 13;

		else if ( vars.aa.choke_weapon_switch.get<bool>() && g_pLocalPlayer->get_next_attack() > prediction::get_curtime() )
			to_choke = 13;

		else if ( vars.aa.choke_shot.get<bool>() && g_cmd->buttons & IN_ATTACK )
			to_choke = 13;
	}

	if ( onpeek_called >= g_pGlobals->curtime && onpeek_called < g_pGlobals->curtime + ticks_to_time( 15 ) )
		to_choke = 13;

	if ( get_fake_walk_state() )
		to_choke = 13;

	//if (vars.aa.fakeduck.get<bool>() && GetAsyncKeyState(vars.key.fakeduck.get<int>()))
	//	to_choke = 14;

	to_choke = std::clamp( to_choke, 1u, 15u );
	if ( ( *g_ppGameRules )->m_bIsValveDS() )
		to_choke = std::clamp( to_choke, 1u, 6u );

	*g_send_packet = g_pClientState->m_nChokedCommands >= to_choke;

	if ( get_fake_walk_state() )
		return;

	if ( unchoke )
	{
		*g_send_packet = true;
		unchoke = false;
		return;
	}

	if ( *g_send_packet )
		onpeek_called = 0.f;

	if ( on_peek_fakelag() )
	{
		if ( *g_send_packet )
			return;

		if ( onpeek_called < g_pGlobals->curtime )
			onpeek_called = g_pGlobals->curtime + ticks_to_time( 14 );
		else if ( g_pClientState->m_nChokedCommands > 1 && onpeek_called < g_pGlobals->curtime )
			unchoke = true;
	}
}

void antiaim::choose_fk()
{
	fake_lag();

	if (*g_send_packet)
	{
		switch (vars.aa.fake_angles.get<int>())
		{
		case 0:
			g_cmd->viewangles.y += misc::get().g_real_angles.y + fmodf(g_pGlobals->tickcount * 10.f, 360.f);
			break;
		case 1:
			g_cmd->viewangles.y += misc::get().g_real_angles.y + RandomFlt(130.f, 260.f);
			break;
		case 2:
			g_cmd->viewangles.y += misc::get().g_real_angles.y;
			break;
		case 3:
			g_cmd->viewangles.y += g_pLocalPlayer->get_lby() - 180.f;
			break;
		case 4:
			g_cmd->viewangles.y += RandomFlt(0.f, 360.f);
			break;
		case 5:
			g_cmd->viewangles.y += 180.f;
			break;
		}
	}
}

void antiaim::update_animstate() 
{
	bool moving = g_pLocalPlayer->get_anim_state()->m_velocity >= 0.1f && g_pLocalPlayer->get_anim_state()->m_bOnGround;
	float time = prediction::get().get_curtime();

	if (moving)
		next_update = time + 0.22f;
	else if (time > next_update) 
		next_update = time + 1.1f;
}

float antiaim::get_next_update() const 
{
	float current_tick = time_to_ticks(prediction::get().get_curtime());
	float update_tick = time_to_ticks(next_update);

	return update_tick - current_tick;
}

bool antiaim::lby_update()
{
	if ( g_pClientState->m_nChokedCommands || !( g_pLocalPlayer->get_flags() & FL_ONGROUND ) )
		return false;

	const auto updated = update_lby;

	if ( update_lby )
	{
		if ( get_antiaim( type )->lby_random->get<bool>() )
		{
			const auto lby_random_range = get_antiaim( type )->lby_random_range->get<int>() * 0.5f;
			const auto lby_random_add = get_antiaim( type )->lby_random_add->get<int>();
			const auto lby = random_float( -lby_random_range, lby_random_range );
			initial_lby = g_cmd->viewangles.y + lby + lby_random_add;
		}

		auto angles = g_cmd->viewangles.y;

		target_lby = initial_lby;
		g_cmd->viewangles.y = initial_lby;
		g_cmd->viewangles.Clamp();
		update_lby = false;

		if (secondupdate || !get_antiaim(type)->lby_static->get<bool>())
		{
			if (get_antiaim(type)->lby_static->get<bool>())
			{
				initial_lby += -get_antiaim(type)->lby_delta_first->get<float>() + get_antiaim(type)->lby_delta->get<float>();
			}
			else
			{
				initial_lby = angles + get_antiaim(type)->lby_delta->get<float>();
			}
			secondupdate = false;
		}
	}

	return updated;
}

void antiaim::lby_prediction()
{
	const auto animstate = *reinterpret_cast< CCSGOPlayerAnimState** >( uintptr_t( static_cast< C_CSPlayer* >( g_pLocalPlayer ) ) + 0x3874 );
	if ( !animstate )
		return;

	if ( g_pClientState->m_nChokedCommands )
		return;

	if (animstate->m_velocity > 0.1f)
	{
		next_lby_update = prediction::get().get_curtime() + 0.22f;
		firstupdate = true;
	}
	else if ( prediction::get().get_curtime() > next_lby_update )
	{
		update_lby = true;
		next_lby_update = prediction::get().get_curtime() + 1.1f;
	}

	const auto get_add_by_choke = [&]() -> float
	{
		static auto max = 137.f;
		static auto min = 100.f;

		auto mult = 1.f / 0.2f * ticks_to_time(to_choke);

		return 100.f + (max - min) * mult;
	};

	if ( firstupdate && animstate->m_velocity <= 0.1f )
	{
		initial_lby = g_cmd->viewangles.y + get_antiaim( type )->lby_delta_first->get<float>();
		secondupdate = true;
		firstupdate = false;
	}

	if ( !firstupdate && prediction::get().get_curtime() + ticks_to_time( to_choke + 1 ) > next_lby_update
		 &&  fabsf( math::get().normalize_float( g_cmd->viewangles.y - initial_lby ) ) < get_add_by_choke() )
	{
		g_cmd->viewangles.y = initial_lby + get_add_by_choke();
	}


}

// thanks aimware.
void antiaim::fix_movement()
{
	Vector view_fwd, view_right, view_up, g_cmd_fwd, g_cmd_right, g_cmd_up;
	math::get().angle_vectors(autostrafer::get().m_strafe_angles, &view_fwd, &view_right, &view_up);
	math::get().angle_vectors(g_cmd->viewangles, &g_cmd_fwd, &g_cmd_right, &g_cmd_up);

	const auto v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const auto v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const auto v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const auto v14 = sqrtf((g_cmd_fwd.x * g_cmd_fwd.x) + (g_cmd_fwd.y * g_cmd_fwd.y));
	const auto v16 = sqrtf((g_cmd_right.x * g_cmd_right.x) + (g_cmd_right.y * g_cmd_right.y));
	const auto v18 = sqrtf(g_cmd_up.z * g_cmd_up.z);

	const Vector norm_g_cmd_fwd((1.f / v14) * g_cmd_fwd.x, (1.f / v14) * g_cmd_fwd.y, 0.f);
	const Vector norm_g_cmd_right((1.f / v16) * g_cmd_right.x, (1.f / v16) * g_cmd_right.y, 0.f);
	const Vector norm_g_cmd_up(0.f, 0.f, (1.f / v18) * g_cmd_up.z);

	const auto v22 = norm_view_fwd.x * g_cmd->forwardmove;
	const auto v26 = norm_view_fwd.y * g_cmd->forwardmove;
	const auto v28 = norm_view_fwd.z * g_cmd->forwardmove;
	const auto v24 = norm_view_right.x * g_cmd->sidemove;
	const auto v23 = norm_view_right.y * g_cmd->sidemove;
	const auto v25 = norm_view_right.z * g_cmd->sidemove;
	const auto v30 = norm_view_up.x * g_cmd->upmove;
	const auto v27 = norm_view_up.z * g_cmd->upmove;
	const auto v29 = norm_view_up.y * g_cmd->upmove;

	g_cmd->forwardmove = ((((norm_g_cmd_fwd.x * v24) + (norm_g_cmd_fwd.y * v23)) + (norm_g_cmd_fwd.z * v25))
		+ (((norm_g_cmd_fwd.x * v22) + (norm_g_cmd_fwd.y * v26)) + (norm_g_cmd_fwd.z * v28)))
		+ (((norm_g_cmd_fwd.y * v30) + (norm_g_cmd_fwd.x * v29)) + (norm_g_cmd_fwd.z * v27));
	g_cmd->sidemove = ((((norm_g_cmd_right.x * v24) + (norm_g_cmd_right.y * v23)) + (norm_g_cmd_right.z * v25))
		+ (((norm_g_cmd_right.x * v22) + (norm_g_cmd_right.y * v26)) + (norm_g_cmd_right.z * v28)))
		+ (((norm_g_cmd_right.x * v29) + (norm_g_cmd_right.y * v30)) + (norm_g_cmd_right.z * v27));
	g_cmd->upmove = ((((norm_g_cmd_up.x * v23) + (norm_g_cmd_up.y * v24)) + (norm_g_cmd_up.z * v25))
		+ (((norm_g_cmd_up.x * v26) + (norm_g_cmd_up.y * v22)) + (norm_g_cmd_up.z * v28)))
		+ (((norm_g_cmd_up.x * v30) + (norm_g_cmd_up.y * v29)) + (norm_g_cmd_up.z * v27));

	autostrafer::get().m_strafe_angles = g_cmd->viewangles;

	if (g_pLocalPlayer->get_move_type() != MOVETYPE_LADDER)
		g_cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
}

void antiaim::fake_walk()
{
	if (!vars.aa.fakewalk.get<bool>() || !GetAsyncKeyState(vars.key.fakewalk.get<int>()))
		return;

	auto velocity = prediction::get().get_unpred_vel();

	static auto predict_velocity = [](Vector* velocity)
	{
		_(sv_friction_s, "sv_friction");
		_(sv_stopspeed_s, "sv_stopspeed");
		static auto sv_friction = g_pCVar->FindVar(sv_friction_s);
		static auto sv_stopspeed = g_pCVar->FindVar(sv_stopspeed_s);

		float speed = velocity->Length();
		if (speed >= 0.1f)
		{
			float friction = sv_friction->get_float();
			float stop_speed = std::max< float >(speed, sv_stopspeed->get_float());
			float time = std::max< float >(g_pGlobals->interval_per_tick, g_pGlobals->frametime);
			*velocity *= std::max< float >(0.f, speed - friction * stop_speed * time / speed);
		}
	};

	static auto moneybot_stop = []()
	{
		auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
		if (!weapon)
			return;

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
		if ((weapon->get_item_definiton_index() == WEAPON_SCAR20 || weapon->get_item_definiton_index() == WEAPON_G3SG1) &&
			g_pLocalPlayer->get_scoped())
		{
			max_speed = weapon->get_wpn_data()->flMaxPlayerSpeedAlt;
		}

		if (g_pLocalPlayer->get_flags() & FL_DUCKING)
		{
			max_speed /= 3.f;
			accel /= 3.f;
		}

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
		Vector ndir = (vel * -1.f); math::get().vector_angles_2(ndir, ndir);
		ndir.y = g_cmd->viewangles.y - ndir.y; // Relative to local view
		math::get().angle_vectors(ndir, &ndir);

		g_cmd->forwardmove = ndir.x * wishspeed;
		g_cmd->sidemove = ndir.y * wishspeed;
	};

	static auto quick_stop = []()
	{
		Vector vel = prediction::get().get_unpred_vel();
		float speed = vel.Length2D();

		if (prediction::get().get_unpred_vel().Length2D() > 13.f)
		{
			Vector direction; math::get().vector_angles_2({ 0.f, 0.f, 0.f }, vel); math::get().vector_angles_2(vel, direction);
			direction.y = g_cmd->viewangles.y - direction.y;

			Vector new_move; math::get().angle_vectors(direction, &new_move);
			new_move *= -450.f;

			g_cmd->forwardmove = new_move.x;
			g_cmd->sidemove = new_move.y;
		}
		else
		{
			g_cmd->forwardmove = 0.f;
			g_cmd->sidemove = 0.f;
		}
	};

	int ticks_to_update = get_next_update() - 1;
	int ticks_to_stop = 0;
	for (;;)
	{
		if (velocity.Length2D() < 0.1f)
			break;
		predict_velocity(&velocity);
		ticks_to_stop++;
	}

	const int max_ticks = min< int >(14, ticks_to_update);
	const int choked = g_pClientState->m_nChokedCommands;
	int ticks_left = max_ticks - choked;

	if (choked < max_ticks || ticks_to_stop)
		fake_walk_called = prediction::get().get_unpred_curtime();
	//*g_send_packet = false;

	if (ticks_to_stop > ticks_left - 1 || !choked)
		//aimbot::get().quick_stop();
		//quick_stop();
		moneybot_stop();
}

void antiaim::fakeduck()
{
	if (!GetAsyncKeyState(vars.key.fakeduck.get<int>()) || !vars.aa.fakeduck.get<bool>())
		return;
	
	static int old_cmds = 0;
	if (old_cmds!= g_cmd->command_number)
		old_cmds = g_cmd->command_number;

	if (*g_send_packet)
	{
		g_cmd->tick_count = INT_MAX / std::min((int)vars.aa.fakemove_ticks.get<int>(), 14);
		g_cmd->command_number = old_cmds;
	}
	if (!*g_send_packet)
	{
		g_cmd->tick_count = old_cmds % 0;
		g_cmd->command_number = INT_MAX / std::min((int)vars.aa.fakemove_ticks.get<int>(), 14);
	}
}

bool antiaim::get_fake_walk_state() const
{
	return fake_walk_called == prediction::get().get_unpred_curtime();
}

bool antiaim::on_peek_fakelag()
{
	if ( !vars.aa.on_peek.get<bool>() )
		return false;

	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( weapon && weapon->get_item_definiton_index() == WEAPON_REVOLVER )
		return false;

	if ( g_pLocalPlayer->get_velocity().Length() < 2.f )
		return false;

	const auto time = prediction::get_curtime() + ticks_to_time( 3 );
	if ( !aimbot::get().can_shoot( time ) )
		return false;

	const auto mindmg = get_config( get_weapon( g_pLocalPlayer->get_active_weapon() ) )->mindmg->get<float>();

	auto newpos = g_pLocalPlayer->get_origin() + g_pLocalPlayer->get_velocity() * g_pGlobals->interval_per_tick * 3; //3 == prediction amount
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || player->IsDormant() || !player->is_enemy() || player == g_pLocalPlayer || player->get_immunity() )
			continue;

		auto& log = player_log::get().get_log( i );
		if ( !log.m_bFilled || log.record.empty() )
			continue;

		auto player_damage = 0.f;
		aimbot::get().get_best_damage( log.record[ 0 ], player_damage, &newpos );

		if ( player_damage > mindmg )
			return true;
	}

	return false;
}

void antiaim::choose_real()
{
		
	const auto air = vars.aa.air.enabled.get<bool>() && !( g_pLocalPlayer->get_flags() & FL_ONGROUND );
	const auto moving = vars.aa.moving.enabled.get<bool>() && g_pLocalPlayer->get_flags() & FL_ONGROUND && prediction::get().get_unpred_vel().Length() > 0.1f;
	const auto fakewalking = vars.aa.fakewalk.get<bool>() && GetAsyncKeyState( vars.key.fakewalk.get<int>() );
	const auto regular = ( !air && !moving ) || ( fakewalking && !air );

	if ( air ) { type = 2; }
	if ( moving ) { type = 1; }
	if ( regular ) { type = 0; }

	if (get_antiaim(type)->at_target->get<bool>())
		do_at_target();

	do_add();

	if (!get_antiaim(type)->edge->get<bool>())
		do_freestand();

	if (get_antiaim(type)->spin->get<bool>())
		do_spin();

	if (get_antiaim(type)->jitter->get<bool>())
		do_jitter();

	if (get_antiaim(type)->lby->get<bool>() && !fakewalking) // TESTING
		do_lby();

	g_cmd->viewangles.x = 89.f;
}

void antiaim::do_spin() const
{
	// note - AkatsukiSun: opai gave me that, so I <3 him.
	// if ( aaVar->distortion && real && g_bStand )
	//{
	//	v22 = (aaVar->distortionSpeed / 100.0) * 0.0625;
	//	v13 = *libm_sse2_pow_precise(v17, v18).m128_u64;
	//	switch_1 = ::switchSide;
	//	distortionAngle = ((1.0 - v13) * aaVar->distortionRange) - (aaVar->distortionRange * 0.5);
	//	if (::switchSide)
	//		cmd->viewangles.y = distortionAngle + cmd->viewangles.y;
	//	else
	//		cmd->viewangles.y = cmd->viewangles.y - distortionAngle;
	//	distortionTimer = distortionTimer + v22;
	//	if (distortionTimer >= 0.69999999)
	//	{
	//		distortionTimer = 0.0;
	//		::switchSide = switch_1 == 0;
	//	}
	//}
	
	// note - AkatsukiSun: when i started to reverse this shit, Kamaz released r2p code.
	float speed = (get_antiaim(type)->spin_speed->get<float>() * 0.01f) * 0.0625f;
	float distortion_angle = get_antiaim(type)->spin_range->get<float>() * 
							(1.f - std::powf(1.f - antiaim::get().timer_distortion, 2)) - 
								(get_antiaim(type)->spin_range->get<float>() * 0.5f);

	g_cmd->viewangles.y += antiaim::get().switch_distortion ? distortion_angle : -distortion_angle;

	// update timer and go back when we at the end if distortion flip
	antiaim::get().timer_distortion += speed;
	if (antiaim::get().timer_distortion >= 0.7f)
	{
		antiaim::get().timer_distortion = 0.f;
		antiaim::get().switch_distortion = !antiaim::get().switch_distortion;
	}
}

void antiaim::do_add()
{
	const auto add = get_antiaim( type )->add->get<int>();

	g_cmd->viewangles.y += add;
}

void antiaim::do_jitter()
{
	auto jitter_range = get_antiaim( type )->jitter_range->get<int>() * 0.5f;
	const auto jitter_speed = get_antiaim( type )->jitter_speed->get<int>();

	static auto last_set_tick = 0;
	static auto flip = false;

	static auto add = 0.f;

	if ( last_set_tick + jitter_speed < g_pLocalPlayer->get_tickbase() || last_set_tick > g_pLocalPlayer->get_tickbase() )
	{
		last_set_tick = g_pLocalPlayer->get_tickbase();

		if ( get_antiaim( type )->jitter_random->get<int>() )
		{
			jitter_range = random_float( -jitter_range, jitter_range );
			flip = true;
		}

		add = flip ? jitter_range : -jitter_range;

		flip = !flip;
	}

	g_cmd->viewangles.y += add;
}

void antiaim::do_freestand()
{
	static auto yaw = 0.f;
	if ( freestanding::get().get_real( yaw ) )
		g_cmd->viewangles.y = yaw;
}

bool antiaim::do_edge()
{
	return false;
}

void antiaim::do_at_target() const
{
	C_CSPlayer* target = nullptr;
	QAngle target_angle;

	QAngle original_viewangles;
	g_pEngine->GetViewAngles( original_viewangles );

	auto lowest_fov = 90.f;
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || !player->is_enemy() || player == g_pLocalPlayer )
			continue;

		if ( player->IsDormant() && ( player->get_simtime() > g_pGlobals->curtime || player->get_simtime() + 5.f < g_pGlobals->curtime ) )
			continue;

		auto enemy_pos = player->get_origin();
		enemy_pos.z += 64.f;

		const auto angle = math::get().calc_angle( g_pLocalPlayer->get_eye_pos(), enemy_pos );
		const auto fov = math::get().get_fov( original_viewangles, angle );

		if ( fov < lowest_fov )
		{
			target = player;
			lowest_fov = fov;
			target_angle = angle;
		}
	}

	if ( !target )
		return;

	g_cmd->viewangles.y = target_angle.y;
}

bool antiaim::do_lby()
{
	lby_prediction();

	return lby_update();
}