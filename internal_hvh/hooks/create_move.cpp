#include "../include_cheat.h"
#include <intrin.h>

bool __stdcall hooks::create_move(float input_sample_time, CUserCmd* cmd)
{
	g_cmd = cmd;
	g_reliable_state = -1;
	g_sequence_rn = -1;

	if (!cmd->command_number)
		return orig_create_move(input_sample_time, cmd);

	misc::clan_tag();
	misc::reveal_ranks();

	uintptr_t* frame_pointer;
	__asm mov frame_pointer, ebp;
	g_send_packet = reinterpret_cast<bool*>(*frame_pointer - 0x1C);
	*g_send_packet = true;

	// сторим углы для всяких автострейферов, мувмент фиксов и прочей ебалы.
	g_pEngine->GetViewAngles(misc::get().g_real_angles);
	prediction::get().unpred_viewangles = cmd->viewangles;
	autostrafer::get().m_strafe_angles = cmd->viewangles;

	if (g_unload)
		return orig_create_move(input_sample_time, cmd);

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
		fake_ping::rehook();
	else if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
	{
		fake_ping::unhook();
		fake_ping::get().sequence_records.clear();
		g_sequence_rn = 0;
	}

	//fake_ping::rehook();

	fake_ping::update_incoming_sequences();

	misc::get().radar();
	misc::get().aspectratio();
	autostrafer::strafe();
	misc::get().bhop();
	g_cmd->buttons |= IN_BULLRUSH;

	if (cmd->buttons & IN_ATTACK && !aimbot::get().get_aimbot_state() && aimbot::get().can_shoot(prediction::get().get_curtime(), true))
		resolver::get().add_shot(/*prediction::get().get_unpred_eyepos()*/g_pLocalPlayer->get_shoot_pos(), nullptr, -1);

	prediction::get().start();
	{
		prediction::get().unpred_viewangles = cmd->viewangles;
		aimbot::get().automatic_revolver();
		grenade_pred::get().trace(); 
		g_tickbase.PreMovement();
		antiaim::get().run();
		antiaim::get().fakeduck();
		aimbot::get().run();
		aimbot::get().no_recoil();
		misc::get().automaticpeek();
		g_tickbase.PostMovement();
	}
	prediction::get().finish();

	// клемпим углы.
	cmd->viewangles.Clamp();

	antiaim::get().fix_movement();

	if (!g_pClientState->m_nChokedCommands)
	{
		//animations::get().g_radar = cmd->viewangles;
		//animations::get().g_radar.Normalize();
		resolver::get().set_last_eye();
		thirdperson::get().set_tp_angle(cmd->viewangles);
	}

	__(cl_sidespeed_s, "cl_sidespeed");
	__(cl_forwardspeed_s, "cl_forwardspeed");
	__(cl_upspeed_s, "cl_upspeed");
	static ConVar* cl_sidespeed = g_pCVar->FindVar(cl_sidespeed_s);
	static ConVar* cl_forwardspeed = g_pCVar->FindVar(cl_forwardspeed_s);
	static ConVar* cl_upspeed = g_pCVar->FindVar(cl_upspeed_s);

	cmd->sidemove = clamp(cmd->sidemove, -cl_sidespeed->get_float(), cl_sidespeed->get_float());
	cmd->forwardmove = clamp(cmd->forwardmove, -cl_forwardspeed->get_float(), cl_forwardspeed->get_float());
	cmd->upmove = clamp(cmd->upmove, -cl_upspeed->get_float(), cl_upspeed->get_float());

	if (antiaim::get().get_fake_walk_state())
	{
		cmd->buttons &= ~IN_MOVERIGHT;
		cmd->buttons &= ~IN_MOVELEFT;
		cmd->buttons &= ~IN_FORWARD;
		cmd->buttons &= ~IN_BACK;
	}
	return false;
}