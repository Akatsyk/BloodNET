#pragma once

class antiaim : public singleton<antiaim>
{
public:
	float lineLBY = 0.f,
		lineFakeAngle = 0.f,
		lineRealAngle = 0.f;
public:
	void run();
	static void fix_movement();
	int get_rest_choke() const { return to_choke - g_pClientState->m_nChokedCommands; }
	int get_to_choke() const { return to_choke; }
	float get_next_update() const;
	float get_target_lby() const { return target_lby; }
	void choose_real();
	void fakeduck();
	bool get_fake_walk_state() const;
	int type = 0;
	bool update_lby = false;
	float next_lby_update = 0.f;
	float next_update = 0.f;

private:
	void choose_fk();
	void update_animstate();
	void fake_lag();
	void fake_walk();

	static bool on_peek_fakelag();
	void do_spin() const;
	void do_add();
	void do_jitter( );
	void do_freestand();
	bool do_edge( );
	void do_at_target() const;
	bool do_lby( );

	bool lby_update( );
	void lby_prediction();

	uint32_t to_choke = 0;
	float fake_walk_called = 0.f;
	float initial_lby = 0.f;
	float target_lby = 0.f;
	bool firstupdate = true;
	bool secondupdate = true;
	bool switch_distortion = false;
	float timer_distortion = 0.f;
};
