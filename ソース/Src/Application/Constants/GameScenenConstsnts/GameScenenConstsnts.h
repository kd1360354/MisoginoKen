#pragma once
#include <string>

// =========================================================================
// シーン・環境・マップ共通定数定義 (SceneConstants.h)
// =========================================================================
namespace SceneParam {

	// --- 💻 システム・グラフィックス ---
	namespace System {
		static constexpr int   EFFECT_VIEW_W = 1280;
		static constexpr int   EFFECT_VIEW_H = 720;
		static constexpr float BGM_VOL_DEF = 0.2f;
	}

	// --- 🌌 環境設定 (夜・ライト) ---
	namespace Environment {
		namespace Night {
			static constexpr Math::Vector3 DIR_LIGHT_DIR = { 1.0f, -1.0f, 1.0f };
			static constexpr Math::Vector3 DIR_LIGHT_COLOR = { 0.15f, 0.20f, 0.25f };
			static constexpr Math::Vector4 AMBIENT_COLOR = { 0.225f, 0.275f, 0.325f, 0.45f };
		}
	}

	// --- 📸 カメラ設定 ---
	namespace Camera {
		static constexpr float FOV = 60.0f;
		static constexpr float CLIP_FAR = 2000.0f;
		static constexpr float CLIP_NEAR = 0.005f;
		static constexpr Math::Vector3 POS_DEFAULT = { 0.0f, 0.0f, 15.0f };
	}

	// --- ⚔️ ゲーム進行・フェーズ管理 ---
	namespace Progression {
		static constexpr int   P1_ENEMY_MAX = 8;
		static constexpr int   P1_MIDBOSS_TRIG = 4;
		static constexpr int   P2_ENEMY_ADD = 6;

		static constexpr float LAST_BOSS_TRIG_X = 20.0f;
		static constexpr float ACTIVATION_TRIG_Z = 35.0f;
	}

	// --- 👾 エネミー初期配置 (Phase 1) ---
	namespace EnemySpawn {
		static constexpr Math::Vector3 P1_CENTER = { 0.0f, 0.0f, -15.0f };
		static constexpr Math::Vector3 P1_FRONT = { 0.0f, 0.0f, -5.0f };
		static constexpr Math::Vector3 P1_RIGHT = { 10.0f, 0.0f, -15.0f };
		static constexpr Math::Vector3 P1_LEFT = { -10.0f, 0.0f, -15.0f };
		static constexpr Math::Vector3 P1_RF = { 5.0f, 0.0f, -25.0f };
		static constexpr Math::Vector3 P1_LF = { -5.0f, 0.0f, -25.0f };
		static constexpr Math::Vector3 P1_RO = { 5.0f, 0.0f, -5.0f };
		static constexpr Math::Vector3 P1_LO = { -5.0f, 0.0f, -5.0f };
	}

	// --- ⛩️ マップオブジェクト設定 (名前衝突回避のため Param を付与) ---
	namespace Map {

		// 🧱 壁 (Wallクラスと衝突しないよう WallParam に変更)
		namespace WallParam {
			static constexpr float Y_POS = 0.0f;
			static constexpr Math::Vector3 POS_F = { 0.0f, Y_POS, 124.0f };
			static constexpr Math::Vector3 POS_L = { -123.0f, Y_POS, 62.0f };
			static constexpr Math::Vector3 POS_B = { 0.0f, Y_POS, -125.0f };
			static constexpr Math::Vector3 POS_R = { 123.0f, Y_POS, -21.0f };

			static constexpr float ROT_Y_F = 0.0f;
			static constexpr float ROT_Y_L = -90.0f;
			static constexpr float ROT_Y_B = -180.0f;
			static constexpr float ROT_Y_R = 90.0f;

			static constexpr Math::Vector3 SCALE_FB = { 200.0f, 10.0f, 30.0f };
			static constexpr Math::Vector3 SCALE_L = { 230.0f, 10.0f, 30.0f };
			static constexpr Math::Vector3 SCALE_R = { 200.0f, 10.0f, 30.0f };
		}

		// 🛤️ 参道 (Approachクラスと衝突しないよう ApproachParam に変更)
		namespace ApproachParam {
			struct Data {
				Math::Vector3 Pos;
				Math::Vector3 Rot;
				Math::Vector3 Scale;
			};
			static constexpr Data SET_1 = { { 0.0f, 0.001f, 0.0f },    { 0.0f, 90.0f, 0.0f }, { 20.0f, 1.0f, 1.03f } };
			static constexpr Data SET_2 = { { 0.0f, 0.012f, 20.0f },   { 0.0f, 0.0f, 0.0f },  { 10.0f, 1.0f, 1.03f } };
			static constexpr Data SET_3 = { { 97.0f, 0.023f, -173.0f }, { 0.0f, 0.0f, 0.0f },  { 8.0f, 1.0f, 1.03f } };
		}

		// ⛩️ 鳥居 (Toriiクラスと衝突しないよう ToriiParam に変更)
		namespace ToriiParam {
			static constexpr float Y_POS = 0.0f;
			static constexpr float SCALE = 1.0f;
			static constexpr float X_CENTER = 0.0f;
			static constexpr float X_RIGHT = 70.0f;
			static constexpr float Z_FRONT = 35.0f;
			static constexpr float Z_BACK = -50.585f;
		}

		// 🏮 ランタン (Lanternクラスと衝突しないよう LanternParam に変更)
		namespace LanternParam {
			static constexpr float Z_FRONT = 35.0f;
			static constexpr float X_R1 = 65.0f;
			static constexpr float X_CR = 5.0f;
			static constexpr float X_CL = -5.0f;

			static constexpr int   ROW_COUNT = 6;
			static constexpr float SIDE_X_ABS = 7.5f;
			static constexpr float Z_START_NEW = -55.0f;
			static constexpr float Z_STEP = 10.0f;

			static constexpr Math::Vector3 LIGHT_COLOR = { 0.8f, 0.5f, 0.1f };
			static constexpr float         LIGHT_RADIUS = 5.0f;
		}

		// 🚧 柵 (Fenceクラスと衝突しないよう FenceParam に変更)
		namespace FenceParam {
			static constexpr Math::Vector3 POS_F1 = { 62.0f, WallParam::Y_POS, 35.0f };
			static constexpr Math::Vector3 POS_F2 = { 75.0f, WallParam::Y_POS, 35.0f };
			static constexpr Math::Vector3 POS_F3 = { -7.0f, WallParam::Y_POS, 35.0f };
			static constexpr Math::Vector3 POS_F4 = { 7.0f, WallParam::Y_POS, 35.0f };

			static constexpr Math::Vector3 POS_L = { -123.0f, WallParam::Y_POS, 62.0f };
			static constexpr Math::Vector3 POS_B = { 0.0f, WallParam::Y_POS, -125.0f };
			static constexpr Math::Vector3 POS_R = { 123.0f, WallParam::Y_POS, -21.0f };

			static constexpr float ROT_Y_F = 0.0f;
			static constexpr float ROT_Y_L = -90.0f;
			static constexpr float ROT_Y_B = -180.0f;
			static constexpr float ROT_Y_R = 90.0f;

			static constexpr Math::Vector3 SCALE_FB = { 200.0f, 10.0f, 30.0f };
			static constexpr Math::Vector3 SCALE_L = { 230.0f, 10.0f, 30.0f };
			static constexpr Math::Vector3 SCALE_R = { 200.0f, 10.0f, 30.0f };
		}
	}

	// --- 🔊 オーディオファイルパス ---
	namespace Audio {
		static constexpr const char* BGM_NOOB_BATTLE = "Asset/Audio/BGM/NoobEnemyBattleBGM.wav";
		static constexpr const char* BGM_MID_BATTLE = "Asset/Audio/BGM/MidBossBattleBGM.wav";
		static constexpr const char* BGM_BOSS_BATTLE = "Asset/Audio/BGM/BossBattleBGM.wav";
		static constexpr const char* SE_WIND_AMBIENT = "Asset/Audio/BGM/Wind.wav";
	}
}