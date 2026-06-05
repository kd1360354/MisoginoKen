#pragma once
#include "../PlayerState.h"

class PlayerState_Death : public PlayerState
{
public:
	enum class DeathPhase {
		KnockStart, // 吹き飛び・倒れ込み
		KnockLoop,  // 倒れたまま待機（絶命）
		DeathAnim,  // 最終アニメーション
	};

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// 死亡中は移動も攻撃も被弾も一切受け付けない
	bool IsAttacking() const override { return false; }
	bool CanMove() const override { return false; }
	bool CanTakeDamage() const override { return false; } // 死体に追い打ちは不要

private:
	DeathPhase m_phase = DeathPhase::KnockStart;
	float m_deathTimer = 0.0f;
};