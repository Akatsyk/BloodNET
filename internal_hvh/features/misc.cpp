#include "../include_cheat.h"

void misc::bhop()
{
	if (!vars.misc.bhop.get<bool>())
		return;

	__(sv_autobunnyhopping_s, "sv_autobunnyhopping");
	__(sv_enablebunnyhopping_s, "sv_enablebunnyhopping");
	g_pCVar->FindVar(sv_autobunnyhopping_s)->set_value(vars.misc.bhop.get<bool>());
	g_pCVar->FindVar(sv_enablebunnyhopping_s)->set_value(vars.misc.bhop.get<bool>());

	static auto last_jumped = false;
	static auto should_fake = false;

	if (g_pLocalPlayer->get_move_type() != MOVETYPE_LADDER && g_pLocalPlayer->get_move_type() != MOVETYPE_NOCLIP && !(g_pLocalPlayer->get_flags() & FL_FAKECLIENT)) {
		if (!last_jumped && should_fake)
		{
			should_fake = false;
			if (vars.misc.bhop.get<bool>())
				g_cmd->buttons |= IN_JUMP;
		}
		else if (g_cmd->buttons & IN_JUMP) 
		{

			if (g_pLocalPlayer->get_flags() & FL_ONGROUND) 
			{
				last_jumped = true;
				should_fake = true;
			}
			else
			{
				if (vars.misc.bhop.get<bool>())
					g_cmd->buttons &= ~IN_JUMP;

				last_jumped = false;
			}
		}
		else
		{
			last_jumped = false;
			should_fake = false;
		}
	}
}

void misc::remove_smoke(const ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START || !vars.misc.remove_smoke.get<bool>())
		return;

	static auto smoke_count = *reinterpret_cast<uintptr_t*>(offsets::smoke_count);
	*reinterpret_cast<int*>(smoke_count) = 0;
}

void misc::remove_flash(const ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START || !vars.misc.remove_flash.get<bool>() || !g_pLocalPlayer)
		return;

	g_pLocalPlayer->get_flash_alpha() = 0.f;
}

void misc::radar()
{
	if (!vars.misc.radar.get<bool>())
		return;

	for (auto i = 1; i <= g_pGlobals->maxClients; i++)
	{
		if (i == g_pEngine->GetLocalPlayer())
			continue;

		auto player = get_entity(i);
		if (!player)
			continue;

		player->get_spotted() = true;
	}
}

void misc::aspectratio()
{
	if (!vars.misc.aspectratio.get<bool>())
		return;
	_(r_aspectratio_s, "r_aspectratio");
	static auto spermanagolove = g_pCVar->FindVar(r_aspectratio_s);
	*(int*)((DWORD)&spermanagolove->fnChangeCallbacks + 0xC) = 0;
	spermanagolove->set_value(vars.misc.aspectratiosize.get<float>() / 10.f);
}

void misc::reveal_ranks()
{
	if (!vars.misc.reveal_ranks.get<bool>() || !(g_cmd->buttons & IN_SCORE))
		return;

	static auto reveal_ranks = reinterpret_cast<bool(__cdecl*)(float*)>(sig("client.dll", "55 8B EC 8B 0D ? ? ? ? 68"));
	float array[3] = { 0,0,0 };

	reveal_ranks(array);
}

void misc::automaticpeek()
{
	static bool returning, placed;

	if (!g_pLocalPlayer || !g_pLocalPlayer->get_alive())
	{
		if (!misc::get().g_autopeek_pos.IsZero())
			misc::get().g_autopeek_pos.Zero();

		returning = false;

		return;
	}

	if (!vars.misc.autopeek.get<bool>() || !GetAsyncKeyState(vars.key.autopeek.get<int>()))
	{
		returning = false;

		if (!misc::get().g_autopeek_pos.IsZero())
			misc::get().g_autopeek_pos.Zero();

		return;
	}

	float die_time = prediction::get().get_curtime();
	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon) return;
	bool revolver_shoot = weapon->get_item_definiton_index() == WEAPON_REVOLVER && !g_revolver_fire && (g_cmd->buttons & IN_ATTACK || g_cmd->buttons & IN_ATTACK2);

	// make sure you shot correctly here
	if ((g_cmd->buttons & IN_ATTACK && weapon->get_item_definiton_index() != WEAPON_REVOLVER || revolver_shoot) && !returning)
		returning = true;

	// draw your beautiful 3d filled circle now
	if (misc::get().g_autopeek_pos.IsZero())
	{
		misc::get().g_autopeek_pos = g_pLocalPlayer->get_abs_origin();
		++die_time;
	}

	auto dLight = g_pEffects->CL_AllocDlight(g_pLocalPlayer->EntIndex());
	dLight->radius = vars.misc.autopeek_radius.get<float>();
	dLight->die += die_time;
	dLight->color = returning ? Color(10, 255, 10, 5) : Color(255, 10, 10, 5);

	dLight->key = g_pLocalPlayer->EntIndex();
	dLight->decay = dLight->radius / 5.0f;
	dLight->origin = misc::get().g_autopeek_pos + Vector(0, 0, 2);

	if (returning)
	{
		if (misc::get().g_autopeek_pos.IsZero())
		{
			returning = false;
			--die_time;
			return;
		}

		Vector nOrigin = g_pLocalPlayer->get_abs_origin() - misc::get().g_autopeek_pos;

		// *;*
		if (nOrigin.Length2D() < 5.f)
		{
			returning = false;
			return;
		}

		auto velocity = 
			Vector(nOrigin.x * cos(misc::get().g_real_angles.y / 180.0f * M_PI) + nOrigin.y * sin(misc::get().g_real_angles.y / 180.0f * M_PI), 
			nOrigin.y * cos(misc::get().g_real_angles.y / 180.0f * M_PI) - nOrigin.x *sin(misc::get().g_real_angles.y / 180.0f * M_PI), 
			nOrigin.z);

		// alright, lets go
		g_cmd->forwardmove = -velocity.x * 20.f;
		g_cmd->sidemove = velocity.y * 20.f;
	}
}

void misc::soundesp(const int idx, const char* sample, const Vector& origin)
{
	if (INRANGE(idx, 1, g_pGlobals->maxClients))
	{
		auto player = get_entity(idx);

		if (!player || !player->get_alive() || player == g_pLocalPlayer || player->GetClientClass()->m_ClassID != 35 || !player->IsDormant())
			return;

		_(footsteps, "player/footsteps");
		_(land, "player/land");

		if (!strstr(sample, footsteps) && !strstr(sample, land))
			return;

		player->get_origin() = origin;
		player->get_oldorigin() = origin;
		player->set_abs_origin(origin);
	}
}

void misc::clan_tag()
{
	const auto setclantag = [](const char* tag)
	{
		static auto set_clantag_fn = reinterpret_cast<int(__fastcall*)(const char*, const char*)>(sig("engine.dll", "53 56 57 8B DA 8B F9 FF 15"));

		set_clantag_fn(tag, tag);
	};

	static auto was_on = false;

	if (!vars.misc.clantag.get<bool>())
	{
		if (was_on)
			setclantag("");
		was_on = false;
		return;
	}

	was_on = true;

	static int prevtime;
	static int prevval;

	if (int(g_pGlobals->curtime * 3.1) != prevtime)
	{
		if (prevval != int(g_pGlobals->curtime * 3.1) % 31)
		{
			prevval = int(g_pGlobals->curtime * 3.1) % 31;
			switch (int(g_pGlobals->curtime * 3.1) % 29)
			{
			case 29: { _(s29, "BloodNET   "); setclantag(s29); break; }
			case 23: { _(s23, " BloodNET  "); setclantag(s23); break; }
			case 22: { _(s22, "  BloodNET "); setclantag(s22); break; }
			case 21: { _(s21, "   BloodNET"); setclantag(s21); break; }
			case 20: { _(s20, "    BloodNE"); setclantag(s20); break; }
			case 19: { _(s19, "     BloodN"); setclantag(s19); break; }
			case 18: { _(s18, "      Blood"); setclantag(s18); break; }
			case 17: { _(s17, "       Bloo"); setclantag(s17); break; }
			case 16: { _(s16, "        Blo"); setclantag(s16); break; }
			case 15: { _(s15, "         Bl"); setclantag(s15); break; }
			case 14: { _(s14, "          B"); setclantag(s14); break; }
			case 13: { _(s13, "              "); setclantag(s13); break; }
			case 12: { _(s12, "              "); setclantag(s12); break; }
			case 11: { _(s11, "              "); setclantag(s11); break; }
			case 10: { _(s10, "              "); setclantag(s10); break; }
			case 9: { _(s9,   "T             "); setclantag(s9); break; }
			case 8: { _(s8,   "ET            "); setclantag(s8); break; }
			case 7: { _(s7,   "NET           "); setclantag(s7); break; }
			case 6: { _(s6,   "dNET          "); setclantag(s6); break; }
			case 5: { _(s5,   "odNET         "); setclantag(s5); break; }
			case 4: { _(s4,   "oodNET        "); setclantag(s4); break; }
			case 3: { _(s3,   "loodNET       "); setclantag(s3); break; }
			case 2: { _(s2,   "BloodNET      "); setclantag(s2); break; }
			case 1: { _(s1,   " BloodNET     "); setclantag(s1); break; }
			case 0: { _(s0,   "  BloodNET    "); setclantag(s0); break; }
			default:;
			}
		}
	}

	prevtime = int(g_pGlobals->curtime);
}

void misc::play_freezetime_music()
{
	_(techno_overpass, "ambient/misc/techno_overpass.wav");

	static float time;
	if (*g_ppGameRules && (*g_ppGameRules)->m_bFreezePeriod())
	{
		if (!time)
			g_pSurface->PlaySound(techno_overpass);

		time += g_pGlobals->frametime;

		/*if ( time >= 6.f )
			time = 0.f;*/
	}
	else
		time = 0.f;
}
