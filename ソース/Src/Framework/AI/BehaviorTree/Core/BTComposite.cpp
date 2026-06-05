#include "BTComposite.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h" 

// --- 💡 共通定数（ファイル内のみ有効にするため static） ---
static constexpr int BT_INVALID_INDEX = -1;

// ==========================================
// BTSelector Implementation
// ==========================================
BTSelector::BTSelector(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner)
{
}

BTStatus BTSelector::Execute() {
	for (auto& child : m_children) {
		BTStatus status = child->Execute();
		// Failure以外が見つかった時点で評価を終了
		if (status != BTStatus::Failure) return status;
	}
	return BTStatus::Failure;
}

// ==========================================
// BTSequence Implementation
// ==========================================
BTSequence::BTSequence(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner), m_lastRunningIdx(BT_INVALID_INDEX)
{
}

BTStatus BTSequence::Execute() {
	int startIdx = (m_lastRunningIdx != BT_INVALID_INDEX) ? m_lastRunningIdx : 0;

	for (int i = startIdx; i < (int)m_children.size(); ++i) {
		BTStatus status = m_children[i]->Execute();

		if (status == BTStatus::Running) {
			m_lastRunningIdx = i;
			return BTStatus::Running;
		}

		if (status == BTStatus::Failure) {
			m_lastRunningIdx = BT_INVALID_INDEX;
			return BTStatus::Failure;
		}
	}

	m_lastRunningIdx = BT_INVALID_INDEX;
	return BTStatus::Success;
}