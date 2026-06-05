#pragma once
#include "../PlayerState.h"

class PlayerState_Run : public PlayerState
{
public:
	enum class RunPhase {
		Start,
		Loop,
		End
	};

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// Run状態は移動入力による座標更新を完全に許可する
	bool CanMove() const override { return true; }

private:
	RunPhase m_phase = RunPhase::Start;
};