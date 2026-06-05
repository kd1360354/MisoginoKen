#pragma once
#include "../Core/BTNode.h"
#include <memory>
#include <string>

class EnemyBase;
enum class EnemyAnimState; // 列挙型の前方宣言

/**
 * @brief 敵の攻撃アクションノード
 */
class BTAction_Attack : public BTNode {
public:
	BTAction_Attack(const std::shared_ptr<EnemyBase>& owner, EnemyAnimState state, std::string animName, float multiplier);

	/**
	 * @brief 更新処理
	 */
	BTStatus Execute() override;

private:
	EnemyAnimState m_targetState; // 遷移先のステート
	std::string    m_animName;    // 再生するアニメーション名
	float          m_multiplier;  // 技の威力倍率
};