#pragma once
#include "../PlayerState.h"

class PlayerState_Dodge : public PlayerState
{
public:
	enum class DodgePhase {
		Start,
		Loop, // 実質的な無敵時間
		End   // 回避後の硬直
	};

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	bool IsDodge() const override { return true; }  // 回避中判定
	bool CanMove() const override { return false; } // 入力による移動を禁止（慣性のみ）
	bool CanTakeDamage() const override { return false; } // 基本的に無敵（ステート中無敵にする設計）

private:
	DodgePhase m_phase = DodgePhase::Start;
	bool m_isBackDodge = false;
};