#pragma once

static constexpr std::string_view APP_AXIS_MOVE = "Move";
static constexpr std::string_view APP_AXIS_CAMERA = "Camera";	
static constexpr std::string_view APP_BTN_ATTACK = "Attack";
static constexpr std::string_view APP_BTN_GUARD = "Guard";
static constexpr std::string_view APP_BTN_DODGE = "Dodge";
static constexpr std::string_view APP_BTN_JUMP = "Jump";
static constexpr std::string_view APP_BTN_POTION = "Potion";
static constexpr std::string_view APP_BTN_POWER_ATK = "PowerAttack";
static constexpr std::string_view APP_BTN_LOCKON = "LockOn";
static constexpr std::string_view APP_BTN_LALT = "LALT";
static constexpr std::string_view APP_BTN_RALT = "RALT";
static constexpr std::string_view APP_BTN_TARGET_SW = "TargetSwitch";
static constexpr std::string_view APP_BTN_MODE_CHANGE = "ModeChange";
static constexpr std::string_view APP_BTN_CAMERA_RESET = "CameraReset";
static constexpr std::string_view APP_AXIS_WHEEL = "CameraWheel";
static constexpr std::string_view APP_BTN_RCLICK = "CameraRClick";
static constexpr std::string_view APP_BTN_MOVE_FRONT = "FreeCamFRONT";
static constexpr std::string_view APP_BTN_MOVE_BACK = "FreeCamBACK";
static constexpr std::string_view APP_BTN_MOVE_LEFT = "FreeCamLEFT";
static constexpr std::string_view APP_BTN_MOVE_RIGHT = "FreeCamRIGHT";

namespace InputThreshold
{
	// スティックを「倒した」とみなす最小値
	static constexpr float MOVE_SQUARED_THRESHOLD = 0.01f;

	// ターゲット切り替え等の「フリック・弾き」判定しきい値
	static constexpr float STICK_FLICK = 0.7f;

	// ロックオン切り替え等で使う強い倒し込みのしきい値
	static constexpr float STICK_SWITCH_TARGET = 0.8f;

	// スティックが中央付近に戻ったとみなすしきい値
	static constexpr float STICK_NEUTRAL = 0.1f;
}