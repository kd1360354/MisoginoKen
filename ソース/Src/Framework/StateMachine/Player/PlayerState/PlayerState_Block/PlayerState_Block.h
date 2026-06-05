#pragma once
#include "../PlayerState.h"

class PlayerState_Block : public PlayerState {
public:
	enum class Phase {
		Start,
		Loop,
		End
	};
	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	bool IsBlocking() const override { return true; }  // ガード中判定をON
	bool CanMove() const override { return false; } // ガード中は足が止まる


private:
	Phase m_phase = Phase::Start;
	float m_effectTimer = 0.0f;
	std::weak_ptr<KdEffekseerObject> m_wpEffect;
};