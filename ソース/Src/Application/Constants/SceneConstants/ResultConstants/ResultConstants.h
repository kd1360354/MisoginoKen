#pragma once
#include <string>

// =========================================================================
// リザルト画面定数定義 (ResultConstants.h)
// =========================================================================
namespace ResultParam {

	// --- 🎨 テクスチャインデックス ---
	enum class TexIndex {
		Background,
		ClickToTitle,
		LeftArrow,
		ThankYou,
		GameOver,
		Max // 総数
	};

	// --- 🖼️ テクスチャパス ---
	namespace Path {
		static constexpr const char* BACKGROUND = "Asset/Textures/Reslut/TitleBackGround.png";
		static constexpr const char* CLICK_TO_TITLE = "Asset/Textures/Reslut/ClickToTitle.png";
		static constexpr const char* LEFT_ARROW = "Asset/Textures/Reslut/LeftArrow.png";
		static constexpr const char* GAMEOVER = "Asset/Textures/Reslut/GameOver.png";
		static constexpr const char* THANK_YOU = "Asset/Textures/Reslut/ThankyouforPlaying.png";
	}

	// --- 🖥️ 画面座標 ---
	namespace UI {
		static constexpr Math::Vector2 CLICK_TO_TITLE_POS = { 450.0f, -635.0f };
		static constexpr Math::Vector2 LEFT_ARROW_POS = { -150.0f, -315.0f };
		static constexpr Math::Vector2 THANK_YOU_POS = { 400.0f, -300.0f };
	}
}