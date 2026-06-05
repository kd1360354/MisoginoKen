#include "BTAction_Hit.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTAction_Hit::BTAction_Hit(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner)
{
}

BTStatus BTAction_Hit::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// --- 1. すでに被弾アクション実行中の場合 ---
	if (spOwner->GetState() == EnemyAnimState::Hit) {
		// アニメーションが最後まで再生されたかチェック
		if (spOwner->GetAnimator()->IsAnimationEnd()) {
			// 💡 被弾フラグのリセット
			// 理由：怯み動作が完了したことをAIに知らせ、次フレームからセレクターが別の行動を選べるようにするため。
			spOwner->SetHitFlag(false);

			return BTStatus::Success;
		}
		// まだのけぞり中なら Running を維持
		return BTStatus::Running;
	}

	// --- 2. 被弾アクションの開始（初フレーム） ---
	spOwner->SetState(EnemyAnimState::Hit);

	auto modelData = spOwner->GetModel()->GetData();
	if (modelData) {
		// "Hit" を定数 ENEMY_ANIM_HIT に置き換え（EnemyConstants.h 準拠）
		auto hitAnim = modelData->GetAnimation(EnemyParam::Anim::HIT);
		if (hitAnim) {
			// 被弾アニメはループさせない（false）
			spOwner->GetAnimator()->SetAnimation(hitAnim, false);
		}
	}

	return BTStatus::Running;
}