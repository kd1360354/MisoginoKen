#include "BTCondition_HPCheck.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTCondition_HPCheck::BTCondition_HPCheck(const std::shared_ptr<EnemyBase>& owner, float ratio)
	: BTNode(owner), m_ratio(ratio)
{
}

BTStatus BTCondition_HPCheck::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// 💡 判定ロジック
	// 理由：最大HPに対する割合で判定することで、HP総量が異なる敵（雑魚・ボス）でも同じノードを再利用可能にする。
	float currentHp = spOwner->GetHp();
	float maxHp = spOwner->GetMaxHP();

	// 判定の閾値を算出（マジックナンバーを排除し、計算式を明確化）
	float threshold = maxHp * m_ratio;

	if (currentHp <= threshold) {
		return BTStatus::Success;
	}

	return BTStatus::Failure;
}