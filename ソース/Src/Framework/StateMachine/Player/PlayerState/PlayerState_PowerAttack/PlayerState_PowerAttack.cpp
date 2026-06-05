#include "PlayerState_PowerAttack.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"

void PlayerState_PowerAttack::Enter(Player& player)
{
	m_phase = PowerPhase::PreAction;
	m_isHitGenerated = false;
	m_isEffectGenerated = false;
	m_waitTimer = 0.0f;

	// 1. コスト消費：PlayerParam::PowerAttack::MAX_COST (100.0f)
	player.ConsumePowerCost(PlayerParam::PowerAttack::MAX_COST);

	// 2. 移動速度をリセット
	player.SetMoveSpeed(0.0f);

	// 3. 溜めアニメーション開始
	if (player.GetAnimator() && player.GetModel() && player.GetModel()->GetData()) {
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::POWER_CHARGE), false);
	}

	// 4. 溜めエフェクト発生 (Visual::Effect::PRE_POWER)
	player.PlayEffect(PlayerParam::Visual::Effect::PRE_POWER, player.GetPos());
}

void PlayerState_PowerAttack::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;
	float normTime = animator->GetNormalizedTime();

	// 必殺技中は移動不可
	player.SetMoveSpeed(0.0f);

	switch (m_phase)
	{
	case PowerPhase::PreAction:
	{
		// 溜めフェーズ
		m_waitTimer += deltaTime;

		// 最低溜め時間を超え、最大溜め時間に達したら発動
		if (m_waitTimer >= PlayerParam::PowerAttack::MIN_CHARGE_TIME)
		{
			if (m_waitTimer >= PlayerParam::PowerAttack::MAX_CHARGE_TIME)
			{
				m_phase = PowerPhase::Attack;
				if (player.GetModel()->GetData()) {
					animator->SetAnimation(
						player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::POWER_ATK), false);
				}
				player.AutoTargetClosestEnemy(); // 発動時に敵を向く
			}
		}
	}
	break;

	case PowerPhase::Attack:
		// 1. 斬撃エフェクト：発動直後に発生
		if (!m_isEffectGenerated && normTime >= PlayerParam::PowerAttack::EFK_TIME)
		{
			// エフェクト名とオフセット座標を定数から適用
			player.PlayEffect(
				PlayerParam::Visual::Effect::POWER_ATK,
				player.GetPos() + PlayerParam::Visual::Effect::POWER_ATK_OFFS);

			m_isEffectGenerated = true;
		}

		// 2. 円範囲攻撃判定：PlayerParam::PowerAttack のパラメータを使用
		if (!m_isHitGenerated && normTime >= PlayerParam::PowerAttack::HIT_TIME)
		{
			float totalDamage = (float)player.GetAttackPower() * PlayerParam::PowerAttack::DAMAGE_MULT;
			player.GenerateAreaAttack(PlayerParam::PowerAttack::RADIUS, totalDamage);
			m_isHitGenerated = true;
		}

		// 3. 終了フェーズへの遷移
		if (m_isHitGenerated && normTime >= PlayerParam::PowerAttack::END_TIME) {
			m_phase = PowerPhase::End;
		}
		break;

	case PowerPhase::End:
		// フォロースルー
		break;
	}

	// アニメーション終了時の遷移
	if (animator->IsAnimationEnd()) {
		if (m_phase == PowerPhase::End || m_isHitGenerated) {
			player.ChangeState(std::make_shared<PlayerState_Idle>());
		}
	}
}

void PlayerState_PowerAttack::Exit(Player& player)
{
	player.SetInvincible(false);
}