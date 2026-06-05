#pragma once
// =========================================================================
// ゲーム共通定数定義 (GameConstants.h)
// =========================================================================

// --- 🏃 汎用移動・しきい値 ---
constexpr float MOVE_DIRECTION_SQUARED_THRESHOLD = 0.0001f;
static constexpr float TARGET_VEC_MIN_LENGTH_SQUARED = 0.001f;

// --- 📐 入力方向ベクトル ---
static constexpr Math::Vector3 INPUT_DIR_FORWARD = { 0, 0, 1 };
static constexpr Math::Vector3 INPUT_DIR_BACK = { 0, 0, -1 };
static constexpr Math::Vector3 INPUT_DIR_LEFT = { -1, 0, 0 };
static constexpr Math::Vector3 INPUT_DIR_RIGHT = { 1, 0, 0 };

// --- ⏱️ 時間制御 ---
static constexpr float TIME_STOP_SPEED = 0.0f;
static constexpr float TIME_NORMAL_SPEED = 1.0f;

// --- 🎨 UI/カメラ/敵認識 ---
static constexpr float HP_INTERPOLATION_SPEED_DAMAGE = 10.0f;
static constexpr float HP_INTERPOLATION_SPEED_HEAL = 5.0f;
static constexpr float HP_INTERPOLATION_THRESHOLD = 0.01f;
static constexpr float ENEMY_EFFECT_VERTICAL_OFFSET = 0.5f;
static constexpr float MAX_ENEMY_RECOGNIZE_DISTANCE = 10.0f;
static constexpr float LOCK_ON_MAX_DISTANCE = 10.0f;

// --- 💡 リムライト (敵のデフォルト色など) ---
static constexpr Math::Vector3 ENEMY_RIMLIGHT_COLOR_DEFAULT = { 0.2f, 0.2f, 0.2f };

// --- ⚙️ 物理/アニメーション ---
static constexpr float GRAVITY_ACCELERATION = 0.1f;
static constexpr float ANIM_BLEND_SPEED = 5.0f;
static constexpr float GROUND_ADJUST_HEIGHT = 0.2f;

// --- ❤️ ステータスデフォルト値 ---
static constexpr float PLAYER_MAX_HP_DEFAULT = 1260.0f;
static constexpr float PLAYER_MAX_STAMINA_DEFAULT = 196.0f;

// --- 🔩 レイキャスト/プッシュバック ---
static constexpr float RAY_STEP_HEIGHT_LIMIT = 0.5f;
static constexpr float RAY_MIN_RANGE = 0.2f;
static constexpr float PUSHBACK_MIN_SQUARED_LENGTH = 0.0f;

// プッシュバック球体のデフォルト値
static constexpr Math::Vector3 PUSHBACK_SPHERE_LOW_POS_DEFAULT = { 0.0f, 0.5f, 0.0f };
static constexpr float PUSHBACK_SPHERE_RADIUS_DEFAULT = 0.5f;
static constexpr Math::Vector3 PUSHBACK_SPHERE_UPPER_POS_DEFAULT = { 0.0f, 0.75f, 0.0f };

// --- 📐 画面設定 (共通) ---
static constexpr int SCREEN_WIDTH = 1280;
static constexpr int SCREEN_HEIGHT = 720;
static constexpr int ARROW_SIZE = 64; // 矢印の幅と高さ (64x64)
static const int GAMEOVER_SIZE_W = 244;
static const int GAMEOVER_SIZE_H = 64;
// --- 💡 点滅パラメータ (共通) ---
static constexpr float BLINK_SPEED_DEFAULT = 0.02f;
static constexpr float BLINK_CYCLE_DEFAULT = 6.28f; // 2 * PI
static constexpr float BLINK_ALPHA_MIN = 0.3f;
static constexpr float BLINK_ALPHA_MAX = 1.0f;

