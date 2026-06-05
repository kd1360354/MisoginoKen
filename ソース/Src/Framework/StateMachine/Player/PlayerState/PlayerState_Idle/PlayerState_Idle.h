#pragma once
#include "../PlayerState.h"

class PlayerState_Idle : public PlayerState
{
public:
	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// 待機中は移動入力による移動を完全に許可する
	bool CanMove() const override { return true; }
};