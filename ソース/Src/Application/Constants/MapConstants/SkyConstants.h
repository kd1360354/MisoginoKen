#pragma once

// =========================================================================
// スカイボックス定数定義 (SkyConstants.h)
// =========================================================================
namespace SkyParam {

	// --- 🌌 SkyBase 共通設定 ---
	namespace Base {
		static constexpr float SCALE = 1.0f;
		static constexpr float ROTATION_SPEED = 0.0125f;
		static constexpr Math::Vector3 POS = { 0.0f, 0.0f, 0.0f };
	}

	// --- 🌃 真夜中 (MidNight) ---
	namespace MidNightParam {
		static constexpr const char* NAME = "MidNight";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Sky/Midnight/Midnight.gltf";
	}

	
}