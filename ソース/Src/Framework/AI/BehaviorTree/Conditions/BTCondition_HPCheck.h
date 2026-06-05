#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 敵の残りHP割合チェックノード
 */
class BTCondition_HPCheck : public BTNode {
public:
	/**
	 * @param ratio 判定基準となるHPの割合（0.0f ～ 1.0f）
	 */
	BTCondition_HPCheck(const std::shared_ptr<EnemyBase>& owner, float ratio);

	/**
	 * @brief 条件判定の実行
	 */
	BTStatus Execute() override;

private:
	float m_ratio; // 判定基準となる比率
};