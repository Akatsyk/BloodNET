#pragma once


struct fire_bullet_data
{
	Vector						src;
	trace_t						enter_trace;
	Vector						direction;
	ITraceFilter				filter;
	float						trace_length;
	float						trace_length_remaining;
	float						current_damage;
	int							penetrate_count;
};

class penetration : public singleton<penetration>
{
public:
	bool is_breakable_entity(IClientEntity* entity);
	void scale_damage(C_CSPlayer* ent, CGameTrace& enterTrace, CCSWeaponData* weaponData, float& currentDamage);
	bool trace_to_exit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction);
	bool handle_bullet_penetration(CCSWeaponData* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, float dist = 0.f, Vector start = { 0.f,0.f,0.f });
	bool can_hit(Vector& point, float& damage, const Vector pos = g_pLocalPlayer->get_eye_pos());
	bool fire_bullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, Vector pos = { 0.f,0.f,0.f });
	bool simulate_bullet_fire_ultrahit(C_CSPlayer* shooter, C_CSPlayer* target, C_BaseCombatWeapon* weapon, FireBulletData& data, const Vector& endpos);
	bool get_damage(C_CSPlayer* shooter, C_CSPlayer* target, Vector end, float* damage, CCSWeaponData* custom_weapon, Vector* start_pos, matrix3x4_t* matrix = nullptr);
};
