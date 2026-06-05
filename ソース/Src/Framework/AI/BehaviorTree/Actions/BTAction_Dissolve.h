#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 敵の消滅（ディゾルブ）アクションノード
 */
class BTAction_Dissolve : public BTNode {
public:
	BTAction_Dissolve(const std::shared_ptr<EnemyBase>& owner, float speed = 0.01f);
	BTStatus Execute() override;

private:
	float m_dissolveSpeed;
};