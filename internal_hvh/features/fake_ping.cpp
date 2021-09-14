#include "../include_cheat.h"

void fake_ping::update(const ClientFrameStage_t stage)
{
	static bool prev_state;
	const bool state = GetAsyncKeyState(vars.key.ping_spike.get<int>());
	if (prev_state != state)
	{
		if (state)
			vars.aim.ping_spike.set(!vars.aim.ping_spike.get<bool>());

		prev_state = state;
	}

	if (stage == FRAME_START)
		unhook();
}

void fake_ping::reset()
{
	if (hooks::net_channel)
	{
		delete hooks::net_channel;
		hooks::net_channel = nullptr;
	}
}

void fake_ping::ping_spike(CNetChannel* net_channel/*, float latency*/)
{
	//for (const auto& sequence : fake_ping::get().sequence_records)
	//{
	//	if (prediction::get().get_curtime() - sequence.time >= latency)
	//	{
	//		net_channel->m_nInReliableState = sequence.in_reliable_state;
	//		net_channel->m_nInSequenceNr = sequence.in_sequence_num;
	//		break;
	//	}
	//}

	auto nci = g_pEngine->GetNetChannelInfo();
	float in_latency = nci->GetLatency(0);
	float aaa = 0.f;

	if (vars.aim.ping_spike.get<bool>())
		aaa = vars.aim.ping_spike_amt.get<float>() * 0.001f;

	else if (vars.aim.ping_spike_base.get<bool>())
		aaa = 160.f * 0.001f;

	// amount of latency we want to achieve
	float latency = 0.15f - (in_latency + g_pGlobals->frametime) - g_pGlobals->interval_per_tick;

	if (g_pLocalPlayer && g_pLocalPlayer->get_alive())
	{
		// god this is autistic
		for (auto& it : fake_ping::get().sequence_records)
		{
			float delta = prediction::get().get_curtime() - it.time;

			if (delta >= latency) 
			{
				// apply latency
				net_channel->m_nInReliableState = it.in_reliable_state;
				net_channel->m_nInSequenceNr =  it.in_sequence_num;;
				//net_channel->m_nInSequenceNr += 2 * (64 - 3) - static_cast<uint32_t>((64 - 3) * latency);
				break;
			}
		}
	}

	//if (g_pLocalPlayer->get_health() < 1)
	//	return;

	//static auto oldseqnum = 0;

	//if (oldseqnum == net_channel->m_nInSequenceNr)
	//	return;

	//oldseqnum = net_channel->m_nInSequenceNr;

	//if (vars.aim.ping_spike.get<bool>())
	//{
	//	net_channel->m_nInSequenceNr += NET_FRAMES_MASK * 2 - static_cast<uint32_t>(NET_FRAMES_MASK * (vars.aim.ping_spike_amt.get<float>() / 1000.f));
	//	return;
	//}

	//if (vars.aim.ping_spike_base.get<bool>())
	//{
	//	net_channel->m_nInSequenceNr += NET_FRAMES_MASK * 2 - static_cast<uint32_t>(NET_FRAMES_MASK * (160.f / 1000.f));
	//}
}

void fake_ping::update_incoming_sequences()
{
	auto nci = g_pClientState->m_NetChannel;
	if (!nci)
		return;

	if (nci->m_nInSequenceNr > fake_ping::get().last_incoming_sequence || !nci->m_nInSequenceNr)
	{
		fake_ping::get().last_incoming_sequence = nci->m_nInSequenceNr;

		fake_ping::get().sequence_records.push_front(sequence_t(nci->m_nInReliableState, /*nci->m_nOutReliableState TESTING*/ nci->m_nInReliableState, nci->m_nInSequenceNr, prediction::get().get_curtime()));
	}

	if (fake_ping::get().sequence_records.size() > 2048)
		fake_ping::get().sequence_records.pop_back();
}

void fake_ping::unhook()
{
	const auto clientstate = *reinterpret_cast<uintptr_t*>(g_pClientState);
	if (clientstate && g_pEngine->IsInGame() && g_pClientState->m_NetChannel && !g_unload)
		return;

	reset();
}

void fake_ping::rehook()
{
	if (hooks::net_channel || g_unload)
		return;

	fake_ping::get().sequence_records.clear();

	const auto net_channel = g_pClientState->m_NetChannel;
	if (!net_channel)
		return;

	hooks::net_channel = new c_hook(net_channel);
	hooks::orig_send_datagram = hooks::net_channel->apply<hooks::send_datagram_fn>(48, hooks::send_datagram);
	hooks::orig_send_net_msg = hooks::net_channel->apply< hooks::send_net_msg_fn>(42, hooks::send_net_msg);
}
