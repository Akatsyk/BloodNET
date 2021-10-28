#include "../include_cheat.h"

bool __fastcall hooks::temp_entities( void* ECX, void* EDX, void* msg )
{
	if (!g_pLocalPlayer || !g_pEngine->IsInGame())
	{
		return orig_temp_entities(ECX, msg);
	}

	const bool ret = orig_temp_entities(ECX, msg);

	CEventInfo* ei = g_pClientState->m_Events;
	CEventInfo* next = nullptr;

	if (!ei) {
		return ret;
	}

	do {
		next = *reinterpret_cast<CEventInfo**>(reinterpret_cast<uintptr_t>(ei) + 0x38);

		uint16_t classID = ei->classID - 1;

		auto m_pCreateEventFn = ei->pClientClass->m_pCreateEventFn;
		if (!m_pCreateEventFn) {
			continue;
		}

		void* pCE = m_pCreateEventFn();
		if (!pCE) {
			continue;
		}

		if (classID == 170) {
			ei->fire_delay = 0.0f;
		}
		ei = next;
	} while (next != nullptr);

	return ret;
}