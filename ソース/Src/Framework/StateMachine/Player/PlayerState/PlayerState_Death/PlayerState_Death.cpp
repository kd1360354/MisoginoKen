#include "PlayerState_Death.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../../../../../Application/Scene/SceneManager.h"
#include "../../../../../Application/Scene/ResultScene/ResultScene.h"
// 定数ファイルのインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

void PlayerState_Death::Enter(Player& player)
{
	m_phase = DeathPhase::KnockStart;
	m_deathTimer = 0.0f;

	// 1. 初動：吹き飛んで倒れるアニメーションを開始
	if (player.GetAnimator() && player.GetModel()) {
		// PlayerParam::Anim::DEATH_START を使用
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::DEATH_START), false);
	}
}

void PlayerState_Death::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;

	switch (m_phase)
	{
	case DeathPhase::KnockStart:
		// 倒れきったらループ（絶命待機）フェーズへ
		if (animator->IsAnimationEnd()) {
			m_phase = DeathPhase::KnockLoop;
			// PlayerParam::Anim::DEATH_LOOP を使用
			animator->SetAnimation(
				player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::DEATH_LOOP), true);
		}
		break;

	case DeathPhase::KnockLoop:
		// 倒れたまま指定時間待機。プレイヤーに敗北を確認させる時間。
		m_deathTimer += deltaTime;

		// PlayerParam::Anim::KNOCKDOWN_DUR (5.0f) を使用
		if (m_deathTimer >= PlayerParam::Anim::KNOCKDOWN_DUR) {
			m_phase = DeathPhase::DeathAnim;
			// 待機終了後、最終的な死亡アニメを再生
			// PlayerParam::Anim::DEATH_FINAL を使用
			animator->SetAnimation(
				player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::DEATH_FINAL), false);
		}
		break;

	case DeathPhase::DeathAnim:
		// 最終アニメーション終了後、ゲームオーバー画面へ遷移
		if (animator->IsAnimationEnd()) {
			SceneManager::Instance().StopCurrentSceneBGM();
			SceneManager::Instance().SetNextScene(SceneManager::SceneMode::Result);
			SceneManager::Instance().ChangeScene<ResultScene>();
		}
		break;
	}
}

void PlayerState_Death::Exit(Player& player)
{
}