#pragma once
#include "../PlayerState.h"

class PlayerState_RunAttack : public PlayerState
{
public:
	enum class Phase { Start, Loop, End }; // 三段構成を維持

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	bool IsAttacking() const override { return true; }
	bool CanMove() const override { return false; } // 入力移動は禁止（踏み込み慣性のみ）

private:
	Phase m_phase = Phase::Start;
	bool m_isComboReserved = false;
	bool m_isEffectGenerated = false;
	std::weak_ptr<KdEffekseerObject> m_wpEffect;
};