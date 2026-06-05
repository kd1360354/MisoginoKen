#pragma once


// =========================================================================
// タイトル画面定数定義 (TitleConstants.h)
// =========================================================================
namespace TitleParam {

	// --- 🎨 テクスチャインデックス ---
	enum class TexIndex {
		Background,
		TitleText,
		Logo1,
		Logo2,
		ClickToStart,
		LeftArrow,
		Percent25,
		NowLoading,
		Max // 総数 (TITLE_TEX_COUNT)
	};

	// --- 🖼️ テクスチャパス ---
	namespace Path {
		static constexpr const char* BACKGROUND = "Asset/Textures/Title/TitleBackGround.png";
		static constexpr const char* TITLE_TEXT = "Asset/Textures/Title/TitleText2.png";
		static constexpr const char* LOGO_1 = "Asset/Textures/Title/TitleRogo1.png";
		static constexpr const char* LOGO_2 = "Asset/Textures/Title/TitleRogo2.png";
		static constexpr const char* CLICK_TO_START = "Asset/Textures/Title/ClickToStart.png";
		static constexpr const char* LEFT_ARROW = "Asset/Textures/Title/LeftArrow.png";
		static constexpr const char* PERCENT_25 = "Asset/Textures/Title/25png.png";
		static constexpr const char* NOW_LOADING = "Asset/Textures/Title/NowLoadingText.png";
	}

	// --- 🖥️ 描画座標・サイズ ---
	namespace UI {
		static constexpr Math::Vector2 CENTER_POS = { 0.0f, 0.0f };
		static constexpr Math::Vector2 LEFT_ARROW_POS = { -150.0f, -315.0f };
		static constexpr Math::Vector2 LOADING_POS = { 400.0f, -300.0f };
		static constexpr Math::Vector2 LOADING_RECT_SZ = { 500.0f, 200.0f };
	}

	// --- 📐 制御用 ---
	namespace System {
		static constexpr int KEY_PRESS_MASK = 0x8000;
	}
}