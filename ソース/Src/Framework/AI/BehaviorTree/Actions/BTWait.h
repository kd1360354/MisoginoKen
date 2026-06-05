#pragma once
#include "../Core/BTNode.h"
#include <memory>

class EnemyBase;

/**
 * @brief 指定時間待機するアクションノード
 */
class BTAction_Wait : public BTNode {
public:
	BTAction_Wait(const std::shared_ptr<EnemyBase>& owner, float waitTime);

	/**
	 * @brief 更新処理
	 */
	BTStatus Execute() override;

private:
	/**
	 * @brief 待機開始のセットアップ
	 */
	void PrepareWait(std::shared_ptr<EnemyBase> spOwner);

private:
	float m_waitTime; // 目標待機時間
	float m_timer;    // 現在の経過時間
};