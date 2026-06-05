#include "PlayerState_Jump.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"
#include "../PlayerState_Dodge/PlayerState_Dodge.h"
// 定数ファイルのインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"


void PlayerState_Jump::Enter(Player& player)
{
	m_phase = JumpPhase::Start;

	// 1. 上昇アニメーション開始
	if (player.GetAnimator() && player.GetModel()->GetData()) {
		player.GetAnimator()->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::JUMP_START), false);
	}

	// 2. 上昇力を与える（重力の符号を反転させて上向きの力を加える）
	player.SetGravity(-player.GetJumpPower());

	// 3. 接地判定をオフ
	player.SetIsGround(false);
}

void PlayerState_Jump::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;

	switch (m_phase)
	{
	case JumpPhase::Start: // --- フェーズ1: 上昇中 ---
		if (animator->IsAnimationEnd())
		{
			m_phase = JumpPhase::Loop;
			animator->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::JUMP_LOOP), true);
		}
		break;

	case JumpPhase::Loop: // --- フェーズ2: 滞空・下降中 ---
		// 接地した瞬間に着地アニメーションへ
		if (player.IsGround())
		{
			m_phase = JumpPhase::End;
			animator->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::JUMP_END), false);
			// player.PlaySound(PLAYER_SE_LAND);
		}
		break;

	case JumpPhase::End: // --- フェーズ3: 着地硬直中 ---
		// 着地硬直を回避でキャンセル
		if (KdInputManager::Instance().IsPress(APP_BTN_DODGE))
		{
			player.ChangeState(std::make_shared<PlayerState_Dodge>());
			return;
		}

		// アニメーションが終わったら待機へ
		if (animator->IsAnimationEnd())
		{
			player.ChangeState(std::make_shared<PlayerState_Idle>());
		}
		break;
	}
}

void PlayerState_Jump::Exit(Player& player) {}