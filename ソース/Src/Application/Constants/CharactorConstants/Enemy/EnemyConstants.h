#pragma once
#include <string>

// =========================================================================
// 敵キャラクター共通定数定義 (EnemyConstants.h)
// =========================================================================
namespace EnemyParam {

	// --- ⚙️ ボーン・ノード名 ---
	namespace Node {
		static constexpr const char* LEFT_HAND = "EnemyLeftAttackHand";
		static constexpr const char* RIGHT_HAND = "EnemyRightAttackHand";
		static constexpr const char* LEFT_FOOT = "EnemyLeftAttackLeg";
		static constexpr const char* RIGHT_FOOT = "EnemyRightAttackLeg";
		static constexpr const char* HIPS = "EnemyHipsAttack";
		static constexpr const char* NONE = "";
	}

	// --- 🏃 移動・回転 ---
	namespace Move {
		static constexpr float SPEED_WALK = 0.05f;
		static constexpr float SPEED_RUN = 0.10f;
		static constexpr float SPEED_DEFAULT = 0.15f; // MOVE_SPEED_DEF を補完 [cite: 13]
		static constexpr float ROTATION_SPEED = 5.0f;  // ENEMY_ROTATION_SPEED [cite: 10]
		static constexpr float ROTATION_DEFAULT = 10.0f; // ROTATION_SPEED_DEF を補完 [cite: 13]
		static constexpr float START_ANGLE_THR = 5.0f;  // ENEMY_MOVE_START_ANGLE [cite: 10]
		static constexpr float FORWARD_DIR_Z = -1.0f; // ENEMY_FORWARD_DIR_Z [cite: 11]
	}

	// --- 🎬 アニメーション名 ---
	namespace Anim {
		static constexpr const char* IDLE = "Idle";
		static constexpr const char* WALK = "Walk";
		static constexpr const char* RUN = "Run";
		static constexpr const char* HIT = "Hit";
		static constexpr const char* DEATH = "Death";
	}

	// --- 🔩 物理・当たり判定 ---
	namespace Physical {
		static constexpr float PUSHBACK_RADIUS = 1.25f;
		static constexpr float PUSHBACK_LOW_Y = 1.25f; 
		static constexpr float PUSHBACK_UPPER_Y = 2.25f; 
		static constexpr Math::Vector3 POS_LOW = { 0.0f, 1.25f, 0.0f };
		static constexpr Math::Vector3 POS_UPPER = { 0.0f, 2.25f, 0.0f };
	}

	// --- 🎨 演出 (リムライト・ディゾルブ) ---
	namespace Visual {
		static const Math::Vector3 RIM_COLOR = { 1.0f, 0.2f, 0.25f };
		static constexpr float     RIM_POWER_DEF = 5.0f; 
		static constexpr float     RIM_INTENSITY = 2.0f; 

		static constexpr float DISSOLVE_MIN = 0.0f; 
		static constexpr float DISSOLVE_MAX = 1.0f; 
		static constexpr float DISSOLVE_RESET = 0.0f; 
	}

	// --- 🧠 AI・パラメータ規定値 ---
	namespace Default {
		static constexpr float SEARCH_RADIUS = 15.0f; 
		static constexpr float ATTACK_RADIUS = 2.0f; 
		static constexpr float ATTACK_POWER = 10.0f; 

		static constexpr int PLAYER_HP_LOW_THR = 30; 
		static constexpr int ATTACK_RATE_33_PCT = 34; 
	}

	// --- 👑 ボス共通・特殊アクション ---
	namespace BossSpecial {
		static constexpr const char* ANIM_JUMP_ATTACK = "JumpAttack";
		static constexpr const char* ANIM_ROAR = "Roar";
		static constexpr const char* EFK_JUMP_WAVE = "JumpAttackWave1.efk";

		// ジャンプ攻撃パラメータ
		struct JumpAttack {
			static constexpr float HIT_THRESHOLD = 0.45f;
			static constexpr float SHAKE_POWER = 0.6f;
			static constexpr int   SHAKE_DURATION = 15;
			static constexpr float DAMAGE_MULT = 2.5f;
			static constexpr float RADIUS = 5.0f;
		};

		// 咆哮パラメータ
		struct Roar {
			static constexpr float SHAKE_START = 0.35f;
			static constexpr float SHAKE_END = 0.7f;
			static constexpr float SHAKE_POWER_MAX = 0.5f;
			static constexpr float SHAKE_POWER_MIN = 0.1f;
		};
	}

	// --- 🛠️ システム ---
	static constexpr int EFFECT_INVALID_HANDLE = -1; 
}