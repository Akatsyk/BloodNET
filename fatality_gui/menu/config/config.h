#pragma once

#include "tsl/robin_map.h"

class value
{
public:
	template <typename T = float>
	T get() { return static_cast< T >( m_value ); }

	template <typename T = float>
	void set( T in ) { m_value = static_cast< double >( in ); }

private:
	double m_value = 0.0;
};

class c_vars
{
public:

	struct
	{
		value enabled;
		value autofire;
		value baim_on_key;
		value backtrack;
		value fakelag_compensation;
		value ping_spike;
		value ping_spike_amt;
		value ping_spike_base;
		value ping_spike_base_amt;
		value resolver;
		value resolve_team;
		value knife_bot;
		value zeus_bot;
		value override;
		value override_damage;
		value hsonly;
		value zeus_hc;
		value aimbot_record;
	} aim;


	struct wconfig_types_t
	{
		value hitchance;
		value mindmg;
		//value ov_mindamage;
		value scale;
		value scale_head;
		value adaptive_scale;
		value autostop;
		value autostop_on_key;
		value autostop_lethal;
		value baim_mode;
		value autorevolver;
		value scout_jumpshot;
		value autoscope;
		struct
		{
			value air;
			value resolver_unsure;
			value lethal;
		}baim;
		struct
		{
			value head;
			value neck;
			value pelvis;
			value body;
			value thorax;
			value chest;
			value upperchest;
			value thighs;
			value calfs;
			value foots;
			value hands;
			value upperarms;
			value forearms;
		}hitscan;
		struct
		{
			value head;
			value neck;
			value pelvis;
			value body;
			value thorax;
			value chest;
			value upperchest;
			value thighs;
			value calfs;
			value foots;
			value hands;
			value upperarms;
			value forearms;
		}hitscan_override;
	};

	struct
	{
		wconfig_types_t autosniper, scout, awp, heavy_pistol, pistol, other;
	} config;

	struct aa_types_t
	{
		value enabled;
		value spin;
		value spin_speed;
		value spin_range;
		value jitter;
		value jitter_range;
		value jitter_speed;
		value jitter_random;
		value lby;
		value lby_static;
		value lby_autodirection;
		value at_target;
		value edge;
		value freestand;
		value add;
		value choke;
		value lby_delta_first;
		value lby_delta;
		value lby_random;
		value lby_random_add;
		value lby_random_range;
		value lby_when_hittable;
		value lby_delta_when_hittable;
	};

	struct
	{
		value enabled;
		value fakelag_when_disabled;
		value fakewalk;
		value fakeduck;
		value fakemove_ticks;
		value prevent_onshot;
		value on_peek;
		value fs_dormant_timeout;
		value manual;
		value indicator;
		value indicator_col;
		value choke_reload;
		value choke_weapon_switch;
		value choke_shot;
		value freestanding_fps;
		value fake_angles;

		aa_types_t regular, moving, air;
	} aa;

	struct
	{
		value autostop;
		value override_damage;
		value autopeek;
		value baim;
		value fakewalk;
		value fakeduck;
		value ping_spike;
		value thirdperson;
		value override;
		value aa_back;
		value aa_left;
		value aa_right;
	} key;

	struct
	{
		value bhop;
		value autopeek;
		value autopeek_radius;
		value fov_enabled;
		value fov_scope;
		value fov;
		value vm_fov_enabled;
		value vm_fov;
		value hitmarker;
		value hitsound_select;
		value aspectratio;
		value aspectratiosize;
		//value viewmodel_offset_x;
		//value viewmodel_offset_x_size;
		value grenade_trajectory;
		value grenade_trajectory_size;
		value radar;
		value remove_flash;
		value remove_scope;
		value remove_smoke;
		value autostrafe;
		value wasdstrafe;
		value doubletap;
		value thirdperson;
		value thirdperson_dead;
		value thirdperson_dist;
		value transparency_in_scope;
		value force_crosshair;
		value autoaccept;
		value reveal_ranks;
		value hurtlog;
		value buylog;
		value clantag;
		value preservedn;
		value impacts;
	} misc;

	struct
	{
		struct
		{
			value dormant_esp;
			value ammo;
			value ammo_color;
			value armor;
			value armor_color;
			value ping;
			value ping_color;
			value health;
			value name;
			value name_color;
			value weapon;
			value weapon_color;
			value lby_timer;
			value lby_timer_color;
			value scoped;
			value bomb;
			value box;
			value box_color;

			struct
			{
				value weapons;
				value grenades;
				value c4;
				value color;
			} world;
		} esp;

		struct
		{
			value enabled;
			struct
			{
				value enabled;
				value color;
			} enemy;

			struct
			{
				value enabled;
				value color;
			} team;

			struct
			{
				value enabled;
				value color;
			} local;

		} beams;
		struct
		{
			struct
			{
				value enabled;
				value type;
				value color;
				value hidden_color;
				value color_shine;
			} enemy;

			struct
			{
				value enabled;
				value type;
				value color;
				value hidden_color;
				value color_shine;
			} team;

			struct
			{
				value enabled;
				value type;
				value color;
				value color_shine;
			} local;

			struct
			{
				value enabled;
				value color;
			} history;

			value enabled;
			value extended;
			value hidden;
		} chams;

		struct
		{
			value enabled;
			struct
			{
				value enabled;
				value color;

			} enemy;

			struct
			{
				value enabled;
				value color;
			} team;

			struct
			{
				value enabled;
				value color;
			} local;

			struct
			{
				value weapons;
				value grenades;
				value c4;
				value color;
			} world;
		} glow;

		value dark;
		value prop_transparency;
		value spread_circle;
		value spread_circle_color1;
		value spread_circle_color2;
		value spread_circle_type;
		value out_of_fov;
		value out_of_fov_col;
		value out_of_fov_dst;
	} visuals;

	struct
	{
		value inventory_list_idx;
		value slot;
	} menu;

	struct
	{
		value definition_index;
		value paint_kit;
		value wear;
		value seed;
		value stat_trak;
	} inventory;

	struct
	{
		value auto_s;
		value ssg;
		value awp;
		value deagle;
		value dualies;
		value hegrenade;
		value molly;
		value smokegrenade;
		value flashbang;
		value flashbangs;
		value kit;
		value taser;
		value armor;
	} autobuy;
};

extern c_vars vars;

extern tsl::robin_map< std::string, value* > g_cfg;

struct antiaimconfig_t
{
	antiaimconfig_t( const int current )
	{
		index = current;

		c_vars::aa_types_t* config = nullptr;

		switch ( current )
		{
			case 0:
			{
				config = &vars.aa.regular;
				break;
			}
			case 1:
			{
				config = &vars.aa.moving;
				break;
			}
			case 2:
			{
				config = &vars.aa.air;
				break;
			}
		}

		enabled = &config->enabled;
		spin = &config->spin;
		spin_speed = &config->spin_speed;
		spin_range = &config->spin_range;
		jitter = &config->jitter;
		jitter_range = &config->jitter_range;
		jitter_speed = &config->jitter_speed;
		jitter_random = &config->jitter_random;
		at_target = &config->at_target;
		lby = &config->lby;
		lby_static = &config->lby_static;
		lby_autodirection = &config->lby_autodirection;
		edge = &config->edge;
		freestand = &config->freestand;
		add = &config->add;
		choke = &config->choke;
		lby_delta_first = &config->lby_delta_first;
		lby_delta = &config->lby_delta;
		lby_random = &config->lby_random;
		lby_random_add = &config->lby_random_add;
		lby_random_range = &config->lby_random_range;
	}

	value* enabled;
	value* spin;
	value* spin_speed;
	value* spin_range;
	value* jitter;
	value* jitter_range;
	value* jitter_speed;
	value* jitter_random;
	value* at_target;
	value* lby;
	value* lby_static;
	value* lby_autodirection;
	value* edge;
	value* freestand;
	value* add;
	value* choke;
	value* lby_delta_first;
	value* lby_delta;
	value* lby_random;
	value* lby_random_add;
	value* lby_random_range;
	int index;
};

struct weaponconfig_t
{
	weaponconfig_t( const int current )
	{
		index = current;

		c_vars::wconfig_types_t* config = nullptr;

		switch ( current )
		{
			case 0:
			{
				config = &vars.config.autosniper;
				break;
			}
			case 1:
			{
				config = &vars.config.scout;;
				break;
			}
			case 2:
			{
				config = &vars.config.awp;
				break;
			}
			case 3:
			{
				config = &vars.config.heavy_pistol;
				break;
			}
			case 4:
			{
				config = &vars.config.pistol;
				break;
			}
			case 5:
			{
				config = &vars.config.other;
				break;
			}
		}
		hitchance = &config->hitchance;
		mindmg = &config->mindmg;
		//ov_mindamage = &config->ov_mindamage;
		scale = &config->scale;
		scale_head = &config->scale_head;
		adaptive_scale = &config->adaptive_scale;
		autostop = &config->autostop;
		autostop_key = &config->autostop_on_key;
		autostop_lethal = &config->autostop_lethal;
		baim_mode = &config->baim_mode;
		scout_jumpshot = &config->scout_jumpshot;
		autorevolver = &config->autorevolver;
		autoscope = &config->autoscope;
		baim.lethal = &config->baim.lethal;
		baim.air = &config->baim.air;
		baim.resolver_unsure = &config->baim.resolver_unsure;
		hitscan.head = config->hitscan.head;
		hitscan.neck = config->hitscan.neck;
		hitscan.pelvis = config->hitscan.pelvis;
		hitscan.body = config->hitscan.body;
		hitscan.thorax = config->hitscan.thorax;
		hitscan.chest = config->hitscan.chest;
		hitscan.upperchest = config->hitscan.upperchest;
		hitscan.thighs = config->hitscan.thighs;
		hitscan.calfs = config->hitscan.calfs;
		hitscan.foots = config->hitscan.foots;
		hitscan.hands = config->hitscan.hands;
		hitscan.upperarms = config->hitscan.upperarms;
		hitscan.forearms = config->hitscan.forearms;
		hitscan_override.head = config->hitscan_override.head;
		hitscan_override.neck = config->hitscan_override.neck;
		hitscan_override.pelvis = config->hitscan_override.pelvis;
		hitscan_override.body = config->hitscan_override.body;
		hitscan_override.thorax = config->hitscan_override.thorax;
		hitscan_override.chest = config->hitscan_override.chest;
		hitscan_override.upperchest = config->hitscan_override.upperchest;
		hitscan_override.thighs = config->hitscan_override.thighs;
		hitscan_override.calfs = config->hitscan_override.calfs;
		hitscan_override.foots = config->hitscan_override.foots;
		hitscan_override.hands = config->hitscan_override.hands;
		hitscan_override.upperarms = config->hitscan_override.upperarms;
		hitscan_override.forearms = config->hitscan_override.forearms;
	}
	value* hitchance;
	value* mindmg;
	//value* ov_mindamage;
	value* scale;
	value* scale_head;
	value* adaptive_scale;
	value* autostop;
	value* autostop_key;
	value* autostop_lethal;
	value* baim_mode;
	value* scout_jumpshot;
	value* autorevolver;
	value* autoscope;
	struct
	{
		value* lethal;
		value* air;
		value* resolver_unsure;
	}baim;
	struct
	{
		value head;
		value neck;
		value pelvis;
		value body;
		value thorax;
		value chest;
		value upperchest;
		value thighs;
		value calfs;
		value foots;
		value hands;
		value upperarms;
		value forearms;
	}hitscan;
	struct
	{
		value head;
		value neck;
		value pelvis;
		value body;
		value thorax;
		value chest;
		value upperchest;
		value thighs;
		value calfs;
		value foots;
		value hands;
		value upperarms;
		value forearms;
	}hitscan_override;
	int index;
};

class config : public singleton<config>
{
public:
	config();
	static void set( const std::string& item );
	static void assign( const std::string& item );
	static void write( const std::string& item );
	bool load( int slot ) const;
	bool save( int slot ) const;
	std::array<weaponconfig_t, 6> weapon_config =
	{
		weaponconfig_t( 0 ),
		weaponconfig_t( 1 ),
		weaponconfig_t( 2 ),
		weaponconfig_t( 3 ),
		weaponconfig_t( 4 ),
		weaponconfig_t( 5 ),
	};

	std::array<antiaimconfig_t, 3> antiaim_config =
	{
		antiaimconfig_t( 0 ),
		antiaimconfig_t( 1 ),
		antiaimconfig_t( 2 ),
	};
private:
	std::string	config_name;
};