#include "../include_cheat.h"

void _fastcall hooks::do_extra_bone_processing( void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, byte* boneComputed, CIKContext* context )
{
	if (g_pLocalPlayer->get_effects() & EF_NOINTERP)
		return;

	const auto state = uint32_t(g_pLocalPlayer->get_anim_state());

	if (!state)
		return orig_do_extra_bone_processing(ecx, hdr, pos, q, matrix, boneComputed, context);

	const auto backup_tickcount = *reinterpret_cast<int32_t*>(state + 8);
	*reinterpret_cast<int32_t*>(state + 8) = 0;
	orig_do_extra_bone_processing(ecx, hdr, pos, q, matrix, boneComputed, context);
	*reinterpret_cast<int32_t*>(state + 8) = backup_tickcount;
}