#pragma once

class misc : public singleton<misc>
{
public:
	QAngle g_real_angles;
	Vector g_autopeek_pos;
public:
	static void bhop();
	static void remove_smoke( ClientFrameStage_t stage );
	static void remove_flash( ClientFrameStage_t stage );
	static void radar();
	static void reveal_ranks();
	static void soundesp( int idx, const char* sample, const Vector& origin );
	static void clan_tag();
	static void automaticpeek();
	static void aspectratio();
	//static void grenade_trajectory();
	//static void viewmodel_offset();
	static void play_freezetime_music();
};
