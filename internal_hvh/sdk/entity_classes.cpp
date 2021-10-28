#include "../include_cheat.h"

inline bool invalidChar( char c )
{
	return !( c >= 0 && c < 128 );
}
inline void stripUnicode( std::string & str )
{
	str.erase( remove_if( str.begin(), str.end(), invalidChar ), str.end() );
}

model_t* C_BaseEntity::get_model() { return *reinterpret_cast< model_t** > ( reinterpret_cast< uintptr_t >( this ) + 0x6C ); } // CRASH
IClientRenderable * C_BaseEntity::get_renderable() { return  reinterpret_cast< IClientRenderable* > ( reinterpret_cast< uintptr_t >( this ) + 0x4 ); };

bool C_BasePlayer::get_alive() { return this->get_lifestate() == LifeState::ALIVE; }
Vector C_BasePlayer::get_eye_pos()
{
	Vector out;
	util::get_vfunc<void( __thiscall* )( void*, Vector& )>( this, 277 )( this, out );
	return out;
}

void C_BasePlayer::draw_hitboxes(lag_record_t& record, matrix3x4_t* matrix, Color col, float time) const
{
	if (!g_pLocalPlayer || !g_pLocalPlayer->get_alive())
		return;

	if (!record.valid)
		return;

	const auto player = get_entity(record.index);

	if (!player)
		return;

	const model_t* model = player->get_model();

	if (!model)
		return;

	const auto hdr = g_pModelInfo->GetStudioModel(model);

	if (!hdr)
		return;

	const auto set = hdr->pHitboxSet(0);

	if (!set)
		return;

	for (auto i = 0; i < set->numhitboxes; i++) {
		const auto hitbox = set->pHitbox(i);

		if (!hitbox)
			continue;

		if (hitbox->radius <= 0.f)
		{
			// https://developer.valvesoftware.com/wiki/Rotation_Tutorial

			// convert rotation angle to a matrix.
			matrix3x4_t rot_matrix;
			math::get().angle_matrix(hitbox->rotation, rot_matrix);

			// apply the rotation to the entity input space (local).
			matrix3x4_t matrix;
			math::get().concat_transforms(matrix[hitbox->bone], rot_matrix, matrix);

			// extract the compound rotation as an angle.
			QAngle bbox_angle;
			math::get().martrix_angles(matrix, bbox_angle);

			// extract hitbox origin.
			Vector origin = matrix.GetOrigin();

			// draw box.
			g_pDebugOverlay->AddBoxOverlay(origin, hitbox->bbmin, hitbox->bbmax, bbox_angle, col.r(), col.g(), col.b(), 0, time);
		}
		else
		{
			Vector min, max;
			math::vector_transform(hitbox->bbmin, matrix[hitbox->bone], min);
			math::vector_transform(hitbox->bbmax, matrix[hitbox->bone], max);

			g_pDebugOverlay->AddCapsuleOverlay(min, max, hitbox->radius, col.r(), col.g(), col.b(), col.a(), time);
		}
	}
}

bool C_BasePlayer::is_enemy() { return this->get_team() != g_pLocalPlayer->get_team(); }
player_info_t C_BasePlayer::get_player_info() const { player_info_t pinfo; g_pEngine->GetPlayerInfo( this->EntIndex(), &pinfo ); return pinfo; }
std::string C_BasePlayer::get_name() const { auto name = std::string( this->get_player_info().name ); stripUnicode( name ); return name; }
bool C_BaseCombatWeapon::is_grenade() { if ( !this ) return false; return this->get_weapon_id() >= WEAPON_FLASHBANG && this->get_weapon_id() <= WEAPON_INCGRENADE; }
int C_BaseCombatWeapon::get_weapon_id() { if ( !this ) return -1; return this->get_item_definiton_index(); }
int C_BaseCombatWeapon::get_weapon_type() { if ( !this ) return -1; return this->get_wpn_data()->WeaponType; }
CCSWeaponData* C_BaseCombatWeapon::get_wpn_data_safe()
{
	if ( !this ) return nullptr; return g_pWeaponSystem->GetWpnData( this->get_item_definiton_index() );
}

C_CSPlayer * C_BasePlayer::get_observer()
{
	C_CSPlayer* observer = nullptr;
	const auto spec_handle = this->get_observer_target();
	if ( spec_handle )
		observer = reinterpret_cast< C_CSPlayer* >( g_pEntitiyList->GetClientEntityFromHandle( spec_handle ) );

	return observer;
}

bool C_CSPlayer::setup_bones( lag_record_t& record, int boneMask, matrix3x4_t * pBoneToWorldOut )
{
	alignas( 16 ) matrix3x4_t bone_out[ 128 ];
	const auto ret = bone_setup::get().handle_bone_setup( record, boneMask, bone_out );
	memcpy( pBoneToWorldOut, bone_out, sizeof( matrix3x4_t[ 128 ] ) );
	return ret;
}

bool C_BaseCombatWeapon::is_being_thrown()
{
	if ( this->is_grenade() )
	{
		if ( !this->get_pin_pulled() )
		{
			const auto f_throw_time = this->get_throw_time();

			if ( ( f_throw_time > 0 ) && ( f_throw_time < /*prediction::get().get_unpred_curtime()))*/ g_pGlobals->curtime ) )
				return true;
		}
	}

	return false;
}

void C_CSPlayer::set_anim_layers( std::array<C_AnimationLayer, 15>& layers )
{
	auto this_layers = this->get_anim_layers();
	std::copy( std::begin( this_layers ), std::end( this_layers ), std::begin( layers ) );
}

void C_CSPlayer::set_pose_params( std::array<float, 24>& params )
{
	auto this_params = this->get_pose_params();
	std::copy( std::begin( this_params ), std::end( this_params ), std::begin( params ) );
}

void C_CSPlayer::set_pose_params_scaled( std::array<float, 24>& params )
{
	typedef bool( __thiscall* GetPoseParameterRange_t )( void*, int, float&, float& );
	static auto GetPoseParameterRange = reinterpret_cast< GetPoseParameterRange_t >( sig( "client.dll", "55 8B EC 56 8B F1 83 BE ? ? ? ? ? 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 ? ? ? ? 8B 8E ? ? ? ? 85 C9 74 49 83 39 00 74 44 8B 55 08" ) );

	std::array<float, 24> unscaled_params;
	for ( auto i = 0; i < 24; i++ )
	{
		auto min = 0.0f, max = 0.0f;
		if ( !GetPoseParameterRange( this, params[ i ], min, max ) )
			continue;

		const auto value = ( 1.0 - params[ i ] ) * min + params[ i ] * max;

		unscaled_params[ i ] = value;
	}

	for ( auto i = 0; i < 24; i++ )
		this->set_pose_param( i, unscaled_params[ i ] );
}

void C_CSPlayer::set_pose_param( int param, float value )
{
	static auto Studio_SetPoseParameter = reinterpret_cast< void* >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 08 F3 0F 11 54 24 ? 85 D2" ) );

	auto result = 0.0f;
	auto hdr = this->get_model_ptr();

	__asm {
		pushad
		movss xmm2, [ value ]
		lea eax, [ result ]
		push eax
		mov edx, param
		mov ecx, hdr
		call Studio_SetPoseParameter
		pop eax
		popad
	}

	this->get_pose_params()[ param ] = result;
}

int C_CSPlayer::get_sequence_act( int sequence )
{
	const auto model = this->GetModel();
	if ( !model )
		return -1;

	const auto hdr = g_pModelInfo->GetStudioModel( model );
	if ( !hdr )
		return -1;

	static auto get_sequence_act = reinterpret_cast< int( __fastcall* )( void*, studiohdr_t*, int ) >( offsets::get_sequence_act );
	return get_sequence_act( this, hdr, sequence );
}

CUserCmd*& C_CSPlayer::m_pCurrentCommand()
{
	static auto current_command = *(uint32_t*)/*reinterpret_cast<uintptr_t*>*/(offsets::current_command);

	return *(CUserCmd**)((uintptr_t)this + current_command);
}

Vector C_CSPlayer::get_shoot_pos()
{
	Vector eye_pos;
	util::get_vfunc<void(__thiscall*)(void*, Vector&)>(this, 0xa3)(this, eye_pos);
	if (*reinterpret_cast <int32_t*> (uintptr_t(this) + 0x39E1))
	{
		auto state = this->get_anim_state();
		auto entity = (C_CSPlayer*)state->pBasePlayer;
		if (state)
		{
			if (entity && (state->m_bInHitGroundAnimation || entity->get_duck_amt() != 0.f || !entity->get_ground_entity()))
			{
				auto v5 = 8;

				if (v5 != -1 && entity->get_bone_cache()->m_pCachedBones) 
				{
					Vector head_pos(
						entity->get_bone_cache()->m_pCachedBones[8][0][3],
						entity->get_bone_cache()->m_pCachedBones[8][1][3],
						entity->get_bone_cache()->m_pCachedBones[8][2][3]);

					if (eye_pos.z > head_pos.z + 1.7)
					{
						float v13 = 0.f;
						float v3 = (eye_pos).z - head_pos.z + 1.7;

						float v4 = (v3 - 4.f) * 0.16666667;
						if (v4 >= 0.f)
							v13 = std::fminf(v4, 1.f);

						(eye_pos).z = (((head_pos.z + 1.7 - (eye_pos).z)) * (((v13 * v13) * 3.0) - (((v13 * v13) * 2.0) * v13))) + (eye_pos).z;
					}
				}
			}
		}
	}

	return eye_pos;
}