#pragma once
#include <string>

// =========================================================================
// MidBoss 固有定数定義 (MidBossConstants.h)
// =========================================================================
namespace MidBossParam {

	// --- 🎯 モデル・初期設定 ---
	static constexpr const char* NAME = "MidBoss";
	static constexpr const char* MODEL_PATH = "Asset/Models/Character/Enemy/MidBoss/VampireALusth.gltf";

	static constexpr Math::Vector3 SPAWN_POS = { 70.0f, 0.0f, 50.0f };
	static constexpr Math::Vector3 UPDATE_SCALE = { 1.5f, 1.5f, 1.5f };

	// --- 🥊 ステータス ---
	static constexpr float MAX_HP = 760.0f;
	static constexpr float ATTACK_POWER = 15.0f;
	static constexpr float SEARCH_RADIUS = 10.0f;
	static constexpr float ATTACK_RADIUS = 3.0f;
	static constexpr float DUAL_WIELD_RATIO = 0.5f; // 二刀流へ移行するHP割合

	// --- 🔩 物理・コライダー設定 ---
	namespace Collider {
		static constexpr const char* DAMAGE_TAG = "MidBoss";
		static constexpr const char* PUSHBACK_TAG = "MidBossPushBack";

		// 押し戻し用カプセル/球の設定 (EnemyBaseのデフォルトとは異なるため固有)
		static constexpr float         PUSHBACK_RADIUS = 0.75f;
		static constexpr Math::Vector3 PUSHBACK_POS_LOW = { 0.0f, 0.75f, 0.0f };
		static constexpr Math::Vector3 PUSHBACK_POS_UPPER = { 0.0f, 1.75f, 0.0f };

		// 攻撃判定
		static constexpr float         ATTACK_SPHERE_RADIUS = 1.25f;
		static constexpr Math::Vector3 ATTACK_L_WEAPON_POS = { 0.0f, 0.0f, 0.65f };
	}

	// --- 🗡️ 武器のローカル座標・回転 (行列計算用) ---
	namespace Weapon {
		// 基本設定
		static constexpr float DEFAULT_ROT_X = 90.0f;
		static constexpr float DEFAULT_TRNS_Y = -0.125f;  
		static constexpr float DEFAULT_TRNS_Z = 0.125f;
		static constexpr float DEFAULT_TRNS_X = 0.0f;
	//	static constexpr float DEFAULT_TRNS_Y = 0.0f;

		// 走り状態 (Run)
		struct Run {
			static constexpr float OFFSET_Z = -0.0625f;
			static constexpr float ADJ_X = 90.0f;
			static constexpr float ADJ_Y = -180.0f;
			static constexpr float ADJ_Z = 90.0f;
			static constexpr float TRANS_X = 0.0f;
			static constexpr float TRANS_Y = 0.0f;
		};

		// 構え状態 (Ready)
		struct Ready {
			static constexpr float ROT_X = 90.0f;
			static constexpr float ROT_Y = -15.0f;
			static constexpr float TRNS_Z = 0.05f;
		};

		// 攻撃状態 (Attack)
		struct Attack {
			static constexpr float ROT_X = 90.0f;
			static constexpr float ROT_Y = -180.0f;
			static constexpr float ROT_Z_L = 45.0f;
			static constexpr float ROT_Z_R = -45.0f;
			static constexpr float TRNS_Z = -0.0625f;
			static constexpr float TRANS_X = 0.0f;
			static constexpr float TRANS_Y = 0.0f;
			static constexpr float DOWN_SWING_ROT = -45.0f; 
		};
	}

	// --- 💡 演出・調整用 ---
	namespace Visual {
		static constexpr float ATTACK_WAIT_TIME = 0.5f;
		static constexpr float DISSOLVE_SPEED = 0.02f;

		static constexpr float     RIM_INTENSITY = 5.0f;
		static const Math::Vector3 COLOR_LOCKON = { 1.0f, 0.8f, 0.0f }; // Yellow
	}
}