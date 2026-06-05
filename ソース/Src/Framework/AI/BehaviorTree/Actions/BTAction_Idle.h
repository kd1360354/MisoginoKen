#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 敵の待機アクションノード
 */
class BTAction_Idle : public BTNode {
public:
	BTAction_Idle(const std::shared_ptr<EnemyBase>& owner);

	/**
	 * @brief 更新処理
	 */
	BTStatus Execute() override;
};