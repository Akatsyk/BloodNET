#include "../include_cheat.h"

long __stdcall hooks::present( IDirect3DDevice9* device, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion )
{
	if ( g_unload )
		return orig_present( device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );

	IDirect3DStateBlock9* state;
	if ( device->CreateStateBlock( D3DSBT_ALL, &state ) )
		return orig_present( device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );

	render::get().init( device );
	render::get().setup_render_state();

	visuals::get().render();
	c_menu::get().toggle();
	c_menu::get().render();

	bool old_tickbase = g_tickbase.m_shift_data.m_should_attempt_shift;

	// note - alpha;
		// the maximum amount of ticks we can shift is sv_maxusrcmdprocessticks
		// which is defaulted to 16 - but since we want to be fakelagging atleast 2 ticks
		// for our fake angle to work, we need to reserve 2 ticks for the fakelag. 
		// we want our doubletap to be as fast as possible, and our fake to be as wide as possible
		// therefore let's just fakelag 2 ticks - resulting in our max shift ticks being 14
		// cos sv_maxusrcmdprocessticks Take Away Two Is Fourteen
		// (assuming that the convar wasn't changed to a higher/lower value).
	m_goal_shift = 13;

	// (!) Temporary - replace with checkbox & hotkey later.
	if (old_tickbase != g_tickbase.m_shift_data.m_should_attempt_shift)
	{
		//g_notify.add( tfm::format( XOR( "Tried shifting tickbase" )) );

		if (g_tickbase.m_shift_data.m_should_attempt_shift)
			g_tickbase.m_shift_data.m_needs_recharge = m_goal_shift;
		else
			g_tickbase.m_shift_data.m_needs_recharge = 0;

		g_tickbase.m_shift_data.m_did_shift_before = false;
	}

	state->Apply();
	state->Release();

	return orig_present( device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
}

