#pragma once
#include "../PlayerState.h"

class PlayerState_PowerAttack : public PlayerState
{
public:
	PlayerState_PowerAttack() : m_phase(PowerPhase::PreAction), m_isHitGenerated(false), m_isEffectGenerated(false), m_waitTimer(0.0f) {}

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	bool IsAttacking() const override { return true; }
	bool CanMove() const override { return false; } // 必殺技中は移動完全禁止
	// 無敵フラグはUpdate内のフェーズで動的に切り替えるので、基本はtrue（溜め中を考慮）
	bool CanTakeDamage() const override { return (m_phase == PowerPhase::PreAction); }

private:
	enum class PowerPhase {
		PreAction,
		Attack,
		End
	};

	PowerPhase m_phase;
	bool m_isHitGenerated;
	bool m_isEffectGenerated = false;
	float m_waitTimer;
};