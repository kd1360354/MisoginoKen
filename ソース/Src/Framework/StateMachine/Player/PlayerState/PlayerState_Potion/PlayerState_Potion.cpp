#include "PlayerState_Potion.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"
// 定数ファイルのインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"


void PlayerState_Potion::Enter(Player& player)
{
	m_phase = Phase::Start;

	// 1. 飲み始めのアニメーションを設定
	if (player.GetAnimator()) {
		player.GetAnimator()->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::POTION), false);
	}

	// アイテム使用音
	// player.PlaySound(PLAYER_SE_POTION_DRINK);
}

void PlayerState_Potion::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;

	switch (m_phase)
	{
	case Phase::Start:
		// このフェーズは CanMove() が true を返すため、Player::UpdatePhysics 側で移動が処理されます。

		// 瓶を口に運ぶアニメが終わったら回復フェーズへ
		if (animator->IsAnimationEnd()) {
			m_phase = Phase::Loop;

			// --- HP回復処理 ---
			float currentHp = player.GetHP();
			float maxHp = player.GetMaxHP();
			float healAmount = maxHp * player.GetPotionHealRate();

			player.SetHp(std::min(currentHp + healAmount, maxHp));
			player.SetPotionCount(player.GetPotionCount() - 1);

			// 回復演出（Buff）アニメーションへ遷移
			animator->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::BUFF), false);

			// 回復エフェクト発生
			// player.PlayEffect(PLAYER_EFK_HEAL, player.GetPos());
		}
		break;

	case Phase::Loop:
		// このフェーズから CanMove() が false になるため、足が止まります。
		if (animator->IsAnimationEnd()) {
			m_phase = Phase::End;
		}
		break;

	case Phase::End:
		// 硬直が解けたら待機状態へ
		player.ChangeState(std::make_shared<PlayerState_Idle>());
		break;
	}
}

void PlayerState_Potion::Exit(Player& player) {}