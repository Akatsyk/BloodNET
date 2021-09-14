#pragma once

class prediction : public singleton<prediction>
{
public:
	void start();
	void finish() const;
	static int get_tickbase();
	static float get_curtime();
	float get_unpred_curtime() const;
	Vector get_unpred_vel() const;
	Vector get_pred_vel() const;
	Vector get_unpred_eyepos() const;
	QAngle unpred_viewangles;
private:
	float orig_currtime = 0.f;
	float orig_frametime = 0.f;
	CMoveData move_data;

	int* m_prediction_random_seed = nullptr;
	Vector unpred_vel;
	Vector unpred_eyepos;
	Vector pred_vel;
};

class net_data : public singleton<net_data>
{
private:
	class StoredData_t
	{
	public:
		int    m_tickbase;
		QAngle  m_punch;
		QAngle  m_punch_vel;
		Vector m_view_offset;

	public:
		__forceinline StoredData_t() : m_tickbase{}, m_punch{}, m_punch_vel{}, m_view_offset{} {};
	};

	std::array< StoredData_t, MULTIPLAYER_BACKUP > m_data;

public:
	void store();
	void apply(const ClientFrameStage_t stage);
	void reset();
};