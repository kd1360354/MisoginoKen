#include "BTAction_Death.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTAction_Death::BTAction_Death(const std::shared_ptr<EnemyBase>& owner) : BTNode(owner) {}

BTStatus BTAction_Death::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// --- 1. すでに死亡アクション実行中の場合 ---
	if (spOwner->GetState() == EnemyAnimState::Death) {
		// アニメーションが最後まで再生されたかチェック
		if (spOwner->GetAnimator()->IsAnimationEnd()) {
			// ★ 完了報告：これにより Sequence ノードが次の「消滅（Dissolve）」アクションを実行できる
			return BTStatus::Success;
		}
		// まだ倒れている最中なら Running
		return BTStatus::Running;
	}

	// --- 2. 死亡アクションの開始（初フレーム） ---
	spOwner->SetState(EnemyAnimState::Death);

	// 💡 物理判定の無効化
	// 理由：倒れている最中の敵にプレイヤーが衝突し、移動を妨げられるのを防ぐため。
	if (spOwner->GetCollider()) {
		spOwner->GetCollider()->SetEnableAll(false);
	}

	// アニメーションのセット
	auto modelData = spOwner->GetModel()->GetData();
	if (modelData) {
		// "Death" を定数 ENEMY_ANIM_DEATH に置き換え
		auto deathAnim = modelData->GetAnimation(EnemyParam::Anim::IDLE);
		if (deathAnim) {
			spOwner->GetAnimator()->SetAnimation(deathAnim, false);
		}
	}

	return BTStatus::Running;
}