#include "PlayerState_Dodge.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"

#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

void PlayerState_Dodge::Enter(Player& player)
{
	m_phase = DodgePhase::Start;

	// 1. スタミナ消費
	player.ConsumeStamina(player.GetDodgeStaminaCost());

	// 2. 回避方向の確定
	if (player.IsLockOn()) {
		m_isBackDodge = true;
	}
	else {
		m_isBackDodge = !player.IsMovingForward();
	}

	// 3. アニメーション開始：PlayerParam::Anim 名前空間から取得
	std::string animName = m_isBackDodge ? PlayerParam::Anim::DODGE_B : PlayerParam::Anim::DODGE_F;

	if (player.GetModel() && player.GetModel()->GetData()) {
		auto anim = player.GetModel()->GetData()->GetAnimation(animName);
		if (anim) {
			player.GetAnimator()->SetAnimation(anim, false);
		}
	}

	// 4. 無敵開始
	player.SetInvincible(true);
}

void PlayerState_Dodge::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;

	float normTime = animator->GetNormalizedTime();

	// --- 回避物理の更新：PlayerParam::Dodge 名前空間を使用 ---
	// 特定区間（0.05f ～ 0.6f）のみ移動力を加える
	if (normTime >= PlayerParam::Dodge::MOVE_START_TIME && normTime <= PlayerParam::Dodge::MOVE_END_TIME)
	{
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player.GetAngle()));
		Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), rotMat);

		Math::Vector3 moveDir = m_isBackDodge ? -forward : forward;

		// 慣性移動として速度を加算 (PlayerParam::Dodge::MOVE_POWER を使用)
		player.AddVelocity(moveDir * PlayerParam::Dodge::MOVE_POWER);
	}

	// アニメーション終了で待機状態へ自動遷移
	if (animator->IsAnimationEnd()) {
		player.ChangeState(std::make_shared<PlayerState_Idle>());
	}
}

void PlayerState_Dodge::Exit(Player& player)
{
	// 被弾割り込み時なども含め、確実に無敵を解除
	player.SetInvincible(false);
}