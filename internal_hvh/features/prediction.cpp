#include "../include_cheat.h"
#include "../sdk/checksum_md5.h"

void prediction::start()
{
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
	g_pGameMovement->FinishTrackPredictionErrors( g_pLocalPlayer );
	g_pMoveHelper->SetHost( nullptr );

	*m_prediction_random_seed = -1;

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