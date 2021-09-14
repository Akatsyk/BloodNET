#include "../include_cheat.h"
#include <intrin.h>

void write_user_cmd(bf_write* buf, CUserCmd* in, CUserCmd* out)
{
	static DWORD write_user_cmd_f = (DWORD)sig("client_panorama.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");

	__asm
	{
		mov     ecx, buf
		mov     edx, in
		push    out
		call    write_user_cmd_f
		add     esp, 4
	}
};

bool __fastcall hooks::WriteUserCmdDeltaToBuffer(void* ecx_, void* edx_, int nslot, bf_write* buf, int from, int to, bool isnewcmd)
{
	static auto sendmovecall = (void*)sig("engine.dll", "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84");

	if (_ReturnAddress() != sendmovecall || m_tickbase_shift <= 0) {
		m_tickbase_shift = 0;
		return orig_WriteUserCmdDeltaToBuffer(ecx_, edx_, nslot, buf, from, to, isnewcmd);
	}

	if (from != -1)
		return true;

	int* number_of_backup_commands = (int*)((uintptr_t)buf - 0x30);
	int* number_of_new_commands = (int*)((uintptr_t)buf - 0x2C);
	int32_t new_commands = *number_of_new_commands;

	auto nextcmdnumber = g_pClientState->m_nLastOutgoingCommand + g_pClientState->m_nChokedCommands + 1;
	auto totalnewcommands = std::min(m_tickbase_shift, 62);
	m_tickbase_shift -= totalnewcommands;

	from = -1;
	*number_of_new_commands = totalnewcommands;
	*number_of_backup_commands = 0;

	for (to = nextcmdnumber - new_commands + 1; to <= nextcmdnumber; to++) {
		if (!orig_WriteUserCmdDeltaToBuffer(ecx_, edx_, nslot, buf, from, to, true))
			return false;

		from = to;
	}

	auto lastrealcmd = g_pInput->GetUserCmd(nslot, from);
	CUserCmd fromcmd;

	if (lastrealcmd)
		fromcmd = *lastrealcmd;

	CUserCmd tocmd = fromcmd;
	tocmd.command_number = nextcmdnumber++;
	tocmd.tick_count += 100;

	for (int i = new_commands; i <= totalnewcommands; i++) {
		write_user_cmd(buf, &tocmd, &fromcmd);
		fromcmd = tocmd;
		tocmd.command_number++;
		tocmd.tick_count++;
	}

	return true;
}