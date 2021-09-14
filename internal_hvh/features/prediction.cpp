#include "../include_cheat.h"
#include "../sdk/checksum_md5.h"

void prediction::start()
{
	//if (!move_data)
	//	move_data = std::malloc(182);

	//auto player = g_pLocalPlayer;

	//old_current_time = g_pGlobals->curtime;
	//old_frame_time = g_pGlobals->frametime;

	//g_pGlobals->curtime = player->get_tickbase() * g_pGlobals->interval_per_tick;
	//g_pGlobals->frametime = *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(n_interfaces::prediction) + 0x0A) ? 0 : g_pGlobals->interval_per_tick;

	//if (!m_prediction_random_seed || !m_prediction_player)
	//{
	//	m_prediction_random_seed = *reinterpret_cast<int**>(n_utilities::find_pattern("client_panorama.dll", "A3 ? ? ? ? 66 0F 6E 86") + 0x1);
	//	m_prediction_player = *reinterpret_cast<int**>(n_utilities::find_pattern("client_panorama.dll", "89 35 ? ? ? ? F3 0F 10 48") + 0x2);
	//}

	//// CPlayerMove::StartCommand
	//{
	//	*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x3334) = reinterpret_cast <std::uint32_t>(g_cmd);

	//	*m_prediction_random_seed = g_cmd ? g_cmd->random_seed : -1;
	//	*m_prediction_player = reinterpret_cast<int>(player);
	//}

	//g_pMoveHelper->SetHost(player);

	//g_pGameMovement->StartTrackPredictionErrors(player); // @BigMoneyBoss - (*(void (__thiscall **)(int *, _DWORD *))(game_movement + 12))(&game_movement, player);

	//if (g_cmd->weapon_select != 0) 
	//{
	//	C_BaseCombatWeapon* weapon = reinterpret_cast<C_BaseCombatWeapon*>(n_interfaces::entity_list->get_client_entity(g_cmd->weapon_select));

	//	if (weapon) 
	//	{
	//		CCSWeaponData* weapon_data = weapon->get_weapon_data();

	//		if (weapon_data)
	//			player->select_item(weapon_data->weapon_name, g_cmd->weapon_sub_type);
	//	}
	//}

	//C_BaseEntity* vehicle = nullptr;

	//if (player->get_h_vehicle().is_valid() /*&& player->get_h_vehicle( ).get( ) != nullptr*/)
	//	vehicle = reinterpret_cast<C_BaseEntity*>(g_pEntitiyList->GetClientEntityFromHandle(player->get_h_vehicle()));

	//if (g_cmd->impulse && (!vehicle || player->using_standard_weapons_in_vechile()))
	//	*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x31FC) = g_cmd->impulse;

	//// CBasePlayer::UpdateButtonState
	//{
	//	g_cmd->buttons |= *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x3330);

	//	int m_nButtons = g_cmd->buttons;
	//	int* m_afButtonLast = reinterpret_cast<int*>(reinterpret_cast<std::uint32_t>(player) + 0x31F8);
	//	int buttonsChanged = m_nButtons ^ *m_afButtonLast;
	//	*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x31EC) = *m_afButtonLast;
	//	*m_afButtonLast = m_nButtons;
	//	*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x31F0) = m_nButtons & buttonsChanged;
	//	*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x31F4) = buttonsChanged & ~m_nButtons;
	//}

	//g_pPrediction->check_moving_ground(player, g_pGlobals->frametime);

	////player->set_local_view_angles(g_cmd->viewangles);
	//g_pEngine->SetViewAngles(g_cmd->viewangles);

	//// CPlayerMove::RunPreThink
	//{
	//	if (player->physics_run_think(0))
	//		player->pre_think();
	//}

	//// CPlayerMove::RunThink
	//{
	//	int* thinktick = reinterpret_cast<int*>(reinterpret_cast<std::uint32_t>(player) + 0x0FC);
	//	if (*thinktick != -1 && *thinktick > 0 && *thinktick <= player->get_tick_base()) {
	//		*thinktick = -1;

	//		static auto unknown_fn = reinterpret_cast<void(__thiscall*)(int)>(n_utilities::find_pattern("client_panorama.dll", "55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B"));
	//		unknown_fn(0);

	//		player->think();
	//	}
	//}

	//g_pPrediction->SetupMove(player, g_cmd, g_pMoveHelper, reinterpret_cast<CMoveData*>(move_data));

	//if (vehicle)
	//	n_utilities::get_virtual_function< void(__thiscall*)(int, C_CSPlayer*, void*) >(vehicle, 5)(reinterpret_cast<std::uint32_t>(vehicle), player, move_data);
	//else
	//	g_pGameMovement->process_movement(player, reinterpret_cast<CMoveData*>(move_data));

	//g_pPrediction->FinishMove(player, g_cmd, reinterpret_cast<CMoveData*>(move_data));

	//g_pMoveHelper->process_impacts();

	//// CPlayerMove::RunPostThink
	//{
	//	auto post_think = [](C_CSPlayer* player) -> int
	//	{
	//		n_utilities::get_virtual_function< void(__thiscall*)(void*) >(n_interfaces::model_cache, 33)(n_interfaces::model_cache);

	//		if (player->get_alive() /* index 155 for vfunc */ || *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x3A81)) {
	//			n_utilities::get_virtual_function< void(__thiscall*)(void*) >(player, 339)(player);

	//			if (player->get_flags() & 1) // FL_ONGROUND
	//				*reinterpret_cast<std::uintptr_t*>(std::uintptr_t(player) + 0x3014) = 0;

	//			if (*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(player) + 0x28BC) == -1)
	//				n_utilities::get_virtual_function< void(__thiscall*)(void*, int) >(player, 218)(player, 0);

	//			n_utilities::get_virtual_function< void(__thiscall*)(void*) >(player, 219)(player);

	//			static auto post_think_v_physics = reinterpret_cast<bool(__thiscall*)(C_BaseEntity*)>(n_utilities::find_pattern("client_panorama.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB"));
	//			post_think_v_physics(player);
	//		}

	//		static auto simulate_player_simulated_entities = reinterpret_cast<bool(__thiscall*)(C_BaseEntity*)>(n_utilities::find_pattern("client_panorama.dll", "56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72"));
	//		simulate_player_simulated_entities(player);

	//		return n_utilities::get_virtual_function< int(__thiscall*)(void*) >(n_interfaces::model_cache, 34)(n_interfaces::model_cache);
	//	};

	//	post_think(player);
	//}
	// 
	// 
	// 
	static auto oldorigin = g_pLocalPlayer->get_origin();
	unpred_vel = (g_pLocalPlayer->get_origin() - oldorigin) * (1.0 / g_pGlobals->interval_per_tick);
	oldorigin = g_pLocalPlayer->get_origin();

	unpred_eyepos = g_pLocalPlayer->get_eye_pos();

	m_prediction_random_seed = *reinterpret_cast<int**> (offsets::pred_random_seed);

	*m_prediction_random_seed = MD5_PseudoRandom(g_cmd->command_number) & 0x7FFFFFFF;

	orig_currtime = g_pGlobals->curtime;
	orig_frametime = g_pGlobals->frametime;

	g_pGlobals->curtime = get_curtime();
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	g_pGameMovement->StartTrackPredictionErrors(g_pLocalPlayer);

	memset(&move_data, 0, sizeof(move_data));
	g_pMoveHelper->SetHost(g_pLocalPlayer);
	g_pPrediction->SetupMove(g_pLocalPlayer, g_cmd, g_pMoveHelper, &move_data);
	g_pGameMovement->ProcessMovement(g_pLocalPlayer, &move_data);
	g_pPrediction->FinishMove(g_pLocalPlayer, g_cmd, &move_data);

	static auto pred_oldorigin = g_pLocalPlayer->get_origin();
	pred_vel = (g_pLocalPlayer->get_origin() - pred_oldorigin) * (1.0 / g_pGlobals->interval_per_tick);
	pred_oldorigin = g_pLocalPlayer->get_origin();

	unpred_eyepos = g_pLocalPlayer->get_eye_pos();
}

void prediction::finish() const
{
	//g_pGameMovement->FinishTrackPredictionErrors(g_pLocalPlayer);

	//g_pMoveHelper->SetHost(0);

	//g_pGameMovement->Reset();

	//// CPlayerMove::FinishCommand
	//{
	//	*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint32_t>(g_pLocalPlayer) + 0x3334) = 0;

	//	*m_prediction_random_seed = -1;
	//	*prediction_player = 0;
	//}

	//if (g_pGlobals->frametime > 0.f)
	//	g_pLocalPlayer->get_tickbase()++;

	//g_pGlobals->curtime = orig_currtime;
	//g_pGlobals->frametime = orig_frametime;
	// 
	// 
	// 
	g_pGameMovement->FinishTrackPredictionErrors( g_pLocalPlayer );
	g_pMoveHelper->SetHost( nullptr );

	*m_prediction_random_seed = -1;

	if (g_pGlobals->frametime > 0.f)
		g_pLocalPlayer->get_tickbase()++;

	g_pGlobals->curtime = orig_currtime;
	g_pGlobals->frametime = orig_frametime;
}

int prediction::get_tickbase()
{
	return g_pLocalPlayer->get_tickbase();
}

float prediction::get_curtime()
{
	return get_tickbase() * g_pGlobals->interval_per_tick;
}

float prediction::get_unpred_curtime() const
{
	return orig_currtime;
}

Vector prediction::get_unpred_vel() const
{
	return unpred_vel;
}

Vector prediction::get_pred_vel() const
{
	return pred_vel;
}

Vector prediction::get_unpred_eyepos() const
{
	return unpred_eyepos;
}

void net_data::store()
{
	int          tickbase;
	StoredData_t* data;

	if (!g_pLocalPlayer->get_alive() || !g_pLocalPlayer)
	{
		reset();
		return;
	}

	tickbase = g_pLocalPlayer->get_tickbase();

	// get current record and store data.
	data = &m_data[tickbase % MULTIPLAYER_BACKUP];

	data->m_tickbase = tickbase;
	data->m_punch = g_pLocalPlayer->get_aim_punch();
	data->m_punch_vel = g_pLocalPlayer->get_aim_punch_vel();
	data->m_view_offset = g_pLocalPlayer->get_view_offset();
}

void net_data::apply(const ClientFrameStage_t stage)
{
	if (stage != FRAME_NET_UPDATE_END)
		return;
	int          tickbase;
	StoredData_t* data;
	QAngle        punch_delta, punch_vel_delta;
	Vector       view_delta;

	if (!g_pLocalPlayer->get_alive() || !g_pLocalPlayer)
	{
		reset();
		return;
	}

	tickbase = g_pLocalPlayer->get_tickbase();

	// get current record and validate.
	data = &m_data[tickbase % MULTIPLAYER_BACKUP];

	if (g_pLocalPlayer->get_tickbase() != data->m_tickbase)
		return;

	// get deltas.
	// note - dex;  before, when you stop shooting, punch values would sit around 0.03125 and then goto 0 next update.
	//              with this fix applied, values slowly decay under 0.03125.
	punch_delta = g_pLocalPlayer->get_aim_punch() - data->m_punch;
	punch_vel_delta = g_pLocalPlayer->get_aim_punch_vel() - data->m_punch_vel;
	view_delta = g_pLocalPlayer->get_view_offset() - data->m_view_offset;

	// set data.
	if (std::abs(punch_delta.x) < 0.03125f &&
		std::abs(punch_delta.y) < 0.03125f &&
		std::abs(punch_delta.z) < 0.03125f)
		g_pLocalPlayer->get_aim_punch() = data->m_punch;

	if (std::abs(punch_vel_delta.x) < 0.03125f &&
		std::abs(punch_vel_delta.y) < 0.03125f &&
		std::abs(punch_vel_delta.z) < 0.03125f)
		g_pLocalPlayer->get_aim_punch_vel() = data->m_punch_vel;

	if (std::abs(view_delta.x) < 0.03125f &&
		std::abs(view_delta.y) < 0.03125f &&
		std::abs(view_delta.z) < 0.03125f)
		g_pLocalPlayer->get_view_offset() = data->m_view_offset;
}

void net_data::reset() {
	m_data.fill(StoredData_t());
}