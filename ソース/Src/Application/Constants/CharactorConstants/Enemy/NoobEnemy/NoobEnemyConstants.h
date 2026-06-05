#pragma once
#include <string>

// =========================================================================
// NoobEnemy 固有定数定義 (NoobEnemyConstants.h)
// =========================================================================
namespace NoobEnemyParam {

	// --- 🎯 初期化・モデル設定 ---
	static constexpr const char* NAME = "NoobEnemy";
	static constexpr const char* MODEL_PATH = "Asset/Models/Character/Enemy/NomalEnemy/ShadeofDefilementAndAnime.gltf";

	static constexpr Math::Vector3 INITIAL_SCALE = { 1.5f, 1.5f, 1.5f };

	// --- 🏃 移動パラメータ ---
	namespace Move {
		static constexpr float SPEED_INIT = 0.05f; // 初期移動速度
	}

	// --- 🥊 ステータス ---
	static constexpr float MAX_HP = 200.0f;
	static constexpr float BASE_ATTACK_POWER = 10.0f;
	static constexpr float SEARCH_RADIUS = 15.0f;
	static constexpr float ATTACK_RADIUS = 2.0f;

	// --- ⚔️ 攻撃設定 ---
	namespace Attack {
		// 当たり判定半径
		static constexpr float RADIUS_01 = 0.75f;
		static constexpr float RADIUS_02 = 1.0f;
		static constexpr float RADIUS_JUMP = 1.0f;

		// 威力倍率
		static constexpr float RATIO_01 = 1.0f;
	}

	// --- 💡 演出・調整用 ---
	namespace Visual {
		static constexpr float DISSOLVE_SPEED = 0.02f; // 消滅速度
	}
}