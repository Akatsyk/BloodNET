#include "../external_include.h"
#include <fstream>
#include "json.h"

nlohmann::json preset;

c_vars vars;

tsl::robin_map< std::string, value* > g_cfg
{
{ _n( "v_drk" ) , &vars.visuals.dark },
{ _n( "v_prop_tr" ) , &vars.visuals.prop_transparency },
{ _n( "v_spr" ) , &vars.visuals.spread_circle },
{ _n( "v_spr_cl1" ), &vars.visuals.spread_circle_color1 },
{ _n( "v_spr_cl2" ), &vars.visuals.spread_circle_color2 },
{ _n( "v_spr_typ" ), &vars.visuals.spread_circle_type },
{ _n( "v_oof" ), &vars.visuals.out_of_fov },
{ _n( "v_oof_cl" ), &vars.visuals.out_of_fov_col },
{ _n( "v_oof_dst" ), &vars.visuals.out_of_fov_dst },

{ _n( "aim" ), &vars.aim.enabled },
{ _n( "aim_af" ), &vars.aim.autofire },
{ _n( "aim_bm_ok" ), &vars.aim.baim_on_key },
{ _n( "aim_bt" ), &vars.aim.backtrack },
{ _n( "aim_fk_cmp" ), &vars.aim.fakelag_compensation },
{ _n( "aim_ps" ), &vars.aim.ping_spike },
{ _n( "aim_ps_at" ), &vars.aim.ping_spike_amt },
{ _n( "aim_ps_bs" ), &vars.aim.ping_spike_base },
{ _n( "aim_ps_bs_at" ), &vars.aim.ping_spike_base_amt },
{ _n( "aim_rslv" ), &vars.aim.resolver },
{ _n( "aim_rslv_tm" ), &vars.aim.resolve_team },
{ _n( "aim_ovr" ), &vars.aim.override },
{ _n( "aim_hsonly" ), &vars.aim.hsonly },
{ _n("aim_dtap"), &vars.misc.doubletap },
{ _n( "aim_zs_hc" ), &vars.aim.zeus_hc },
{ _n( "aim_bot_kn" ), &vars.aim.knife_bot },
{ _n( "aim_bot_zs" ), &vars.aim.zeus_bot },
{ _n( "aim_bot_record" ), &vars.aim.aimbot_record },

{ _n("aim_au_hs_1"), &vars.config.autosniper.hitscan.head },
{ _n("aim_au_hs_2"), &vars.config.autosniper.hitscan.neck },
{ _n("aim_au_hs_3"), &vars.config.autosniper.hitscan.chest },
{ _n("aim_au_hs_4"), &vars.config.autosniper.hitscan.forearms },
{ _n("aim_au_hs_5"), &vars.config.autosniper.hitscan.body },
{ _n("aim_au_hs_6"), &vars.config.autosniper.hitscan.calfs },
{ _n("aim_au_hs_7"), &vars.config.autosniper.hitscan.foots },
//{ _n("aim_au_override_hs_1"), &vars.config.autosniper.hitscan_override.head },
//{ _n("aim_au_override_hs_2"), &vars.config.autosniper.hitscan_override.neck },
//{ _n("aim_au_override_hs_3"), &vars.config.autosniper.hitscan_override.pelvis },
//{ _n("aim_au_override_hs_4"), &vars.config.autosniper.hitscan_override.body },
//{ _n("aim_au_override_hs_5"), &vars.config.autosniper.hitscan_override.thorax },
//{ _n("aim_au_override_hs_6"), &vars.config.autosniper.hitscan_override.chest },
//{ _n("aim_au_override_hs_7"), &vars.config.autosniper.hitscan_override.upperchest },
//{ _n("aim_au_override_hs_8"), &vars.config.autosniper.hitscan_override.thighs },
//{ _n("aim_au_override_hs_9"), &vars.config.autosniper.hitscan_override.calfs },
//{ _n("aim_au_override_hs_10"), &vars.config.autosniper.hitscan_override.foots },
//{ _n("aim_au_override_hs_11"), &vars.config.autosniper.hitscan_override.hands },
//{ _n("aim_au_override_hs_12"), &vars.config.autosniper.hitscan_override.upperarms },
//{ _n("aim_au_override_hs_13"), &vars.config.autosniper.hitscan_override.forearms },


{ _n( "cfg_au_hc" ), &vars.config.autosniper.hitchance },
{ _n( "cfg_au_mindmg" ), &vars.config.autosniper.mindmg },
{ _n( "cfg_au_scale" ), &vars.config.autosniper.scale },
{ _n( "cfg_au_scale_head" ), &vars.config.autosniper.scale_head },
{ _n( "cfg_au_ad_scale" ), &vars.config.autosniper.adaptive_scale },
{ _n( "cfg_au_asc" ), &vars.config.autosniper.autoscope },
{ _n( "cfg_au_as" ), &vars.config.autosniper.autostop },
{ _n( "cfg_au_as_ok" ), &vars.config.autosniper.autostop_on_key },
{ _n( "cfg_au_as_ol" ), &vars.config.autosniper.autostop_lethal },
{ _n( "cfg_au_baim_md" ), &vars.config.autosniper.baim_mode },
{ _n( "aim_au_bm_air" ), &vars.config.autosniper.baim.air },
{ _n( "aim_au_bm_l" ), &vars.config.autosniper.baim.lethal },
{ _n( "aim_au_bm_ru" ), &vars.config.autosniper.baim.resolver_unsure },

{ _n( "cfg_sc_hc" ), &vars.config.scout.hitchance },
{ _n( "cfg_sc_mindmg" ), &vars.config.scout.mindmg },
{ _n( "cfg_sc_scale" ), &vars.config.scout.scale },
{ _n( "cfg_sc_scale_head" ), &vars.config.scout.scale_head },
{ _n( "cfg_sc_ad_scale" ), &vars.config.scout.adaptive_scale },
{ _n( "cfg_sc_asc" ), &vars.config.scout.autoscope },
{ _n( "cfg_sc_as" ), &vars.config.scout.autostop },
{ _n( "cfg_sc_as_ok" ), &vars.config.scout.autostop_on_key },
{ _n( "cfg_sc_as_ol" ), &vars.config.scout.autostop_lethal },
{ _n( "cfg_sc_baim_md" ), &vars.config.scout.baim_mode },
{ _n( "cfg_sc_js" ), &vars.config.scout.scout_jumpshot },
{ _n( "aim_sc_bm_air" ), &vars.config.scout.baim.air },
{ _n( "aim_sc_bm_l" ), &vars.config.scout.baim.lethal },
{ _n( "aim_sc_bm_ru" ), &vars.config.scout.baim.resolver_unsure },
{ _n("aim_sc_hs_1"), &vars.config.scout.hitscan.head },
{ _n("aim_sc_hs_2"), &vars.config.scout.hitscan.neck },
{ _n("aim_sc_hs_3"), &vars.config.scout.hitscan.chest },
{ _n("aim_sc_hs_4"), &vars.config.scout.hitscan.forearms },
{ _n("aim_sc_hs_5"), &vars.config.scout.hitscan.body },
{ _n("aim_sc_hs_6"), &vars.config.scout.hitscan.calfs },
{ _n("aim_sc_hs_7"), &vars.config.scout.hitscan.foots },

{ _n( "cfg_awp_hc" ), &vars.config.awp.hitchance },
{ _n( "cfg_awp_mindmg" ), &vars.config.awp.mindmg },
{ _n( "cfg_awp_asc" ), &vars.config.awp.autoscope },
{ _n( "cfg_awp_scale" ), &vars.config.awp.scale },
{ _n( "cfg_awp_scale_head" ), &vars.config.awp.scale_head },
{ _n( "cfg_awp_ad_scale" ), &vars.config.awp.adaptive_scale },
{ _n( "cfg_awp_as" ), &vars.config.awp.autostop },
{ _n( "cfg_awp_as_ok" ), &vars.config.awp.autostop_on_key },
{ _n( "cfg_awp_as_ol" ), &vars.config.awp.autostop_lethal },
{ _n( "cfg_awp_baim_md" ), &vars.config.awp.baim_mode },
{ _n( "aim_awp_bm_air" ), &vars.config.awp.baim.air },
{ _n( "aim_awp_bm_l" ), &vars.config.awp.baim.lethal },
{ _n( "aim_awp_bm_ru" ), &vars.config.awp.baim.resolver_unsure },
{ _n("aim_awp_hs_1"), &vars.config.awp.hitscan.head },
{ _n("aim_awp_hs_2"), &vars.config.awp.hitscan.neck },
{ _n("aim_awp_hs_3"), &vars.config.awp.hitscan.chest },
{ _n("aim_awp_hs_4"), &vars.config.awp.hitscan.forearms },
{ _n("aim_awp_hs_5"), &vars.config.awp.hitscan.body },
{ _n("aim_awp_hs_6"), &vars.config.awp.hitscan.calfs },
{ _n("aim_awp_hs_7"), &vars.config.awp.hitscan.foots },

{ _n( "cfg_hp_hc" ), &vars.config.heavy_pistol.hitchance },
{ _n( "cfg_hp_mindmg" ), &vars.config.heavy_pistol.mindmg },
{ _n( "cfg_hp_scale" ), &vars.config.heavy_pistol.scale },
{ _n( "cfg_hp_scale_head" ), &vars.config.heavy_pistol.scale_head },
{ _n( "cfg_hp_ad_scale" ), &vars.config.heavy_pistol.adaptive_scale },
{ _n( "cfg_hp_as" ), &vars.config.heavy_pistol.autostop },
{ _n( "cfg_hp_as_ok" ), &vars.config.heavy_pistol.autostop_on_key },
{ _n( "cfg_hp_as_ol" ), &vars.config.heavy_pistol.autostop_lethal },
{ _n( "cfg_hp_baim_md" ), &vars.config.heavy_pistol.baim_mode },
{ _n( "cfg_hp_ar" ), &vars.config.heavy_pistol.autorevolver },
{ _n( "aim_hp_bm_air" ), &vars.config.heavy_pistol.baim.air },
{ _n( "aim_hp_bm_l" ), &vars.config.heavy_pistol.baim.lethal },
{ _n( "aim_hp_bm_ru" ), &vars.config.heavy_pistol.baim.resolver_unsure },
{ _n("aim_hp_hs_1"), &vars.config.heavy_pistol.hitscan.head },
{ _n("aim_hp_hs_2"), &vars.config.heavy_pistol.hitscan.neck },
{ _n("aim_hp_hs_3"), &vars.config.heavy_pistol.hitscan.chest },
{ _n("aim_hp_hs_4"), &vars.config.heavy_pistol.hitscan.forearms },
{ _n("aim_hp_hs_5"), &vars.config.heavy_pistol.hitscan.body },
{ _n("aim_hp_hs_6"), &vars.config.heavy_pistol.hitscan.calfs },
{ _n("aim_hp_hs_7"), &vars.config.heavy_pistol.hitscan.foots },

{ _n( "cfg_p_hc" ), &vars.config.pistol.hitchance },
{ _n( "cfg_p_mindmg" ), &vars.config.pistol.mindmg },
{ _n( "cfg_p_scale" ), &vars.config.pistol.scale },
{ _n( "cfg_p_scale_head" ), &vars.config.pistol.scale_head },
{ _n( "cfg_p_ad_scale" ), &vars.config.pistol.adaptive_scale },
{ _n( "cfg_p_as" ), &vars.config.pistol.autostop },
{ _n( "cfg_p_as_ok" ), &vars.config.pistol.autostop_on_key },
{ _n( "cfg_p_as_ol" ), &vars.config.pistol.autostop_lethal },
{ _n( "cfg_p_baim_md" ), &vars.config.pistol.baim_mode },
{ _n( "aim_p_bm_air" ), &vars.config.pistol.baim.air },
{ _n( "aim_p_bm_l" ), &vars.config.pistol.baim.lethal },
{ _n( "aim_p_bm_ru" ), &vars.config.pistol.baim.resolver_unsure },
{ _n("aim_p_hs_1"), &vars.config.pistol.hitscan.head },
{ _n("aim_p_hs_2"), &vars.config.pistol.hitscan.neck },
{ _n("aim_p_hs_3"), &vars.config.pistol.hitscan.chest },
{ _n("aim_p_hs_4"), &vars.config.pistol.hitscan.forearms },
{ _n("aim_p_hs_5"), &vars.config.pistol.hitscan.body },
{ _n("aim_p_hs_6"), &vars.config.pistol.hitscan.calfs },
{ _n("aim_p_hs_7"), &vars.config.pistol.hitscan.foots },

{ _n( "cfg_o_hc" ), &vars.config.other.hitchance },
{ _n( "cfg_o_mindmg" ), &vars.config.other.mindmg },
{ _n( "cfg_o_scale" ), &vars.config.other.scale },
{ _n( "cfg_o_scale_head" ), &vars.config.other.scale_head },
{ _n( "cfg_o_ad_scale" ), &vars.config.other.adaptive_scale },
{ _n( "cfg_o_as" ), &vars.config.other.autostop },
{ _n( "cfg_o_as_ok" ), &vars.config.other.autostop_on_key },
{ _n( "cfg_o_as_ol" ), &vars.config.other.autostop_lethal },
{ _n( "cfg_o_baim_md" ), &vars.config.other.baim_mode },
{ _n( "aim_o_bm_air" ), &vars.config.other.baim.air },
{ _n( "aim_o_bm_l" ), &vars.config.other.baim.lethal },
{ _n( "aim_o_bm_ru" ), &vars.config.other.baim.resolver_unsure },
{ _n("aim_o_hs_1"), &vars.config.other.hitscan.head },
{ _n("aim_o_hs_2"), &vars.config.other.hitscan.neck },
{ _n("aim_o_hs_3"), &vars.config.other.hitscan.chest },
{ _n("aim_o_hs_4"), &vars.config.other.hitscan.forearms },
{ _n("aim_o_hs_5"), &vars.config.other.hitscan.body },
{ _n("aim_o_hs_6"), &vars.config.other.hitscan.calfs },
{ _n("aim_o_hs_7"), &vars.config.other.hitscan.foots },

{ _n( "aa" ), &vars.aa.enabled },
{ _n( "aa_fk_dis" ), &vars.aa.fakelag_when_disabled },
{ _n( "aa_fkwk" ), &vars.aa.fakewalk },
{ _n("aa_fkdk"), &vars.aa.fakeduck },
{ _n("aa_fkmv_t"), &vars.aa.fakemove_ticks },
{ _n( "aa_fk_on_peek" ), &vars.aa.on_peek },
{ _n( "aa_onsht" ), &vars.aa.prevent_onshot },
{ _n( "aa_fs_drm_to" ), &vars.aa.fs_dormant_timeout },
{ _n( "aa_manual" ), &vars.aa.manual },
{ _n( "aa_indicator" ), &vars.aa.indicator },
{ _n( "aa_indicator_col" ), &vars.aa.indicator_col },
{ _n( "aa_ck_rl" ), &vars.aa.choke_reload },
{ _n( "aa_ck_sw" ), &vars.aa.choke_weapon_switch },
{ _n( "aa_ck_sh" ), &vars.aa.choke_shot },
{ _n( "aa_fs_fps" ), &vars.aa.freestanding_fps },
{ _n("aa_fk_angles"), &vars.aa.fake_angles },

{ _n( "aa_r" ), &vars.aa.regular.enabled },
{ _n( "aa_r_spin" ), &vars.aa.regular.spin },
{ _n( "aa_r_spin_speed" ), &vars.aa.regular.spin_speed },
{ _n( "aa_r_spin_range" ), &vars.aa.regular.spin_range },
{ _n( "aa_r_jitter" ), &vars.aa.regular.jitter },
{ _n( "aa_r_jitter_range" ), &vars.aa.regular.jitter_range },
{ _n( "aa_r_jitter_speed" ), &vars.aa.regular.jitter_speed },
{ _n( "aa_r_jitter_random" ), &vars.aa.regular.jitter_random },
{ _n( "aa_r_lby" ), &vars.aa.regular.lby },
{ _n( "aa_r_lby_static" ), &vars.aa.regular.lby_static },
{ _n( "aa_r_lby_autodirection" ), &vars.aa.regular.lby_autodirection },
{ _n( "aa_r_at_target" ), &vars.aa.regular.at_target },
{ _n( "aa_r_edge" ), &vars.aa.regular.edge },
{ _n( "aa_r_freestand" ), &vars.aa.regular.freestand },
{ _n( "aa_r_add" ), &vars.aa.regular.add },
{ _n( "aa_r_choke" ), &vars.aa.regular.choke },
{ _n( "aa_r_lby_delta_first" ), &vars.aa.regular.lby_delta_first },
{ _n( "aa_r_lby_delta" ), &vars.aa.regular.lby_delta },
{ _n( "aa_r_lby_random" ), &vars.aa.regular.lby_random },
{ _n( "aa_r_lby_random_add" ), &vars.aa.regular.lby_random_add },
{ _n( "aa_r_lby_random_range" ), &vars.aa.regular.lby_random_range },
{ _n( "aa_r_lby_when_hittable" ), &vars.aa.moving.lby_when_hittable },
{ _n( "aa_r_lby_delta_when_hittable" ), &vars.aa.moving.lby_delta_when_hittable },

{ _n( "aa_a" ), &vars.aa.air.enabled },
{ _n( "aa_a_spin" ), &vars.aa.air.spin },
{ _n( "aa_a_spin_speed" ), &vars.aa.air.spin_speed },
{ _n( "aa_a_spin_range" ), &vars.aa.air.spin_range },
{ _n( "aa_a_jitter" ), &vars.aa.air.jitter },
{ _n( "aa_a_jitter_range" ), &vars.aa.air.jitter_range },
{ _n( "aa_a_jitter_speed" ), &vars.aa.air.jitter_speed },
{ _n( "aa_a_jitter_random" ), &vars.aa.air.jitter_random },
{ _n( "aa_a_lby" ), &vars.aa.air.lby },
{ _n( "aa_a_at_target" ), &vars.aa.air.at_target },
{ _n( "aa_a_edge" ), &vars.aa.air.edge },
{ _n( "aa_a_freestand" ), &vars.aa.air.freestand },
{ _n( "aa_a_add" ), &vars.aa.air.add },
{ _n( "aa_a_choke" ), &vars.aa.air.choke },

{ _n( "aa_m" ), &vars.aa.moving.enabled },
{ _n( "aa_m_spin" ), &vars.aa.moving.spin },
{ _n( "aa_m_spin_speed" ), &vars.aa.moving.spin_speed },
{ _n( "aa_m_spin_range" ), &vars.aa.moving.spin_range },
{ _n( "aa_m_jitter" ), &vars.aa.moving.jitter },
{ _n( "aa_m_jitter_range" ), &vars.aa.moving.jitter_range },
{ _n( "aa_m_jitter_speed" ), &vars.aa.moving.jitter_speed },
{ _n( "aa_m_jitter_random" ), &vars.aa.moving.jitter_random },
{ _n( "aa_m_at_target" ), &vars.aa.moving.at_target },
{ _n( "aa_m_edge" ), &vars.aa.moving.edge },
{ _n( "aa_m_freestand" ), &vars.aa.moving.freestand },
{ _n( "aa_m_add" ), &vars.aa.moving.add },
{ _n( "aa_m_choke" ), &vars.aa.moving.choke },


{ _n( "k_au" ), &vars.key.autostop },
{ _n( "k_bm" ), &vars.key.baim },
{ _n( "k_fkwk" ), &vars.key.fakewalk },
{ _n("k_fkdk"), &vars.key.fakeduck },
{ _n( "k_ps" ), &vars.key.ping_spike },
{ _n( "k_tp" ), &vars.key.thirdperson },
{ _n( "k_ovr" ), &vars.key.override },
{ _n( "k_aa_back" ), &vars.key.aa_back },
{ _n( "k_aa_left" ), &vars.key.aa_left },
{ _n( "k_aa_right" ), &vars.key.aa_right },
{ _n( "k_autopeek" ), &vars.key.autopeek },

{ _n( "m_bhp" ), &vars.misc.bhop },
{ _n("aspect"), &vars.misc.aspectratio },
{ _n("aspectsize"), &vars.misc.aspectratiosize },

{ _n("m_gren_traj"), &vars.misc.grenade_trajectory },
{ _n("m_gren_size"), &vars.misc.grenade_trajectory_size },
{ _n( "m_fov" ), &vars.misc.fov_enabled },
{ _n( "m_fov_sc" ), &vars.misc.fov_scope },
{ _n( "m_fov_val" ), &vars.misc.fov },
{ _n( "m_vm_fov" ), &vars.misc.vm_fov_enabled },
{ _n( "m_vm_fov_val" ), &vars.misc.vm_fov },
{ _n( "m_hitsnd" ), &vars.misc.hitmarker },
{ _n( "m_hitsnd_f" ), &vars.misc.hitsound_select },
{ _n( "m_rdr" ), &vars.misc.radar },
{ _n( "m_rm_flsh" ), &vars.misc.remove_flash },
{ _n( "m_rm_sc" ), &vars.misc.remove_scope },
{ _n( "m_rm_smk" ), &vars.misc.remove_smoke },
{ _n( "m_strf" ), &vars.misc.autostrafe },
{ _n( "m_tp" ), &vars.misc.thirdperson },
{ _n( "m_tp_ded" ), &vars.misc.thirdperson_dead },
{ _n( "m_tp_dst" ), &vars.misc.thirdperson_dist },
{ _n( "m_tr_sc" ), &vars.misc.transparency_in_scope },
{ _n( "m_fr_ch" ), &vars.misc.force_crosshair },
{ _n( "m_aac" ), &vars.misc.autoaccept },
{ _n( "m_rvr" ), &vars.misc.reveal_ranks },
{ _n( "m_hrt" ), &vars.misc.hurtlog },
{ _n( "m_buy" ), &vars.misc.buylog },
{ _n( "m_cltg" ), &vars.misc.clantag },
{ _n( "m_prsrv" ), &vars.misc.preservedn },
{ _n( "m_impct" ), &vars.misc.impacts },
{ _n( "m_autopeek" ), &vars.misc.autopeek },
{ _n( "m_autopeek_radius" ), &vars.misc.autopeek_radius },

{ _n( "v_drmnt_esp" ), &vars.visuals.esp.dormant_esp },
{ _n( "v_amm" ), &vars.visuals.esp.ammo },
{ _n( "v_amm_cl" ), &vars.visuals.esp.ammo_color },
{ _n( "v_arm" ), &vars.visuals.esp.armor },
{ _n( "v_arm_cl" ), &vars.visuals.esp.armor_color },
{ _n( "v_png" ), &vars.visuals.esp.ping },
{ _n( "v_png_cl" ), &vars.visuals.esp.ping_color },
{ _n( "v_hp" ), &vars.visuals.esp.health },
{ _n( "v_i_nm" ), &vars.visuals.esp.name },
{ _n( "v_i_nm_cl" ), &vars.visuals.esp.name_color },
{ _n( "v_i_w_nm" ), &vars.visuals.esp.weapon },
{ _n( "v_i_w_nm_cl" ), &vars.visuals.esp.weapon_color },
{ _n( "v_lby_t" ), &vars.visuals.esp.lby_timer },
{ _n("v_lby_t_cl"), &vars.visuals.esp.lby_timer_color },
{ _n( "v_i_scop" ), &vars.visuals.esp.scoped },
{ _n( "v_i_bomb" ), &vars.visuals.esp.bomb },
{ _n( "v_box" ), &vars.visuals.esp.box },
{ _n( "v_box_cl" ), &vars.visuals.esp.box_color },
{ _n( "v_w_weap" ), &vars.visuals.esp.world.weapons },
{ _n( "v_w_gre" ), &vars.visuals.esp.world.grenades },
{ _n( "v_w_c4" ), &vars.visuals.esp.world.c4 },
{ _n( "v_w_cl" ), &vars.visuals.esp.world.color },

{ _n( "v_bms" ), &vars.visuals.beams.enabled },
{ _n( "v_bms_e" ), &vars.visuals.beams.enemy.enabled },
{ _n( "v_bms_e_cl" ), &vars.visuals.beams.enemy.color },
{ _n( "v_bms_t" ), &vars.visuals.beams.team.enabled },
{ _n( "v_bms_t_cl" ), &vars.visuals.beams.team.color },
{ _n( "v_bms_loc" ), &vars.visuals.beams.local.enabled },
{ _n( "v_bms_loc_cl" ), &vars.visuals.beams.local.color },

{ _n( "v_ch" ), &vars.visuals.chams.enabled },
{ _n( "v_ch_xt" ), &vars.visuals.chams.extended },
{ _n( "v_ch_hid" ), &vars.visuals.chams.hidden },
{ _n( "v_ch_e" ), &vars.visuals.chams.enemy.enabled },
{ _n( "v_ch_e_t" ), &vars.visuals.chams.enemy.type },
{ _n( "v_ch_e_cl" ), &vars.visuals.chams.enemy.color },
{ _n( "v_ch_hid_e_cl" ), &vars.visuals.chams.enemy.hidden_color },
{ _n( "v_ch_e_cl_sh" ), &vars.visuals.chams.enemy.color_shine },
{ _n( "v_ch_t" ), &vars.visuals.chams.team.enabled },
{ _n( "v_ch_t_t" ), &vars.visuals.chams.team.type },
{ _n( "v_ch_t_cl" ), &vars.visuals.chams.team.color },
{ _n( "v_ch_hid_t_cl" ), &vars.visuals.chams.team.hidden_color },
{ _n( "v_ch_t_cl_sh" ), &vars.visuals.chams.team.color_shine },
{ _n( "v_ch_loc" ), &vars.visuals.chams.local.enabled },
{ _n( "v_ch_loc_t" ), &vars.visuals.chams.local.type },
{ _n( "v_ch_loc_cl" ), &vars.visuals.chams.local.color },
{ _n( "v_ch_loc_cl_sh" ), &vars.visuals.chams.local.color_shine },
{ _n( "v_ch_hst" ), &vars.visuals.chams.history.enabled },
{ _n( "v_ch_hst_cl" ), &vars.visuals.chams.history.color },

{ _n( "v_gl" ), &vars.visuals.glow.enabled },
{ _n( "v_gl_e" ), &vars.visuals.glow.enemy.enabled },
{ _n( "v_gl_e_cl" ), &vars.visuals.glow.enemy.color },
{ _n( "v_gl_t" ), &vars.visuals.glow.team.enabled },
{ _n( "v_gl_t_cl" ), &vars.visuals.glow.team.color },
{ _n( "v_gl_loc" ), &vars.visuals.glow.local.enabled },
{ _n( "v_gl_loc_cl" ), &vars.visuals.glow.local.color },
{ _n( "v_gl_w_weap" ), &vars.visuals.glow.world.weapons },
{ _n( "v_gl_w_gre" ), &vars.visuals.glow.world.grenades },
{ _n( "v_gl_w_c4" ), &vars.visuals.glow.world.c4 },
{ _n( "v_gl_w_cl" ), &vars.visuals.glow.world.color },

{ _n( "menu_slot" ), &vars.menu.slot },
{ _n( "inventory_list_idx" ), &vars.menu.inventory_list_idx },

{ _n( "inventory_paint_kit" ), &vars.inventory.paint_kit },
{ _n( "inventory_definition_index" ), &vars.inventory.definition_index },
{ _n( "inventory_seed" ), &vars.inventory.seed },
{ _n( "inventory_stat_trak" ), &vars.inventory.stat_trak },
{ _n( "inventory_wear" ), &vars.inventory.wear },

{ _n("vis_buy_auto"), &vars.autobuy.auto_s },
{ _n("vis_buy_ssg"), &vars.autobuy.ssg },
{ _n("vis_buy_awp"), &vars.autobuy.awp },
{ _n("vis_buy_deagle"), &vars.autobuy.deagle },
{ _n("vis_buy_dualies"), &vars.autobuy.dualies },
{ _n("vis_buy_he"), &vars.autobuy.hegrenade },
{ _n("vis_buy_molly"), &vars.autobuy.molly },
{ _n("vis_buy_smoke"), &vars.autobuy.smokegrenade },
{ _n("vis_buy_flash"), &vars.autobuy.flashbang },
{ _n("vis_buy_flashs"), &vars.autobuy.flashbangs },
{ _n("vis_buy_kit"), &vars.autobuy.kit },
{ _n("vis_buy_taser"), &vars.autobuy.taser },
{ _n("vis_buy_armor"), &vars.autobuy.armor },
};

config::config()
{
	config_name = _w( "bloodnet_#" );
}

void config::set( const std::string& item )
{
	auto value = g_cfg[ item ];

	if (!value)
		return;

	value->set( preset[ item ].get<double>() );
}

void config::assign( const std::string& item )
{
	if ( preset[ item ].empty() )
		return;

	set( item );
}

void config::write( const std::string& item )
{
	auto value = g_cfg[ item ];

	if (!value)
		return;

	preset[ item ] = value->get<double>();
}

bool config::load( const int bloodnet) const
{
	_u( config_name_u, config_name );
	const auto config = config_name_u + std::to_string(bloodnet);
	auto input_file = std::ifstream( config );

	if ( !input_file.good() )
		return false;

	try
	{
		preset << input_file;
	}
	catch ( ... )
	{
		input_file.close();
		return false;
	}

	for ( auto& item : g_cfg )
	{
		if (!item.second || item.first.length() <= 1)
			continue;

		assign( item.first );
	}

	input_file.close();

	return true;
}

bool config::save( const int bloodnet) const
{
	_u( config_name_u, config_name );
	const auto config = config_name_u + std::to_string(bloodnet);
	auto output_file = std::ofstream( config );

	if ( !output_file.good() )
		return false;

	_( inventory_s, "inventory_" );
	for ( auto& item : g_cfg )
	{
		if (!item.second || item.first.length() <= 1)
			continue;

		if ( !strstr( item.first.c_str(), inventory_s ) )
			write( item.first );
	}

	output_file << std::setw( 4 ) << preset << std::endl;
	output_file.close();

	return true;
}
