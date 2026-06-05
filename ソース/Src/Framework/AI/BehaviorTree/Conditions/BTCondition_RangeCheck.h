#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief ターゲットとの距離判定ノード
 */
class BTCondition_InRange : public BTNode {
public:
	/**
	 * @param owner 判定を行う敵のポインタ
	 * @param range 判定基準となる距離
	 */
	BTCondition_InRange(const std::shared_ptr<EnemyBase>& owner, float range);

	/**
	 * @brief 条件判定の実行
	 */
	BTStatus Execute() override;

private:
	float m_range; // 判定しきい値
};