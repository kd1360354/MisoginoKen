#include "BTCondition_IsHit.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTCondition_IsHit::BTCondition_IsHit(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner)
{
}

BTStatus BTCondition_IsHit::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// 💡 フラグによる分岐
	// 被弾フラグが立っていれば Success（＝被弾アクションへ）、そうでなければ Failure（＝次の行動評価へ）
	if (spOwner->IsHit()) {
		return BTStatus::Success;
	}

	return BTStatus::Failure;
}