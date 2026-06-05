#include "BTAction_Attack.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTAction_Attack::BTAction_Attack(const std::shared_ptr<EnemyBase>& owner, EnemyAnimState state, std::string animName, float multiplier)
	: BTNode(owner), m_targetState(state), m_animName(animName), m_multiplier(multiplier)
{
}

BTStatus BTAction_Attack::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// --- 1. 攻撃動作中の更新・終了チェック ---
	if (spOwner->GetState() == m_targetState) {
		// アニメーションが最後まで再生されたか確認
		if (spOwner->GetAnimator()->IsAnimationEnd()) {
			// 待機状態へステートだけ戻す(攻撃範囲内は攻撃を維持させるためにステートだけ変更)
			spOwner->SetState(EnemyAnimState::Idle);
			// 攻撃一回分が完了したので成功を返す
			return BTStatus::Success;
		}
		// まだ再生中なら Running を維持
		return BTStatus::Running;
	}

	// --- 2. 攻撃開始の事前チェック（距離判定） ---
	auto spTarget = spOwner->GetTarget().lock();
	if (spTarget) {
		float dist = (spTarget->GetPos() - spOwner->GetPos()).Length();
		if (dist > spOwner->GetAttackRadius()) return BTStatus::Failure;
	}

	// --- 3. 攻撃開始（最初の1フレームのみ実行） ---
	// 内部状態とヒットフラグのリセット
	spOwner->SetState(m_targetState);
	spOwner->SetHasHitPlayerInCurrentAttack(false);

	// 💡 技固有の倍率を EnemyBase にセット（SphierAttack側で計算に利用される）
	spOwner->SetCurrentAttackPower(m_multiplier);

	// アニメーションの切り替え
	auto modelData = spOwner->GetModel()->GetData();
	if (modelData) {
		auto attackAnim = modelData->GetAnimation(m_animName);
		if (attackAnim) {
			spOwner->GetAnimator()->SetAnimation(attackAnim, false);
		}
	}

	// 開始した直後なので Running を返す
	return BTStatus::Running;
}