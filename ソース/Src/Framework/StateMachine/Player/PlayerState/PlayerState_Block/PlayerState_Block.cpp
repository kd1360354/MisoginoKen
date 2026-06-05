#include "PlayerState_Block.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"

#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"




void PlayerState_Block::Enter(Player& player) {
	m_phase = Phase::Start;

	// ガード開始アニメーション再生 (PlayerParam::Anim::BLOCK_START)
	if (player.GetAnimator() && player.GetModel()) {
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::BLOCK_START), false);
	}
}

void PlayerState_Block::Update(Player& player, float deltaTime) {
	auto animator = player.GetAnimator();
	if (!animator) return;

	// ガードアクション中は移動速度をゼロに
	player.SetMoveSpeed(0.0f);

	switch (m_phase) {
	case Phase::Start:
		// 開始アニメーションが終わったらループへ移行 (PlayerParam::Anim::BLOCK_LOOP)
		if (animator->IsAnimationEnd()) {
			m_phase = Phase::Loop;
			animator->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::BLOCK_LOOP), true);
		}
		break;

	case Phase::Loop:
		// ガード維持中はスタミナ回復の開始を遅らせる
		player.ResetStaminaRecoveryDelay();

		// --- 1. エフェクトの定期発生 (PlayerParam::Visual::Effect配下を使用) ---
		if (m_effectTimer <= 0.0f) {
			m_wpEffect = KdEffekseerManager::GetInstance().Play(
				PlayerParam::Visual::Effect::BLOCK_LOOP, player.GetPos());

			// エフェクト発生間隔を定数から設定
			m_effectTimer = PlayerParam::Visual::Effect::BLOCK_INTERVAL;
		}
		m_effectTimer -= deltaTime;

		// --- 2. 発生中のエフェクトをプレイヤーの正面に追従させる ---
		if (auto spEffect = m_wpEffect.lock()) {
			Math::Matrix playerRot = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player.GetAngle()));

			// プレイヤー前方へのオフセット距離 (PlayerParam::Visual::Effect::BLOCK_FWD_OFFS)
			Math::Matrix effectWorld =
				Math::Matrix::CreateTranslation(0.0f, 0.0f, PlayerParam::Visual::Effect::BLOCK_FWD_OFFS) * playerRot * Math::Matrix::CreateTranslation(player.GetPos());

			KdEffekseerManager::GetInstance().SetWorldMatrix(spEffect->GetHandle(), effectWorld);
		}

		// ボタンを離したら解除フェーズへ (PlayerParam::Anim::BLOCK_END)
		if (!KdInputManager::Instance().IsHold(APP_BTN_GUARD)) {
			m_phase = Phase::End;
			animator->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::BLOCK_END), false);
		}
		break;

	case Phase::End:
		// 解除アニメーションが終わったら待機状態へ
		if (animator->IsAnimationEnd()) {
			player.ChangeState(std::make_shared<PlayerState_Idle>());
		}
		break;
	}
}

void PlayerState_Block::Exit(Player& player) {
	// 必要に応じてエフェクトの停止処理などを記述
}