#pragma once
#include "../PlayerState.h"

class PlayerState_Potion : public PlayerState
{
public:
	enum class Phase {
		Start,  // 瓶を取り出して飲み始める
		Loop,   // 回復エフェクト発生・HP加算
		End     // 瓶をしまう・硬直
	};

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// Startフェーズ（飲み始め）だけ移動を許可し、それ以外は止める設計にします
	// ※Update内で制御するので、ここでは「基本は動けない」としておき、
	//   Player::UpdatePhysics側でこのフラグを参照させます。
	bool CanMove() const override { return false; }
private:
	Phase m_phase = Phase::Start;
};