#include "PlayerState_RunEnd.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"
#include "../PlayerState_Run/PlayerState_Run.h"
#include "../PlayerState_Dodge/PlayerState_Dodge.h"
// 定数ファイル
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

void PlayerState_RunEnd::Enter(Player& player) {
	if (player.GetAnimator() && player.GetModel()->GetData()) {
		player.GetAnimator()->SetAnimation(player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::RUN_END), false);
	}
}

void PlayerState_RunEnd::Update(Player& player, float deltaTime) {
	// A. キャンセル：再度移動入力があれば即座に Run に戻る
	Math::Vector2 axis = KdInputManager::Instance().GetAxisState(APP_AXIS_MOVE);
	if (axis.LengthSquared() > MOVE_DIRECTION_SQUARED_THRESHOLD) {
		player.ChangeState(std::make_shared<PlayerState_Run>());
		return;
	}

	// B. キャンセル：回避ボタンで硬直をキャンセル
	if (KdInputManager::Instance().IsPress(APP_BTN_DODGE)) {
		player.ChangeState(std::make_shared<PlayerState_Dodge>());
		return;
	}

	// C. アニメーション終了で待機状態へ
	if (player.GetAnimator()->IsAnimationEnd()) {
		player.ChangeState(std::make_shared<PlayerState_Idle>());
	}
}

void PlayerState_RunEnd::Exit(Player& player) {}