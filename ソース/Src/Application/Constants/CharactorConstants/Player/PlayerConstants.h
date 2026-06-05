#pragma once
#include <string>

// =========================================================================
// プレイヤー固有定数定義 (PlayerConstants.h)
// =========================================================================
namespace PlayerParam {

	// --- 🎯 基本設定 ---
	static constexpr const char* NAME       = "Player";
	static constexpr const char* MODEL_PATH = "Asset/Models/Character/Player/kaya3.0.5/Kaya/Kaya.gltf";

	static constexpr Math::Vector3 INITIAL_POS = { 0.0f, 0.0f, -70.0f };
	static constexpr float         GROUND_Y    = 0.217f;

	// --- 🥊 ステータス ---
	static constexpr float ATTACK_POWER_DEFAULT = 55.0f;

	namespace Guard {
		static constexpr float BREAK_MAX_DAMAGE    = 600.0f;
		static constexpr int   BREAK_MAX_HIT_COUNT = 3;
		static constexpr float REDUCTION_RATE      = 0.8f;
		static constexpr float MIN_HP_REDUCE_THR   = 0.01f;
		static constexpr float MIN_STAMINA_RUN_THR = 0.01f;
	}

	// --- 🏃 移動・回転 ---
	namespace Move {
		static constexpr float ROT_INTERPOLATION = 0.2f;
		static constexpr float ROT_MIN_ANGLE     = 0.1f;
		static constexpr float ROT_MAX_SPEED     = 5.0f;

		static constexpr float SPEED_DOWN  = 0.1f;
		static constexpr float SPEED_UP    = 0.2f; 

		static constexpr float JUMP_GRAVITY_THR  = 0.0f;
	}

	// --- ⚔️ 攻撃設定 (共通・通常攻撃) ---
	namespace Attack {
		static constexpr int   MAX_NORMAL_COMBO = 4;
		static constexpr float TARGET_STOP_DIST = 1.5f;
		static constexpr float TARGET_DIST_MARGIN = 0.1f;

		struct Step {
			static constexpr float START_TIME = 0.1f;
			static constexpr float END_TIME   = 0.4f;
			static constexpr float POWER      = 0.15f;
		};

		static constexpr float HIT_CHECK_RANGE     = 2.5f;
		static constexpr float HIT_INTERVAL        = 0.3f;
		static constexpr float EFFECT_DELAY        = 0.15f;
		static constexpr float FORWARD_OFFS        = 1.5f;
		static constexpr float HIT_HEIGHT_OFFS     = 1.0f;
		static constexpr float JUST_DODGE_HIT_STOP = 0.5f;

		struct Cancel {
			static constexpr float RESERVE_START      = 0.3f;
			static constexpr float COMBO_START        = 0.3f;
			static constexpr float COMBO_END          = 0.8f;
			static constexpr float DODGE_TO_ATK_START = 0.2f;
			static constexpr float DODGE_TO_ATK_END   = 0.9f;
			static constexpr float ATK_TO_DODGE_START = 0.4f;
			static constexpr float ATK_TO_DODGE_END   = 0.9f;
		};
	}

	// --- 🏃 回避 (Dodge) ---
	namespace Dodge {
		static constexpr float MOVE_START_TIME = 0.05f;
		static constexpr float MOVE_END_TIME   = 0.6f;
		static constexpr float MOVE_POWER      = 0.18f;

		static constexpr float CANCEL_MOVE_START = 0.3f;
		static constexpr float CANCEL_MOVE_END   = 0.8f;

		static constexpr float JUST_WINDOW_DUR = 0.15f;
	}

	// --- 🔥 必殺攻撃 (Power Attack) ---
	namespace PowerAttack {
		static constexpr float HIT_START_TIME = 0.3f;
		static constexpr float HIT_END_TIME   = 0.5f;
		static constexpr float RADIUS         = 6.0f;
		static constexpr float VERTICAL_OFFS  = 1.0f;
		static constexpr float DAMAGE_MULT    = 3.8f;

		static constexpr float STAMINA_COST_MULT = 3.0f;
		static constexpr float MAX_COST          = 100.0f;
		static constexpr float GAIN_ON_HIT       = 10.0f;
		static constexpr float GAIN_ON_JUST_DODGE = 30.0f;

		static constexpr float MIN_CHARGE_TIME = 0.5f;
		static constexpr float MAX_CHARGE_TIME = 1.2f;

		static constexpr float EFK_TIME = 0.05f;
		static constexpr float HIT_TIME = 0.35f;
		static constexpr float END_TIME = 0.65f;
	}

	// --- ⚔️ ダッシュ攻撃 (Run Attack) ---
	namespace RunAttack {
		static constexpr float STEP_START = 0.1f;
		static constexpr float STEP_END   = 0.4f;
		static constexpr float STEP_POWER = 0.15f;
	}

	// --- 🎬 アニメーション ---
	namespace Anim {
		static constexpr const char* IDLE         = "Idle_Combat";
		static constexpr const char* HIT_START    = "Hit_Combat_F";
		static constexpr const char* HIT_RECOVER  = "Hit_Recover";
		static constexpr const char* DEATH_START  = "Knock_Down_Start";
		static constexpr const char* DEATH_LOOP   = "Knock_Down_Loop";
		static constexpr const char* DEATH_FINAL  = "Knock_Down_Death_Combat_B";
		static constexpr float       KNOCKDOWN_DUR = 5.0f;

		static constexpr const char* ATK_01       = "Combo_Attack_05_01";
		static constexpr const char* ATK_02       = "Combo_Attack_05_02";
		static constexpr const char* ATK_03       = "Combo_Attack_05_03";
		static constexpr const char* ATK_04       = "Combo_Attack_05_04";
		static constexpr const char* RUN_ATK      = "Run_Attack_01";
		static constexpr const char* POWER_CHARGE = "Power_Attack_01";
		static constexpr const char* POWER_ATK    = "Power_Attack";

		static constexpr const char* RUN_START    = "Run_Combat_Start_F_0";
		static constexpr const char* RUN_LOOP_F   = "Run_Combat_Loop_F_0";
		static constexpr const char* RUN_LOOP_B   = "Run_Combat_Loop_B_180";
		static constexpr const char* RUN_LOOP_L   = "Run_Combat_Loop_B_L_90";
		static constexpr const char* RUN_LOOP_R   = "Run_Combat_Loop_B_R_90";
		// --- 追加: 斜め移動ループ ---
		static constexpr const char* RUN_LOOP_FL  = "Run_Combat_Loop_F_L_45";
		static constexpr const char* RUN_LOOP_FR  = "Run_Combat_Loop_F_R_45";
		static constexpr const char* RUN_LOOP_BL  = "Run_Combat_Loop_B_L_45";
		static constexpr const char* RUN_LOOP_BR  = "Run_Combat_Loop_B_R_45";

		static constexpr const char* RUN_END      = "Run_Combat_Stop_F_0";
		static constexpr const char* JUMP_START   = "Jump_Combat_Start_0";
		static constexpr const char* JUMP_LOOP    = "Jump_Combat_Loop_0";
		static constexpr const char* JUMP_END     = "Jump_Combat_End_0";

		static constexpr const char* BLOCK_START  = "Block_Combat_Start";
		static constexpr const char* BLOCK_LOOP   = "Block_Combat_Loop";
		static constexpr const char* BLOCK_END    = "Block_Combat_End";
		static constexpr const char* DODGE_F      = "Dodge_Combat_F_0";
		static constexpr const char* DODGE_B      = "Dodge_Combat_B_180";

		static constexpr const char* POTION       = "Drink_Potion";
		static constexpr const char* BUFF         = "Buff";

		struct Transition {
			static constexpr float RUN_S_TO_L   = 40.0f;
			static constexpr float RUN_E_TO_I   = 88.0f;
			static constexpr float BLOCK_S_TO_L = 17.0f;
			static constexpr float BLOCK_E_TO_I = 48.0f;
		};
	}

	// --- 🎨 演出・サウンド・UI ---
	namespace Visual {
		static constexpr Math::Vector3 RIM_COLOR     = { 0.1f, 0.4f, 1.0f };
		static constexpr Math::Vector3 LOCKON_COLOR  = { 1.0f, 0.8f, 0.0f };

		struct Effect {
			static constexpr const char* ATK_L1      = "NormalAttackLeft1.efk";
			static constexpr const char* ATK_R1      = "NormalAttackRight1.efk";
			static constexpr const char* BLOCK_LOOP  = "BlookEffect1.efk";
			static constexpr const char* POWER_ATK   = "SpecialAttackSlash.efk";
			static constexpr const char* PRE_POWER   = "SpecialAttackblackmist.efk";

			static constexpr float BLOCK_INTERVAL    = 1.0f;
			static constexpr float BLOCK_FWD_OFFS    = 0.85f;
			static constexpr float BLOCK_SCALE       = 0.5f;

			static constexpr Math::Vector3 BLOCK_LOOP_OFFS  = { 0.0f, 0.5f, 0.8f };
			static constexpr Math::Vector3 BLOCK_HIT_OFFS   = { 0.0f, 2.5f, 5.0f };
			static constexpr Math::Vector3 POWER_ATK_OFFS   = { 0.0f, 1.0f, 1.0f };
		};

		struct Sound {
			static constexpr const char* KATANA_SWING   = "Asset/Audio/SE/KatanaSE.wav";
			static constexpr float       VOLUME_DEFAULT = 0.2f;
		};
	}
}