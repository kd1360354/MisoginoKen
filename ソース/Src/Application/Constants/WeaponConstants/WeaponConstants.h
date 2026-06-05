#pragma once

// =========================================================================
// 武器共通定数定義 (WeaponConstants.h)
// =========================================================================

// --- ⚔️ ノード名 (ボーン名) ---
static constexpr const char* NODE_NAME_LEFT_WEAPON = "LeftWeaponPoint";
static constexpr const char* NODE_NAME_RIGHT_WEAPON = "RightWeaponPoint";
static constexpr const char* NODE_NAME_HIP_WEAPON = "HipWeaponPoint";

// --- 💥 攻撃コライダー (デフォルト値) ---
static constexpr Math::Vector3 ATTACK_BOX_CENTER_DEFAULT = { 0.0f, 0.0f, 0.1f };
static constexpr Math::Vector3 ATTACK_BOX_EXTENTS_DEFAULT = { 0.05f, 0.05f, 0.05f };

// --- ⚙️ ヒット処理 ---
static constexpr float HIT_STOP_DURATION_DEFAULT = 0.1f;

// --- 💡 攻撃識別子 ---
static constexpr int ATTACK_IDENTIFIER_DEFAULT = 0; // 左右武器/Katanaで共通