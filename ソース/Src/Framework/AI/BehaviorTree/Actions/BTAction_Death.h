#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 敵の死亡アクションノード
 */
class BTAction_Death : public BTNode {
public:
	BTAction_Death(const std::shared_ptr<EnemyBase>& owner);
	BTStatus Execute() override;
};