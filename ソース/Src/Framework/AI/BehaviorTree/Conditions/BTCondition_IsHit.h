#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 敵の被弾フラグチェックノード
 */
class BTCondition_IsHit : public BTNode {
public:
	BTCondition_IsHit(const std::shared_ptr<EnemyBase>& owner);
	BTStatus Execute() override;
};