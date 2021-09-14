#include "../include_cheat.h"

int __fastcall hooks::send_datagram(void* netchan, void*, void* datagram)
{
	//if (!g_pEngine->IsInGame() || g_unload)
	//	return orig_send_datagram(netchan, datagram);

	//auto nci = g_pEngine->GetNetChannelInfo();
	//if (!nci)
	//	return orig_send_datagram(netchan, datagram);

	//float latency = nci->GetLatency(FLOW_OUTGOING);

	//const auto net_channel = reinterpret_cast<CNetChannel*> (netchan);

	///*const*/ auto backup_inrelstate = 0;//net_channel->m_nInReliableState;
	///*const*/ auto backup_seqnr = 0;//net_channel->m_nInSequenceNr;

	//if (backup_inrelstate == net_channel->m_nInReliableState || backup_seqnr == net_channel->m_nInSequenceNr)
	//	return orig_send_datagram(netchan, datagram);

	//backup_inrelstate = net_channel->m_nInReliableState;
	//backup_seqnr = net_channel->m_nInSequenceNr;

	//if (vars.aim.ping_spike.get<bool>() || vars.aim.ping_spike_base.get<bool>())
	//{
	//	float ping = !vars.aim.ping_spike.get<bool>() ? 200 : vars.aim.ping_spike_amt.get<float>();

	//	// the target latency.
	//	float correct = max(0.f, (ping / 1000.f) - latency - lagcomp::get().get_lerp_time());

	//	fake_ping::get().ping_spike(net_channel, correct);
	//}

	//int ret = orig_send_datagram(net_channel, datagram);

	//net_channel->m_nInReliableState = backup_inrelstate;
	//net_channel->m_nInSequenceNr = backup_seqnr;

	//return ret;

	const auto net_channel = reinterpret_cast<CNetChannel*> (netchan);
	auto cl = g_pClientState;
	if (!cl || cl->m_nDeltaTick <= 0) 
	{
		return orig_send_datagram(netchan, datagram);
	}

	int backup_state = net_channel->m_nInReliableState;
	int backup_sequence = net_channel->m_nInSequenceNr;

	fake_ping::get().ping_spike(net_channel);

	int ret = orig_send_datagram(netchan, datagram);

	net_channel->m_nInReliableState = backup_state;
	net_channel->m_nInSequenceNr = backup_sequence;

	return ret;
}

bool __fastcall hooks::send_net_msg(void* netchan, void* edx, INetMessage& msg, bool reliable, bool voice)
{
	if (msg.GetGroup() == 9)
		voice = true;

	return orig_send_net_msg(netchan, msg, reliable, voice);
}