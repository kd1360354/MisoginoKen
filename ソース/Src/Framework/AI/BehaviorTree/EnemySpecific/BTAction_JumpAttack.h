#pragma once
#include "../Core/BTNode.h"
#include <memory>

class LastBoss; // 前方宣言

/**
 * @brief ラスボス専用：ジャンプ攻撃アクションノード
 */
class BTAction_JumpAttack : public BTNode {
public:
	BTAction_JumpAttack(const std::shared_ptr<EnemyBase>& owner);

	/**
	 * @brief 更新処理
	 */
	BTStatus Execute() override;

private:
	void StartJumpAttack(std::shared_ptr<LastBoss> spBoss);
	void ExecuteAirWarp(std::shared_ptr<LastBoss> spBoss);
	void ExecuteLandImpact(std::shared_ptr<LastBoss> spBoss);
	void FinishJumpAttack(std::shared_ptr<LastBoss> spBoss);

private:
	bool m_hasWarped; // ワープ済みフラグ
	bool m_hasEffect; // 衝撃波発生済みフラグ
};