#include "../include_cheat.h"

#define dont

TickbaseSystem g_tickbase;

void TickbaseSystem::WriteUserCmd(bf_write* buf, CUserCmd* incmd, CUserCmd* outcmd) {
	// Generic WriteUserCmd code as seen in every hack ever
	using WriteUsercmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);
	static WriteUsercmd_t fnWriteUserCmd = (WriteUsercmd_t)sig("client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");

	__asm
	{
		mov     ecx, buf
		mov     edx, incmd
		push    outcmd
		call    fnWriteUserCmd
		add     esp, 4
	}
}

bool __fastcall hooks::WriteUserCmdDeltaToBuffer(int m_nSlot, void* m_pBuffer, int m_nFrom, int m_nTo, bool m_bNewCmd)
{

	if (g_tickbase.m_shift_data.m_ticks_to_shift <= 0)
	{
		return orig_WriteUserCmdDeltaToBuffer(m_nSlot, m_pBuffer, m_nFrom, m_nTo, m_bNewCmd);

		//	return g_hooks.m_client.GetOldMethod<WriteUsercmdDeltaToBuffer_t>(24)(this, m_nSlot, m_pBuffer, m_nFrom, m_nTo, m_bNewCmd);
	}

	if (m_nFrom != -1)
		return true;

	m_nFrom = -1;

	int m_nTickbase = g_tickbase.m_shift_data.m_ticks_to_shift;
	g_tickbase.m_shift_data.m_ticks_to_shift = 0;

	int* m_pnNewCmds = (int*)((uintptr_t)m_pBuffer - 0x2C);
	int* m_pnBackupCmds = (int*)((uintptr_t)m_pBuffer - 0x30);

	*m_pnBackupCmds = 0;

	int m_nNewCmds = *m_pnNewCmds;
	int m_nNextCmd = g_pClientState->m_nChokedCommands + g_pClientState->m_nLastOutgoingCommand + 1;
	int m_nTotalNewCmds = std::min(m_nNewCmds + abs(m_nTickbase), 16);

	*m_pnNewCmds = m_nTotalNewCmds;

	for (m_nTo = m_nNextCmd - m_nNewCmds + 1; m_nTo <= m_nNextCmd; m_nTo++)
	{
		if (!orig_WriteUserCmdDeltaToBuffer(m_nSlot, m_pBuffer, m_nFrom, m_nTo, m_bNewCmd))
			return false;

		m_nFrom = m_nTo;
	}

	CUserCmd* m_pCmd = g_pInput->GetUserCmd(m_nSlot, m_nFrom);
	if (!m_pCmd)
		return true;

	CUserCmd m_ToCmd = *m_pCmd, m_FromCmd = *m_pCmd;
	m_ToCmd.command_number++;
	m_ToCmd.tick_count += 3 * m_rate;

	for (int i = m_nNewCmds; i <= m_nTotalNewCmds; i++)
	{
		g_tickbase.WriteUserCmd((bf_write*)m_pBuffer, &m_ToCmd, &m_FromCmd);
		m_FromCmd = m_ToCmd;

		m_ToCmd.command_number++;
		m_ToCmd.tick_count++;
	}

	g_tickbase.m_shift_data.m_current_shift = m_nTickbase;
	return true;
}

void TickbaseSystem::PreMovement()
{
	// Invalidate next shift amount and the ticks to shift prior to shifting
	g_tickbase.m_shift_data.m_next_shift_amount = g_tickbase.m_shift_data.m_ticks_to_shift = 0;
}

bool CanFireWeapon(float curtime)
{
	auto m_weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	// the player cant fire.
	//if (!m_player_fire)
	//	return false;

	if (m_weapon->is_grenade())
		return false;

	// if we have no bullets, we cant shoot.
	if (m_weapon->get_weapon_type() != WEAPONTYPE_KNIFE && m_weapon->get_clip1() < 1)
		return false;

	// r8 revolver.
	if (m_weapon->get_weapon_id() == WEAPON_REVOLVER)
	{
		// mouse1.
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// yeez we have a normal gun.
	if (curtime >= m_weapon->get_next_primary_attack())
		return true;

	return false;
}

bool IsFiring(float curtime)
{
	auto m_weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	if (!m_weapon)
		return false;

	const auto IsZeus = m_weapon->get_weapon_id() == WEAPON_TASER;
	const auto IsKnife = !IsZeus && m_weapon->get_weapon_type() == WEAPONTYPE_KNIFE;

	if (m_weapon->is_grenade())
		return !m_weapon->get_pin_pulled() && m_weapon->get_throw_time() > 0.f && m_weapon->get_throw_time() < curtime;
	else if (IsKnife)
		return (g_cmd->buttons & (IN_ATTACK) || g_cmd->buttons & (IN_ATTACK2)) && CanFireWeapon(curtime);
	else
		return g_cmd->buttons & (IN_ATTACK) && CanFireWeapon(curtime);
}

void TickbaseSystem::PostMovement()
{
	// Perform sanity checks to make sure we're able to shift
	if (!g_pEngine->IsConnected() || !vars.misc.doubletap.get<bool>())
	{
		return;
	}


	if (!g_cmd)
	{
		return;
	}

	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	if (weapon->get_weapon_id() == WEAPON_REVOLVER ||
		weapon->get_weapon_id() == WEAPON_C4 ||
		weapon->get_weapon_type() == WEAPONTYPE_KNIFE ||
		weapon->is_grenade())
	{
		g_tickbase.m_shift_data.m_did_shift_before = false;
		g_tickbase.m_shift_data.m_should_be_ready = false;
		g_tickbase.m_shift_data.m_should_disable = true;
		return;
	}

	// Don't attempt to shift if we're supposed to
	if (!g_tickbase.m_shift_data.m_should_attempt_shift)
	{
		g_tickbase.m_shift_data.m_did_shift_before = false;
		g_tickbase.m_shift_data.m_should_be_ready = false;
		return;
	}

	g_tickbase.m_shift_data.m_should_disable = false;

	// Setup variables we will later use in order to dermine if we're able to shift tickbase or not

	// This could be made into a checkbox 
	bool bFastRecovery = false;

	float flNonShiftedTime = ticks_to_time(g_pLocalPlayer->get_tickbase() - m_goal_shift);

	// Determine if we are able to shoot right now (at the time of the shift)
	bool bCanShootNormally = CanFireWeapon(ticks_to_time(g_pLocalPlayer->get_tickbase()));

	// Determine if we are able to shoot in the previous iShiftAmount ticks (iShiftAmount ticks before we shifted)
	bool bCanShootIn12Ticks = CanFireWeapon(flNonShiftedTime);

	bool bIsShooting = IsFiring(ticks_to_time(g_pLocalPlayer->get_tickbase()));

	// Determine if we are able to shift the tickbase respective to previously setup variables (rofl)
	g_tickbase.m_shift_data.m_can_shift_tickbase = bCanShootIn12Ticks || (!bCanShootNormally || bFastRecovery) && (g_tickbase.m_shift_data.m_did_shift_before);

	// If we can shift tickbase, shift enough ticks in order to double-tap
	// Always prioritise fake-duck if we wish to
	if (g_tickbase.m_shift_data.m_can_shift_tickbase && !GetAsyncKeyState(vars.key.fakeduck.get<int>()))
	{
		// Tell the cheat to shift tick-base and disable fakelag
		g_tickbase.m_shift_data.m_next_shift_amount = m_goal_shift;
	}
	else {
		g_tickbase.m_shift_data.m_next_shift_amount = 0;
		g_tickbase.m_shift_data.m_should_be_ready = false;
	}

	// we want to recharge after stopping fake duck.
	if (GetAsyncKeyState(vars.key.fakeduck.get<int>()))
	{
		g_tickbase.m_shift_data.m_prepare_recharge = true;

		g_tickbase.m_shift_data.m_next_shift_amount = 0;
		g_tickbase.m_shift_data.m_should_be_ready = false;

		g_tickbase.m_shift_data.m_should_disable = true;

		return;
	}

	// Are we even supposed to shift tickbase?
	if (g_tickbase.m_shift_data.m_next_shift_amount > 0)
	{
		// Prevent m_iTicksAllowedForProcessing from being incremented.
		// g_cl.m_cmd->m_tick = INT_MAX;
		// Determine if we're able to double-tap  
		if (bCanShootIn12Ticks)
		{
			if (g_tickbase.m_shift_data.m_prepare_recharge && !bIsShooting)
			{
				g_tickbase.m_shift_data.m_needs_recharge = m_goal_shift;
				g_tickbase.m_shift_data.m_prepare_recharge = false;
			}
			else
			{
				if (bIsShooting)
				{

					// Store shifted command
					g_tickbase.m_prediction.m_shifted_command = g_cmd->command_number;

					// Store shifted ticks
					g_tickbase.m_prediction.m_shifted_ticks = abs(g_tickbase.m_shift_data.m_current_shift);

					// Store original tickbase
					g_tickbase.m_prediction.m_original_tickbase = g_pLocalPlayer->get_tickbase();

					// Update our wish ticks to shift, and later shift tickbase
					g_tickbase.m_shift_data.m_ticks_to_shift = g_tickbase.m_shift_data.m_next_shift_amount;

					//Akatsuki:: trying make teleport doubletap
				}
			}
		}
		else
		{
			g_tickbase.m_shift_data.m_prepare_recharge = true;
			g_tickbase.m_shift_data.m_should_be_ready = false;
		}
	}
	else
	{
		g_tickbase.m_shift_data.m_prepare_recharge = true;
		g_tickbase.m_shift_data.m_should_be_ready = false;
	}

	// Note DidShiftBefore state 
	g_tickbase.m_shift_data.m_did_shift_before = g_tickbase.m_shift_data.m_next_shift_amount > 0;
}