#pragma once
#include <string>

namespace LastBossParam {

	// --- 🎯 基本設定 ---
	static constexpr const char* NAME = "LastBoss";
	static constexpr const char* MODEL_PATH = "Asset/Models/Character/Enemy/LastBoss/Mutane.gltf";
	static constexpr Math::Vector3 SPAWN_POS = { 0.0f, 0.0f, 50.0f };
	static constexpr Math::Vector3 INITIAL_SCALE = { 2.0f, 2.0f, 2.0f };

	// --- 🥊 ステータス ---
	static constexpr float MAX_HP = 1000.0f;
	static constexpr float SEARCH_RADIUS = 15.0f;
	static constexpr float ATTACK_RADIUS = 3.0f;
	static constexpr float ATTACK_WAIT_TIME = 0.3f; // 攻撃後の硬直

	// --- 🧠 AI/フェーズ制御 ---
	static constexpr float HP_TRIGGER_PHASE_2 = 0.50f;
	static constexpr float HP_TRIGGER_PHASE_3 = 0.30f;
	static constexpr float PHASE_CHANGE_WAIT = 1.5f;   // フェーズ移行時の溜め時間
	static constexpr int ATTACK_CHOICES = 3;          // 攻撃パターンの選択肢数

	// --- 🎬 演出・アニメーション ---
	static constexpr const char* DEATH_EFFECT_NAME = "boss_death.efk";

	namespace Anim {
		static constexpr float SPEED_DEFAULT = 0.35f;
		static constexpr float SPEED_DEATH = 0.2f;
		static constexpr float SPEED_MOVE = 0.5f;
	}

	namespace Dissolve {
		static constexpr float SPEED = 0.005f;
		static constexpr float THRESHOLD = 0.0f;
	}

	// --- ⚔️ 攻撃設定 ---
	namespace Attack {
		// ダメージ倍率
		static constexpr float RATIO_LIGHT = 1.0f; 
		static constexpr float RATIO_MEDIUM = 1.2f;
		static constexpr float RATIO_HEAVY = 1.8f; 

		// ジャンプ攻撃固有 (Jump Attack)
		static constexpr float JUMP_WARP_TIME = 48.0f;   // ワープまでの時間
		static constexpr float JUMP_WARP_HEIGHT = 5.0f; // ワープ時の高さ
		static constexpr float JUMP_RADIUS = 5.0f;      // 攻撃判定半径
	}
}