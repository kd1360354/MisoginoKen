#include "BTAction_Dissolve.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"
#include "../../../../Application/Constants/CharactorConstants/Enemy/EnemyConstants.h"

BTAction_Dissolve::BTAction_Dissolve(const std::shared_ptr<EnemyBase>& owner, float speed)
	: BTNode(owner), m_dissolveSpeed(speed)
{
}

BTStatus BTAction_Dissolve::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// --- 1. 開始時の初期設定 ---
	// 修正ポイント：LB_... ではなく共通定数 ENEMY_DISSOLVE_MIN (0.0f) を使用
	if (spOwner->GetDissolveThreshold() <= EnemyParam::Visual::DISSOLVE_MIN) {
		if (spOwner->GetCollider()) {
			spOwner->GetCollider()->SetEnableAll(false);
		}
	}

	// --- 2. アニメーションの最終更新 ---
	if (spOwner->GetAnimator()) {
		spOwner->GetAnimator()->AdvanceTime(spOwner->GetModel()->WorkNodes());
	}

	// --- 3. ディゾルブ進捗更新 ---
	float current = spOwner->GetDissolveThreshold();
	current += m_dissolveSpeed;
	spOwner->SetDissolveThreshold(current);

	// --- 4. 完了チェック（モデル消失 + エフェクト終了） ---
	// 修正ポイント：1.0f ではなく共通定数 ENEMY_DISSOLVE_MAX を使用
	if (current >= EnemyParam::Visual::DISSOLVE_MAX) {
		spOwner->SetDissolveThreshold(EnemyParam::Visual::DISSOLVE_MAX);

		int handle = spOwner->GetDeathEffectHandle();
		if (!KdEffekseerManager::GetInstance().IsPlaying(handle)) {
			spOwner->Expire();
			return BTStatus::Success;
		}
	}

	return BTStatus::Running;
}