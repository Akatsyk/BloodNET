#include "../include_cheat.h"

void __fastcall hooks::run_command(void* ecx_, void* edx_, C_BasePlayer* entity, CUserCmd* ucmd, IMoveHelper* move_helper) {
	if (g_unload)
		return orig_run_command(ecx_, edx_, entity, ucmd, move_helper);

	// airstuck jitter / overpred fix.
	if (ucmd->tick_count >= std::numeric_limits< int >::max())
		return;

	orig_run_command(ecx_, edx_, entity, ucmd, move_helper);

	// store non compressed netvars.
	net_data::get().store();
}