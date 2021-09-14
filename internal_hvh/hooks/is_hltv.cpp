#include "../include_cheat.h"
#include <intrin.h>

bool __fastcall hooks::is_hltv( void* ecx, void* edx )
{
	// if wanted, force the game to always call AccumulateLayers.

	if ( g_ishltv_fix )
		return true;

	// if wanted, force game to use abs velocity instead of estimating it.

	static auto setup_velocity = reinterpret_cast< uintptr_t* >( sig( "client.dll", "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80" ) );

	if ( g_setup_vel_fix && _ReturnAddress() == setup_velocity )
		return true;

	return orig_is_hltv( ecx );
}

bool __fastcall hooks::is_paused(void* ecx, void* edx)
{
	static auto return_to_extrapolation = reinterpret_cast<uintptr_t*>(sig("client.dll", "FF D0 A1 ? ? ? ? ? ? ? ? B9 ? ? ? ? ? ? ? ? D9 1D ? ? ? ? ? ? ? ? FF 50 34 85 C0 74 22 8B 0D ? ? ? ? ? ? ? ? ") + 0x29);
	static auto return_to_interpolation = reinterpret_cast<uintptr_t*>(sig("client.dll", "84 C0 74 07 C6 05 ? ? ? ? ? 8B 0D"));

	if (_ReturnAddress() == (void*)return_to_extrapolation || _ReturnAddress() == (void*)return_to_interpolation)
		return true;

	return orig_is_paused(ecx);
}