#include "BTCondition_OnceFlag.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"

BTCondition_OnceFlag::BTCondition_OnceFlag(const std::shared_ptr<EnemyBase>& owner, std::string flagName)
	: BTNode(owner), m_flagName(flagName), m_hasExecuted(false)
{
}

BTStatus BTCondition_OnceFlag::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// ★ 修正: 実行済みなら Failure
	if (m_hasExecuted) {
		return BTStatus::Failure;
	}

	// ★ 初回実行時に true にする
	m_hasExecuted = true;
	return BTStatus::Success;
}