#pragma once

/**
 * @brief ノードの実行結果ステート
 */
enum class BTStatus {
	Success,    // 成功（ノードの目的を達成した）
	Failure,    // 失敗（条件を満たさなかった）
	Running     // 実行中（まだ処理が続いている）
};

class EnemyBase;

/**
 * @brief ビヘイビアツリーの最小単位（基底クラス）
 * 理由：すべての行動や条件判断のインターフェースを統一し、多態性によって柔軟なAI構築を可能にするため。
 */
class BTNode {
public:
	BTNode(const std::shared_ptr<EnemyBase>& owner) : m_wpOwner(owner) {}
	virtual ~BTNode() {}

	/**
	 * @brief ノードの更新処理
	 * @return 実行結果（Success / Failure / Running）
	 */
	virtual BTStatus Execute() = 0;

protected:
	std::weak_ptr<EnemyBase> m_wpOwner; // 持ち主（敵）への参照
};