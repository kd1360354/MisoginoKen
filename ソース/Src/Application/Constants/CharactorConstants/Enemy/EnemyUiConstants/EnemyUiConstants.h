#pragma once
#include <string>

// =========================================================================
// 敵UI定数定義 (EnemyUiConstants.h)
// =========================================================================
namespace EnemyUIParam {

	// --- 🖼️ テクスチャパス ---
	namespace Path {
		static constexpr const char* HP_BACKGROUND = "Asset/Textures/UI/HpBackGround.png";
		static constexpr const char* HP_FRAME = "Asset/Textures/UI/HpFrame.png";
		static constexpr const char* HP_BAR = "Asset/Textures/UI/HPBar.png";
		static constexpr const char* NAME_MID_BOSS = "Asset/Textures/UI/MidBossNameUi.png";
		static constexpr const char* NAME_LAST_BOSS = "Asset/Textures/UI/LastBossNameUi.png";
	}

	// --- ⚔️ ノーマルエネミー (ビルボード) パラメータ ---
	namespace Normal {
		static constexpr float HP_BAR_OFFSET_Y = 3.0f;  
		static constexpr float BILLBOARD_Z_OFFSET = 0.05f; 
		static constexpr float DEATH_ANIM_MARGIN = 0.01f; 

		// 初期スケール
		static constexpr float DEFAULT_SCALE_X = 2.5f;
		static constexpr float DEFAULT_SCALE_Y = 0.5f;
	}

	// --- 👑 ボスUI (スプライト) パラメータ ---
	namespace Boss {
		static constexpr float POS_Y = 280.0f; 
		static constexpr float HP_BAR_OFFSET_Y = 30.0f;

		// HPバーの基本サイズ
		static constexpr float HP_BAR_WIDTH = 512.0f; 
		static constexpr float HP_BAR_HEIGHT = 64.0f;

		// 名称UIのサイズ
		static constexpr float NAME_WIDTH = 512.0f;
		static constexpr float NAME_HEIGHT = 128.0f;

		// 強調表示（RC）用サイズ
		static constexpr float NAME_RC_WIDTH = 800.0f;
		static constexpr float NAME_RC_HEIGHT = 200.0f;
	}

	// --- 📉 アニメーション制御 ---
	namespace Anim {
		// HP減少速度
		static constexpr float HP_DECREASE_SPEED = 200.0f; 
	}
}