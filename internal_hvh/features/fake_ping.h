#pragma once

struct sequence_t
{
	sequence_t(int in_reliable, int out_reliable, int in_sequence, float realtime)
	{
		in_reliable_state = in_reliable;
		out_reliable_state = out_reliable;
		in_sequence_num = in_sequence;

		time = realtime;
	}

	int in_reliable_state;
	int out_reliable_state;
	int in_sequence_num;

	float time;
};

class fake_ping : public singleton<fake_ping>
{
public:
	int last_incoming_sequence = 0;
	std::deque<sequence_t> sequence_records;
public:
	static void ping_spike(CNetChannel* net_channel/*, float latency*/);
	static void update_incoming_sequences();
	static void unhook();
	static void update(const ClientFrameStage_t);
	static void rehook();
private:
	static void reset();
};
