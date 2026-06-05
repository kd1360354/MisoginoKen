#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 敵の被弾（怯み）アクションノード
 */
class BTAction_Hit : public BTNode {
public:
	BTAction_Hit(const std::shared_ptr<EnemyBase>& owner);

	/**
	 * @brief 更新処理
	 */
	BTStatus Execute() override;
};