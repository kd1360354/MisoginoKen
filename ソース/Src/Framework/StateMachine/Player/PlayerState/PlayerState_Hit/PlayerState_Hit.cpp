#include "PlayerState_Hit.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"
// 定数ファイルのインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

void PlayerState_Hit::Enter(Player& player) {
	m_phase = Phase::Start;

	// 1. 被弾アニメーション（のけぞり）開始
	if (player.GetAnimator() && player.GetModel()) {
		// PlayerParam::Anim::HIT_START を使用
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::HIT_START), false);
	}
}

void PlayerState_Hit::Update(Player& player, float deltaTime) {
	auto animator = player.GetAnimator();
	if (!animator) return;

	// 被弾中は移動速度をゼロにして硬直を表現
	player.SetMoveSpeed(0.0f);

	switch (m_phase) {
	case Phase::Start: // 衝撃を受けた瞬間の「のけぞり」
		if (animator->IsAnimationEnd()) {
			// のけぞりが終わったら「体勢立て直し」フェーズへ
			m_phase = Phase::End;
			// PlayerParam::Anim::HIT_RECOVER を使用
			animator->SetAnimation(
				player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::HIT_RECOVER), false);
		}
		break;

	case Phase::End: // 体勢を立て直して操作可能に戻るまでの「硬直」
		if (animator->IsAnimationEnd()) {
			// 完全に立て直したら待機状態へ
			player.ChangeState(std::make_shared<PlayerState_Idle>());
		}
		break;
	}
}

void PlayerState_Hit::Exit(Player& player) {
}