#pragma once
#include "../include_cheat.h"

struct dormancyData
{
	C_CSPlayer* entity;
	bool	prevstate;
	bool	set;
	int		flags;
	Vector	oldpos = Vector( 0.f, 0.f, 0.f );
	struct {
		int time;
	} sounddata;
};

class dormancy_manager : public singleton<dormancy_manager>
{
public:
	void position_correction( int stage );
	bool set( const int index );

	bool viable( C_CSPlayer* entity );

	void sound_dormancy( );
	void adjust_player( const int index );
	void restore_sound_dormancy( ); // restoreSoundDormancy
	bool valid_sound( soundInfo& sound );
private:
	CUtlVector<soundInfo> soundBuffer;
	CUtlVector<soundInfo> soundList;

	dormancyData players[ 65 ];
};