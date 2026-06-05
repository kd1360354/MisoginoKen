#pragma once
#include "../Core/BTNode.h"
#include <memory>

class LastBoss;

/**
 * @brief ラスボス専用：咆哮アクションノード
 */
class BTAction_Roar : public BTNode {
public:
	BTAction_Roar(const std::shared_ptr<EnemyBase>& owner);
	BTStatus Execute() override;

private:
	void StartRoar(std::shared_ptr<LastBoss> spBoss);
	void UpdateRoarShake(std::shared_ptr<LastBoss> spBoss, float normTime);
	void FinishRoar(std::shared_ptr<LastBoss> spBoss);

private:
	bool m_isRoarShaked;
};