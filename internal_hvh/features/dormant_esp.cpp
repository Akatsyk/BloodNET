#include "../include_cheat.h"

#define range( x, a, b )    ( x >= a && x <= b )

void dormancy_manager::position_correction( int stage ) {
	if ( stage != FRAME_RENDER_START)
	//if ( stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	for ( int i = 1; i <= g_pGlobals->maxClients; i++ ) 
	{
		auto entity = get_entity( i );

		if ( entity == g_pLocalPlayer )
			continue;

		if ( !viable( entity ) )
			continue;

		if ( !entity->IsDormant( ) && !players[ i ].set )
			players[ i ].oldpos = entity->get_origin( );

		if ( !entity->IsDormant( ) || players[ i ].set )
			continue;

		entity->set_abs_origin( players[ i ].oldpos );
	}
}

bool dormancy_manager::set( const int index ) {
	if ( !range( index, 1, 64 ) )
		return false;

	return players[ index ].set;
}

bool dormancy_manager::viable( C_CSPlayer* entity ) {
	if ( !entity )
		return false;

	if ( !g_pLocalPlayer )
		return false;

	if ( !entity->get_alive( ) || entity->get_flags( ) & ( 1 << 6 ) )
		return false;

	if ( entity->get_team( ) == g_pLocalPlayer->get_team( ) )
		return false;

	if ( entity->get_immunity( ) )
		return false;

	return true;
}

void dormancy_manager::sound_dormancy( ) {
	soundList.RemoveAll( );
	g_pEngineSound->GetActiveSounds( soundList );

	if ( !soundList.Count( ) )
		return;

	for ( int i = 0; i < soundList.Count( ); i++ ) {
		soundInfo& sound = soundList[ i ];
		if ( !range( sound.sound_source, 1, 64 ) )
			continue;

		if ( sound.sound_source == g_pEngine->GetLocalPlayer( ) )
			continue;

		auto entity = reinterpret_cast< C_CSPlayer* >(get_entity( sound.sound_source ) );

		if ( !entity || sound.origin->IsZero( ) /*|| !valid_sound( sound )*/ )
			continue;

		auto& player = players[ sound.sound_source ];
		player.entity = entity;

	
		Vector src_3d, dst_3d;
		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;

		filter.pSkip = entity;
		src_3d = ( *sound.origin ) + Vector( 0, 0, 1 );
		dst_3d = src_3d - Vector( 0, 0, 100 );
		ray.Init( src_3d, dst_3d );

		g_pTrace->TraceRay( ray, MASK_PLAYERSOLID, &filter, &tr );

		if ( tr.allsolid ) // if stuck
			player.sounddata.time = -1;

		*sound.origin = ( ( tr.fraction < 0.97 ) ? tr.endpos : *sound.origin );
		player.flags = entity->get_flags( );
		player.flags |= ( tr.fraction < 0.50f ? ( 1 << 1 ) /*ducking*/ : 0 ) | ( tr.fraction != 1 ? ( 1 << 0 ) /*on ground*/ : 0 );
		player.flags &= ( tr.fraction > 0.50f ? ~( 1 << 1 ) /*ducking*/ : 0 ) | ( tr.fraction == 1 ? ~( 1 << 0 ) /*on ground*/ : 0 );

		player.oldpos = *sound.origin;
		player.sounddata.time = GetTickCount( );

		if ( !entity->IsDormant( ) )
			continue;

		adjust_player( sound.sound_source );
	}

	soundBuffer = soundList;
}

void dormancy_manager::adjust_player( const int index ) 
{
	if ( !vars.visuals.esp.dormant_esp.get<bool>() )
		return;

	auto& player = players[ index ];

	static int duration = 1000;
	bool expired = GetTickCount( ) - player.sounddata.time > duration;

	if ( expired )
		player.set = false;

	if ( !expired )
		player.set = true;

	// dormant check
	player.entity->get_eflags() = player.flags;
	player.entity->get_origin( ) = player.oldpos;
	player.entity->set_abs_origin( player.oldpos );
}
//bool dormancy_manager::valid_sound( soundInfo& sound ) 
//{
//	if ( !sound.from_server )
//		return false;
//
//	for ( int i = 0; i < soundBuffer.Count( ); i++ ) {
//		soundInfo& cached_sound = soundBuffer[ i ];
//		if ( cached_sound.guid == sound.guid )
//			return false;
//	}
//
//	return true;
//}