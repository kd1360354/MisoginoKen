#include "BTCondition_RangeCheck.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"
BTCondition_InRange::BTCondition_InRange(const std::shared_ptr<EnemyBase>& owner, float range)
	: BTNode(owner), m_range(range)
{
}

BTStatus BTCondition_InRange::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// ★ 修正2: SceneManager ではなく EnemyBase が持つターゲット(Player等)を参照する
	auto spTarget = spOwner->GetTarget().lock();
	if (!spTarget) return BTStatus::Failure;

	// ★ 修正3: spPlayer->GetPos() を spTarget->GetPos() に変更
	float dist = (spTarget->GetPos() - spOwner->GetPos()).Length();

	if (dist <= m_range) {
		return BTStatus::Success;
	}
	return BTStatus::Failure;
}