#pragma once
#include "../Core/BTNode.h"
#include <string>
#include <set>
#include <memory>

class EnemyBase;

/**
 * @brief 一回限り実行条件ノード
 */
class BTCondition_OnceFlag : public BTNode {
public:
	BTCondition_OnceFlag(const std::shared_ptr<EnemyBase>& owner, std::string flagName);

	/**
	 * @brief 条件判定の実行
	 */
	BTStatus Execute() override;


private:
	std::string m_flagName;

	// 💡 実体は .cpp 側で定義
	bool m_hasExecuted = false;
};