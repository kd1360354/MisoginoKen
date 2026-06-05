#pragma once
#include "../PlayerState.h"

class PlayerState_Jump : public PlayerState
{
public:
	enum class JumpPhase {
		Start, // 飛び上がり（上昇中）
		Loop,  // 滞空（落下中）
		End    // 着地モーション
	};

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// --- 判定関数のオーバーライド ---
	// ジャンプ中は「自由移動(CanMove)」を false にし、
	// 空中制御（慣性や微調整）は Player 側の UpdatePhysics で行う設計にします。
	bool CanMove() const override { return false; }

	// 必要であれば「空中攻撃」の判定などもここに追加できます。

private:
	JumpPhase m_phase = JumpPhase::Start;
};