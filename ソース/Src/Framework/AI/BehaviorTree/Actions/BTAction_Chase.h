#pragma once
#include "../Core/BTNode.h"
#include <memory>

// 前方宣言：具体的な中身は .cpp で読み込む
class EnemyBase;

/**
 * @brief 敵の追跡アクションノード
 */
class BTAction_Chase : public BTNode {
public:
	BTAction_Chase(const std::shared_ptr<EnemyBase>& owner);

	/**
	 * @brief 更新処理
	 */
	BTStatus Execute() override;

private:
	/**
	 * @brief 追跡の中断と停止処理
	 */
	void StopChase(std::shared_ptr<EnemyBase> spOwner);
};