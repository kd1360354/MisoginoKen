#pragma once
#include "../PlayerState.h"

class PlayerState_Attack : public PlayerState
{
public:
	PlayerState_Attack(int comboCount = 1) : m_comboCount(comboCount) {}

	void Enter(Player& player) override;
	void Update(Player& player, float deltaTime) override;
	void Exit(Player& player) override;

	// =========================================================
	// ステートの性質をオーバーライド
	// =========================================================
	bool IsAttacking() const override { return true; }  // 攻撃中として判定
	bool CanMove() const override { return false; } // 攻撃中は移動入力を受け付けない
	// 攻撃の出だしなど、スパアマ（スーパーアーマー）を付けたい場合は 
	// ここで CanTakeDamage を調整することも可能ですが、今はデフォルト(true)でOK

private:
	std::string GetComboEffectName();
	int m_comboCount = 1;
	bool m_isComboReserved = false;
	bool m_isEffectGenerated = false;
	std::weak_ptr<KdEffekseerObject> m_wpEffect;
};