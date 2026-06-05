#include "BTRandomSelector.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"
#include <algorithm>
#include <random>
#include <vector>

// BTComposite.cpp で定義した定数と同じ値（共通ヘッダーがあればそちらへ）
static constexpr int BT_INVALID_INDEX = -1;

BTRandomSelector::BTRandomSelector(const std::shared_ptr<EnemyBase>& owner)
	: BTSelector(owner), m_lastRunningIdx(BT_INVALID_INDEX)
{
}

BTStatus BTRandomSelector::Execute() {
	if (m_children.empty()) return BTStatus::Failure;

	// 1. すでに実行中のノードがあるなら継続（目移り防止）
	if (m_lastRunningIdx != BT_INVALID_INDEX) {
		BTStatus status = m_children[m_lastRunningIdx]->Execute();

		// 完了するまでは Running を返し続け、Success/Failure になったら記憶を解除
		if (status == BTStatus::Running) return BTStatus::Running;

		m_lastRunningIdx = BT_INVALID_INDEX;
		return status;
	}

	// 2. 新しく選ぶフェーズ（シャッフル評価）
	std::vector<size_t> indices(m_children.size());
	for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;

	// 実行のたびに評価順をランダムに入れ替える
	std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));

	for (size_t idx : indices) {
		BTStatus status = m_children[idx]->Execute();

		if (status == BTStatus::Running) {
			m_lastRunningIdx = (int)idx; // 実行中ノードを保存
			return BTStatus::Running;
		}

		if (status == BTStatus::Success) return BTStatus::Success;
	}

	return BTStatus::Failure;
}