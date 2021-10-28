#pragma once

class misc : public singleton<misc>
{
public:
	static void bhop();
	static void remove_smoke( ClientFrameStage_t stage );
	static void remove_flash( ClientFrameStage_t stage );
	static void radar();
	static void reveal_ranks();
	static void soundesp( int idx, const char* sample, const Vector& origin );
	static void clan_tag();
	//static void automaticpeek();
	virtual void Instance();
	virtual void DrawCircle();
	static void aspectratio();
	//static void grenade_trajectory();
	//static void viewmodel_offset();
	static void play_freezetime_music();
public:
	QAngle g_real_angles;
	float_t m_flAnimationTime = 0.0f;

	Vector m_vecStartPosition = Vector(0, 0, 0);

	bool m_bTurnedOn = false;
	bool m_bNegativeSide = false;
	bool m_bRetreat = false;
	bool m_bRetreated = false;
	bool m_bForceDisabled = false;
	bool m_bWaitAnimationProgress = false;
};
