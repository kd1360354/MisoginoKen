#pragma once
#include <string>

// =========================================================================
// マップ共通定数定義 (MapConstants.h)
// =========================================================================
namespace MapParam {

	// --- 🛤️ 地面・参道 (Ground / Approach) ---
	namespace GroundParam {
		static constexpr const char* NAME = "Ground";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Ground/Ground.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { 0.0f, 0.0f, 0.0f };
		static constexpr Math::Vector3 INITIAL_SCALE = { 125.0f, 1.0f, 125.0f };
		static constexpr const char* COLLIDER_NAME = "Stage01";
		static constexpr Math::Vector2 UV_TILING = { 300.0f, 300.0f };
	}

	namespace ApproachParam {
		static constexpr const char* NAME = "Approach";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Ground/Approach.gltf";
		static constexpr Math::Vector2 UV_TILING = { 5.0f, 5.0f };
	}

	// --- 🔥 焚き火 (BonFire) ---
	namespace BonfireParam {
		static constexpr const char* NAME = "Bonfire";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/BonFire/BonFire.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { -60.0f, 0.0f, -7.0f };
		static constexpr Math::Vector3 INITIAL_SCALE = { 0.75f, 0.75f, 0.75f };

		// 💡 ライト設定
		static constexpr Math::Vector3 COLOR_INACTIVE = { 1.0f, 0.6f, 0.1f };
		static constexpr float         RADIUS_INACTIVE = 5.0f;
		static constexpr int           IS_BRIGHT_INACTIVE = 1;
	}

	// --- 💧 手水舎 (Chozuya) ---
	namespace ChozuyaParam {
		static constexpr const char* NAME = "Chozuya";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Chozuya/Chozuya.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { -75.0f, 0.0f, 20.0f };
	}

	// --- 🏯 塔 (Pagoda) ---
	namespace GojuParam { // 五重塔
		static constexpr const char* NAME = "GojunoTou";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Tou/Pagoda_Goju_57m.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { -70.0f, 0.0f, 100.0f };
	}

	namespace SanjuParam { // 三重塔
		static constexpr const char* NAME = "SanjunoTou";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Tou/Pagoda_Sanju_33m.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { 70.0f, 0.0f, 100.0f };
	}

	// --- ⛩️ 本殿 (Honden) ---
	namespace HondenParam {
		static constexpr const char* NAME = "Honden";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Honden/Honden.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { 0.0f, 0.0f, 100.0f };
		static constexpr Math::Vector3 INITIAL_SCALE = { 0.5f, 0.5f, 0.5f };
	}

	// --- 🏮 ランタン (Lantern) ---
	namespace LanternParam {
		static constexpr const char* NAME = "Lantern";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/lantern/lantern1.gltf";
		static constexpr float       LIGHT_Y_OFFSET = 1.5f;

		// 💡 点灯時
		static constexpr Math::Vector3 COLOR_ACTIVE = { 1.0f, 0.8f, 0.6f };
		static constexpr float         RADIUS_ACTIVE = 10.0f;
		static constexpr int           IS_BRIGHT_ACTIVE = 1;

		// 💡 消灯時
		static constexpr Math::Vector3 COLOR_INACTIVE = { 0.01f, 0.01f, 0.01f };
		static constexpr float         RADIUS_INACTIVE = 0.0f;
		static constexpr int           IS_BRIGHT_INACTIVE = 0;
	}

	// --- 🌸 植物 (Sakura / Grass) ---
	namespace SakuraParam {
		static constexpr const char* NAME = "Sakura";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Sakura/Sakura.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { 60.0f, 0.0f, -7.0f };
	}

	namespace GrassParam { // GROSS から Grass へ修正
		static constexpr const char* NAME = "Grass";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Grass/Grass.gltf";
		static constexpr Math::Vector3 INITIAL_POS = { 0.0f, 0.0f, 0.0f };
	}

	// --- ⛩️ 鳥居 (Torii) ---
	namespace ToriiParam {
		static constexpr const char* NAME = "Torii";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Torii/Torii.gltf";
	}

	// --- 🧱 壁・柵 (Wall / Fence) ---
	namespace WallParam {
		static constexpr const char* NAME = "Wall";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Wall/Wall.gltf";
	}

	namespace FenceParam {
		static constexpr const char* NAME = "Fence";
		static constexpr const char* MODEL_PATH = "Asset/Models/Map/Stage1/Fence/Fence.gltf";
	}
}