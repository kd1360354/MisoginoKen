#include "BTAction_Idle.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTAction_Idle::BTAction_Idle(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner)
{
}

BTStatus BTAction_Idle::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// --- 1. 冗長な処理の回避 ---
	// 既に Idle 状態であれば Success を返して終了。
	// 理由：フレームごとの SetAnimation 呼び出し等による処理負荷を抑えるため。
	if (spOwner->GetState() == EnemyAnimState::Idle) {
		return BTStatus::Success;
	}

	// --- 2. ステートと移動速度のリセット ---
	spOwner->SetState(EnemyAnimState::Idle);

	// 物理的な移動を完全に停止させる（定数 0.0f）
	spOwner->SetSpeed(0.0f);

	// --- 3. アニメーションの切り替え ---
	auto modelData = spOwner->GetModel()->GetData();
	if (modelData) {
		// "Idle" を定数 ENEMY_ANIM_IDLE に置き換え（EnemyConstants.h 準拠）
		auto anim = modelData->GetAnimation(EnemyParam::Anim::IDLE);
		if (anim) {
			// 💡 待機アニメーションはループ再生（true）としてセット
			spOwner->GetAnimator()->SetAnimation(anim, true);
		}
	}

	// 即座に成功を返し、セレクターが次フレームで再度評価できるようにする
	return BTStatus::Running;
}