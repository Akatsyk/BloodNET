#include "../include_cheat.h"

void __stdcall hooks::frame_stage_notify( ClientFrameStage_t stage )
{
	if ( g_erase )
	{
		erase_fn( init::on_startup );
		g_erase = false;
	}

	chams::get().dark_mode();

	if ( g_unload )
		return orig_frame_stage_notify( stage );

	if (stage == FRAME_NET_UPDATE_START)
	{
		m_rate = (int)std::round(1.f / g_pGlobals->interval_per_tick);
	}

	freestanding::get().update_hotkeys( stage );

	misc::remove_smoke( stage );

	misc::remove_flash( stage );

	dormancy_manager::get().position_correction(stage);

	resolver::get().approve_shots( stage );

	resolver::get().update_missed_shots( stage );

	fake_ping::get().update( stage );

	animations::get().stop( stage );

	animations::get().fix_local_anims( stage );

	net_data::get().apply( stage );

	//resolver::get().extrpolate_players(stage);

	orig_frame_stage_notify( stage );

	resolver::get().collect_wall_detect( stage );

	player_log::get().log( stage );
}