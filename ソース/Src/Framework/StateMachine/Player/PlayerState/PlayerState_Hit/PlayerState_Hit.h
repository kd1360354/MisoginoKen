#pragma once
#include "../PlayerState.h"

class PlayerState_Hit : public PlayerState {
public:
	enum class Phase {
		Start,
		End
	};
	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// 被弾中は移動も攻撃もできない
	bool CanMove() const override { return false; }

	// 被弾の最中にさらにのけぞる（ハメ殺し）を防ぎたい場合はここを false にする設計もアリ
	bool CanTakeDamage() const override { return true; }

private:
	Phase m_phase = Phase::Start;
};