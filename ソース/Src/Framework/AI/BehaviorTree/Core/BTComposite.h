#pragma once
#include "BTNode.h"
#include <vector>
#include <memory>

// 前方宣言：EnemyBase.hをインクルードせずに「クラスが存在すること」だけ伝える
class EnemyBase;

/**
 * @brief セレクターノード
 */
class BTSelector : public BTNode {
public:
	BTSelector(const std::shared_ptr<EnemyBase>& owner);
	BTStatus Execute() override;
	void AddChild(std::shared_ptr<BTNode> child) { m_children.push_back(child); }

protected:
	std::vector<std::shared_ptr<BTNode>> m_children;
};

/**
 * @brief シーケンスノード
 */
class BTSequence : public BTNode {
public:
	BTSequence(const std::shared_ptr<EnemyBase>& owner);
	BTStatus Execute() override;
	void AddChild(std::shared_ptr<BTNode> child) { m_children.push_back(child); }

protected:
	std::vector<std::shared_ptr<BTNode>> m_children;
	int m_lastRunningIdx;
};