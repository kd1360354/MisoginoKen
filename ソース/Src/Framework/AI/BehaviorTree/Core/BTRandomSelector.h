#pragma once
#include "BTComposite.h"

/**
 * @brief ランダムセレクターノード
 */
class BTRandomSelector : public BTSelector {
public:
	BTRandomSelector(const std::shared_ptr<EnemyBase>& owner);

	/**
	 * @brief ノードの実行
	 */
	BTStatus Execute() override;

private:
	int m_lastRunningIdx; // 実行中の子ノードのインデックス
};