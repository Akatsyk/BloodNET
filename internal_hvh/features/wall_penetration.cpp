#include "../include_cheat.h"

bool apply_breakables_fix( IClientEntity* entity )
{
	ClientClass *pClass = entity->GetClientClass();

	if ( ( pClass->m_pNetworkName[ 1 ] == 'B' && pClass->m_pNetworkName[ 9 ] == 'e' && pClass->m_pNetworkName[ 10 ] == 'S' && pClass->m_pNetworkName[ 16 ] == 'e' )
		 || ( pClass->m_pNetworkName[ 1 ] != 'B' || pClass->m_pNetworkName[ 5 ] != 'D' ) )
		 ( ( C_CSPlayer* )entity )->get_take_damage() = DAMAGE_YES;

	return true;
}

bool is_cfuncbrush( IClientEntity* entity )
{
	if ( !entity || entity->EntIndex() == 0 )
		return false;


	ClientClass *pClass = entity->GetClientClass();

	if ( !pClass )
		return false;

	_( CFuncBrush, "CFuncBrush" );
	return strstr( pClass->m_pNetworkName, CFuncBrush ) != nullptr;
}

////////////////////////////////////// Misc Functions //////////////////////////////////////
bool penetration::is_breakable_entity( IClientEntity* entity )
{
	using Fn = bool(__fastcall*)(IClientEntity*);
	static auto fn = reinterpret_cast<Fn>(sig("client.dll", "55 8B EC 51 56 8B F1 85 F6 74 68 83 BE"));

	if (!entity || !entity->EntIndex())
		return false;

	auto take_damage{ (char*)((uintptr_t)entity + *(size_t*)((uintptr_t)fn + 0x26)) };
	auto take_damage_backup{ *take_damage };

	ClientClass* pClass = entity->GetClientClass();

	if (pClass->m_pNetworkName == "CBreakableSurface")
		*take_damage = DAMAGE_YES;
	else if (pClass->m_pNetworkName == "CBaseDoor" || pClass->m_pNetworkName == "CDynamicProp")
		*take_damage = DAMAGE_NO;

	bool breakable = fn(entity);
	*take_damage = take_damage_backup;

	return breakable;
}

void penetration::scale_damage(C_CSPlayer* ent, CGameTrace& enterTrace, CCSWeaponData* weaponData, float& currentDamage)
{
	// cred. to N0xius for reversing this.
	bool hasHeavyArmor = ent->get_heavy_armor();
	int armorValue = ent->get_armor();
	int hitGroup = enterTrace.hitgroup;

	auto IsArmored = [&]()->bool
	{
		switch (hitGroup)
		{
		case HITGROUP_HEAD:
			return !!ent->get_helmet(); // !! нужны чтобы компилятор меня нахуй не послал.
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	// фикс авола для серверов где эта поебота изменена.
	__(mp_damage_scale_t_head_s, "mp_damage_scale_t_head");
	__(mp_damage_scale_t_body_s, "mp_damage_scale_t_body");
	__(mp_damage_scale_ct_head_s, "mp_damage_scale_ct_head");
	__(mp_damage_scale_ct_body_s, "mp_damage_scale_ct_body");

	static auto mp_damage_scale_t_head = g_pCVar->FindVar(mp_damage_scale_t_head_s);
	static auto mp_damage_scale_t_body = g_pCVar->FindVar(mp_damage_scale_t_body_s);
	static auto mp_damage_scale_ct_head = g_pCVar->FindVar(mp_damage_scale_ct_head_s);
	static auto mp_damage_scale_ct_body = g_pCVar->FindVar(mp_damage_scale_ct_body_s);

	auto head_scale = ent->get_team() == 3 ? mp_damage_scale_ct_head->get_float() : mp_damage_scale_t_head->get_float();
	auto body_scale = ent->get_team() == 3 ? mp_damage_scale_ct_body->get_float() : mp_damage_scale_t_body->get_float();

	if (hasHeavyArmor)
		head_scale *= .5f;

	switch (hitGroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= 4.0f * head_scale;
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f * body_scale;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
	case HITGROUP_GEAR:
		currentDamage *= body_scale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= .75f * body_scale;
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = .5f, armorRatio = weaponData->flArmorRatio * .5f;

		if (hasHeavyArmor)
		{
			armorBonusRatio = .33f;
			armorRatio *= .5f;
			bonusValue = .33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= .85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}

////////////////////////////////////// Main Autowall Functions //////////////////////////////////////
bool penetration::trace_to_exit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction)
{
	Vector start, end;
	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		// add extra distance to our ray.
		currentDistance += rayExtension;

		// multiply the direction vector to the distance so we go outwards, add our position to it.
		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = g_pTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		int pointContents = g_pTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents)
		{
			// let's setup our end position by deducting the direction by the extra added distance.
			end = start - (direction * rayExtension);

			// let's cast a ray from our start pos to the end pos.
			util_trace_line(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			// let's check if a hitbox is in-front of our enemy and if they are behind a solid wall.
			if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				util_trace_line(start, startPosition, MASK_SHOT_HULL, exitTrace.m_pEnt, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			// can we hit? is the wall solid?
			if (exitTrace.DidHit() && !exitTrace.startsolid)
			{

				// is the wall a breakable? if so, let's shoot through it.
				if (is_breakable_entity(enterTrace.m_pEnt) && is_breakable_entity(exitTrace.m_pEnt))
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.startsolid)
			{
				if (enterTrace.DidHitNonWorldEntity() && is_breakable_entity(enterTrace.m_pEnt))
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}
			}
		}
	}
	return false;
}
bool penetration::handle_bullet_penetration(CCSWeaponData* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, float dist, Vector start)
{
	CGameTrace exitTrace;
	C_CSPlayer* pEnemy = (C_CSPlayer*)enterTrace.m_pEnt;
	surfacedata_t* enterSurfaceData = g_pSurfaceProps->GetSurfaceData(enterTrace.surface.surfaceProps);
	int enterMaterial = enterSurfaceData->game.material;

	float enterSurfPenetrationModifier = enterSurfaceData->game.penetrationmodifier;
	float enterDamageModifier = enterSurfaceData->game.damagemodifier;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

	if (possibleHitsRemaining <= 0
		// test for "DE_CACHE/DE_CACHE_TELA_03" as the entering surface and "CS_ITALY/CR_MISCWOOD2B" as the exiting surface.
		// fixes a wall in de_cache which seems to be broken in some way. Although bullet penetration is not recorded to go through this wall.
		// decals can be seen of bullets within the glass behind of the enemy. Hacky method, but works.
		// you might want to have a check for this to only be activated on de_cache.
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
		|| weaponData->flPenetration <= 0.f
		|| !trace_to_exit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(g_pTrace->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;

	surfacedata_t* exitSurfaceData = g_pSurfaceProps->GetSurfaceData(exitTrace.surface.surfaceProps);
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.penetrationmodifier;
	float exitDamageModifier = exitSurfaceData->game.damagemodifier;

	if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
	{
		combinedPenetrationModifier = 3.f;
		finalDamageModifier = .05f;
	}
	else if (isSolidSurf || isLightSurf)
	{
		combinedPenetrationModifier = 1.f;
		finalDamageModifier = .16f;
	}
	else if (enterMaterial == CHAR_TEX_FLESH && (!pEnemy->is_enemy() && ff_damage_reduction_bullets == 0.f))
	{
		// look's like you aren't shooting through your teammate today.
		if (ff_damage_bullet_penetration == 0.f)
			return false;

		// let's shoot through teammates and get kicked for teamdmg! whatever, atleast we did damage to the enemy. i call that a win.
		combinedPenetrationModifier = ff_damage_bullet_penetration;
		finalDamageModifier = .16f;
	}
	else
	{
		combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) * .5f;
		finalDamageModifier = .16f;
	}

	// do our materials line up?
	if (enterMaterial == exitMaterial)
	{
		if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
			combinedPenetrationModifier = 3.f;
		else if (exitMaterial == CHAR_TEX_PLASTIC)
			combinedPenetrationModifier = 2.f;
	}

	// calculate thickness of the wall by getting the length of the range of the trace and squaring.
	thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
	modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier.
	lostDamage = fmaxf(
		((modifier * thickness) / 24.f)
		+ ((currentDamage * finalDamageModifier)
			+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

	// did we loose too much damage?
	if (lostDamage > currentDamage)
		return false;

	// we can't use any of the damage that we've lost.
	if (lostDamage > 0.f)
		currentDamage -= lostDamage;

	// do we still have enough damage to deal?
	if (currentDamage < 1.f)
		return false;

	eyePosition = exitTrace.endpos;
	--possibleHitsRemaining;

	return true;
}

bool penetration::fire_bullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, const Vector pos)
{
	if (!pWeapon)
		return false;

	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration, rayExtension = 40.f;
	Vector eyePosition = pos;

	__(ff_damage_reduction_bullets_s, "ff_damage_reduction_bullets");
	static ConVar* damageReductionBullets = g_pCVar->FindVar(ff_damage_reduction_bullets_s);
	__(ff_damage_bullet_penetration_s, "ff_damage_bullet_penetration");
	static ConVar* damageBulletPenetration = g_pCVar->FindVar(ff_damage_bullet_penetration_s);
	ff_damage_reduction_bullets = damageReductionBullets->get_float();
	ff_damage_bullet_penetration = damageBulletPenetration->get_float();

	CCSWeaponData* weaponData = pWeapon->get_wpn_data();
	CGameTrace enterTrace;

	// we should be skipping g_pLocalPlayer when casting a ray to players.
	CTraceFilter filter;
	filter.pSkip = g_pLocalPlayer;

	if (!weaponData)
		return false;

	maxRange = weaponData->flRange;
	penetrationDistance = 3000.0f;
	penetrationPower = weaponData->flPenetration;

	// габен сказал - "The total number of surfaces any bullet can penetrate in a single flight is capped at 4."
	int possibleHitsRemaining = vars.aa.freestanding_fps.get<bool>() ? 2 : 4;

	// set our current damage to what our gun's initial damage reports it will do.
	currentDamage = weaponData->iDamage;

	// if our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		// calculate max bullet range.
		maxRange -= currentDistance;

		// create endpoint of bullet.
		Vector end = eyePosition + direction * maxRange;

		util_trace_line(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, g_pLocalPlayer, &enterTrace);
		g_pTrace->clip_ray_to_player(eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &filter, &enterTrace);

		// we have to do this *after* tracing to the player.
		surfacedata_t* enterSurfaceData = g_pSurfaceProps->GetSurfaceData(enterTrace.surface.surfaceProps);
		float enterSurfPenetrationModifier = enterSurfaceData->game.penetrationmodifier;
		int enterMaterial = enterSurfaceData->game.material;

		// если никуда не попали - нахуй продолжать чет считать?
		if (enterTrace.fraction == 1.f)
			break;

		// calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * maxRange;

		// let's make our damage drops off the further away the bullet is.
		currentDamage *= pow(weaponData->flRangeModifier, (currentDistance / 500.f));

		// sanity checking / can we actually shoot through?
		if (currentDistance > penetrationDistance && weaponData->flPenetration > 0.f || enterSurfPenetrationModifier < .1f)
			break;

		// this looks gay as fuck if we put it into 1 long line of code.
		bool canDoDamage = (enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC);
		bool isPlayer = ((enterTrace.m_pEnt)->GetClientClass()->m_ClassID == ClassId_CCSPlayer);
		bool isEnemy = ((C_CSPlayer*)enterTrace.m_pEnt)->is_enemy();

		if (canDoDamage && isPlayer && isEnemy)
		{
			scale_damage((C_CSPlayer*)enterTrace.m_pEnt, enterTrace, weaponData, currentDamage);
			return true;
		}

		if (isPlayer && (C_CSPlayer*)enterTrace.m_pEnt == g_pLocalPlayer)
		{
			enterTrace.hitgroup = 1;
			return true;
		}

		// calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!handle_bullet_penetration(weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, true, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}
	return false;
}

////////////////////////////////////// Usage Calls //////////////////////////////////////
bool penetration::can_hit(Vector& point, float& damage, const Vector pos)
{
	QAngle angles;
	Vector direction;
	Vector tmp = point - pos;

	math::get().vector_angles(tmp, angles);
	math::get().angle_vectors(angles, &direction);
	direction.NormalizeInPlace();

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	const auto ret = fire_bullet(weapon, direction, damage, pos);

	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////
bool penetration::get_damage( C_CSPlayer* shooter, C_CSPlayer* target, Vector end, float* damage, CCSWeaponData* custom_weapon, Vector* start_pos, matrix3x4_t* matrix )
{
	if ( !g_pLocalPlayer )
		return false;

	*damage = 0.f;

	if ( !shooter || !target )
		return false;

	auto weapon = get_weapon( shooter->get_active_weapon() );
	if ( !weapon )
		return false;

	auto weapon_data = weapon->get_wpn_data();
	if ( !weapon_data )
		return false;

	g_in_hbp = true;

	bool ultra_hit = !!custom_weapon;

	FireBulletData data;
	data.wpn_data = ultra_hit ? custom_weapon : weapon_data;
	if ( !data.wpn_data )
		return false;
	data.src = shooter->get_eye_pos();
	if ( start_pos )
	{
		data.src = *start_pos;
	}
	data.filter.pSkip = shooter;
	data.length_to_end = ( end - data.src ).Length();

	QAngle angles = math::get().calc_angle( data.src, end );
	math::get().angle_vectors( angles, &data.direction );

	data.direction.NormalizeInPlace();


	if ( simulate_bullet_fire_ultrahit( shooter, target, weapon, data, end ) )
	{
		*damage = data.current_damage;
		g_in_hbp = false;
		return true;
	}

	*damage = 0.f;
	g_in_hbp = false;
	return false;
}

bool penetration::simulate_bullet_fire_ultrahit( C_CSPlayer* shooter, C_CSPlayer* target, C_BaseCombatWeapon* weapon, FireBulletData& data,
												 const Vector& endpos )
{
	if ( !shooter || !target || !weapon )
		return false;


	data.penetrate_count = vars.aa.freestanding_fps.get<bool>() ? 2 : 4;
	data.trace_length = 0.0f;

	data.current_damage = static_cast< float >( data.wpn_data->iDamage );

	__( ff_damage_reduction_bullets_s, "ff_damage_reduction_bullets" );
	static ConVar* damageReductionBullets = g_pCVar->FindVar( ff_damage_reduction_bullets_s );
	__( ff_damage_bullet_penetration_s, "ff_damage_bullet_penetration" );
	static ConVar* damageBulletPenetration = g_pCVar->FindVar( ff_damage_bullet_penetration_s );
	auto ff_damage_reduction_bullets = damageReductionBullets->get_float();
	auto ff_damage_bullet_penetration = damageBulletPenetration->get_float();

	QAngle angles;
	Vector direction;
	Vector tmp = data.src - endpos;

	math::get().vector_angles( tmp, angles );
	math::get().angle_vectors( angles, &direction );
	direction.NormalizeInPlace();

	auto start = data.src;

	while ( data.penetrate_count > 0 && data.current_damage >= 1.f )
	{
		// 8192.f = max weapon range (awp and such)
		data.trace_length_remaining = 8192.f - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		util_trace_line( data.src, end, MASK_SHOT_HULL, shooter, &data.enter_trace );

		bool cond1 = data.enter_trace.fraction == 1.0f;
		// this is better than checking the fraction (its unreliable)
		bool cond2 = data.enter_trace.m_pEnt == target;

		bool cond3 = data.enter_trace.fraction * data.trace_length_remaining > data.length_to_end;

		if ( cond1 || cond2 || cond3 )
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= powf( data.wpn_data->flRangeModifier, data.trace_length * 0.002f );

			scale_damage( target, data.enter_trace, data.wpn_data, data.current_damage );
			return true;
		}

		static DWORD dwOffset = 0;
		if ( !dwOffset )
		{
			dwOffset = sig( "client.dll", "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 78 8B 53 14" );
		}

		if ( dwOffset )
		{
			surfacedata_t* enter_surface = g_pSurfaceProps->GetSurfaceData( data.enter_trace.surface.surfaceProps );
			if ( !enter_surface )
				return false;

			int flag = ( data.enter_trace.contents >> 3 ) & 1;
			int enter_material = enter_surface->game.material;
			int mask = 0x1002;

			using HandleBulletPenetrationFn = bool( __thiscall* )( C_BaseEntity*, float&, int&, int*, trace_t*, Vector*, float, float, float, int, int, float, int*, Vector*, float, float, float* );
			static HandleBulletPenetrationFn handle_bullet_pen = reinterpret_cast< HandleBulletPenetrationFn >( dwOffset );

			if ( data.enter_trace.m_pEnt )
			{
				if ( is_cfuncbrush( data.enter_trace.m_pEnt ) )
				{
					data.penetrate_count++;
				}
			}

			uint8_t* take_damage = reinterpret_cast< uint8_t* >( uintptr_t( data.enter_trace.m_pEnt ) + 0x27C );
			uint8_t old_dmg = *take_damage;
			if ( data.enter_trace.m_pEnt )
			{
				apply_breakables_fix( data.enter_trace.m_pEnt );
			}

			float pen = data.wpn_data->flPenetration;
			bool bRetVal = handle_bullet_penetration( data.wpn_data, data.enter_trace, data.src, direction, data.penetrate_count, data.current_damage, data.wpn_data->flPenetration, true, ff_damage_reduction_bullets, ff_damage_bullet_penetration, data.length_to_end, start );

			*take_damage = old_dmg;

			if ( !bRetVal )
				break;
		}
	}

	return false;
}