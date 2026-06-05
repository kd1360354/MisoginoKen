#include "PlayerState_Idle.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Attack/PlayerState_Attack.h"
#include "../PlayerState_Run/PlayerState_Run.h"
#include "../PlayerState_Potion/PlayerState_Potion.h"
#include "../PlayerState_Dodge/PlayerState_Dodge.h"
#include "../PlayerState_PowerAttack/PlayerState_PowerAttack.h"
#include "../PlayerState_Block/PlayerState_Block.h"
#include "../PlayerState_Jump/PlayerState_Jump.h"
// 定数ファイルのインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

void PlayerState_Idle::Enter(Player& player)
{
	// 待機アニメーションの再生 (PlayerParam::Anim::IDLE)
	if (player.GetAnimator() && player.GetModel())
	{
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::IDLE), true);
	}
}

void PlayerState_Idle::Update(Player& player, float deltaTime)
{
	// 待機中は移動速度を0に
	player.SetMoveSpeed(0.0f);

	// --- 状態遷移の判定（優先順位順） ---

	// 1. 回避 (Dodge)
	if (KdInputManager::Instance().IsPress(APP_BTN_DODGE))
	{
		player.ChangeState(std::make_shared<PlayerState_Dodge>());
		return;
	}

	// 2. 攻撃 (Attack)
	if (KdInputManager::Instance().IsPress(APP_BTN_ATTACK))
	{
		player.ChangeState(std::make_shared<PlayerState_Attack>(1));
		return;
	}

	// 3. 必殺技 (Power Attack)：整理後の PlayerParam::PowerAttack::MAX_COST を使用
	if (KdInputManager::Instance().IsPress(APP_BTN_POWER_ATK) &&
		player.GetPowerCost() >= PlayerParam::PowerAttack::MAX_COST)
	{
		player.ChangeState(std::make_shared<PlayerState_PowerAttack>());
		return;
	}

	// 4. ガード (Block)
	if (KdInputManager::Instance().IsHold(APP_BTN_GUARD))
	{
		player.ChangeState(std::make_shared<PlayerState_Block>());
		return;
	}

	// 5. ジャンプ (Jump)
	if (KdInputManager::Instance().IsPress(APP_BTN_JUMP))
	{
		player.ChangeState(std::make_shared<PlayerState_Jump>());
		return;
	}

	// 6. ポーション (Potion)
	if (KdInputManager::Instance().IsPress(APP_BTN_POTION) && player.GetPotionCount() > 0)
	{
		player.ChangeState(std::make_shared<PlayerState_Potion>());
		return;
	}

	// 7. 移動 (Run)
	Math::Vector2 moveAxis = KdInputManager::Instance().GetAxisState(APP_AXIS_MOVE);
	// MOVE_DIRECTION_SQUARED_THRESHOLD はシステム/アプリ共通定数として維持
	if (moveAxis.LengthSquared() > MOVE_DIRECTION_SQUARED_THRESHOLD)
	{
		player.ChangeState(std::make_shared<PlayerState_Run>());
		return;
	}
}

void PlayerState_Idle::Exit(Player& player)
{
}