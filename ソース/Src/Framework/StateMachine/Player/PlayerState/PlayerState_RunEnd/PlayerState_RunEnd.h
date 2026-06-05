#pragma once
#include "../PlayerState.h"

class PlayerState_RunEnd : public PlayerState
{
public:
	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// 止まるモーション中も、再入力によるキャンセル移動を許可する
	bool CanMove() const override { return true; }
};