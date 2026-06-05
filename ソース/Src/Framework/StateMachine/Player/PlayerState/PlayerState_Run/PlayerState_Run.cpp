#include "PlayerState_Run.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
// 新しい定数ファイルをインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

// 必要な遷移先ステート
#include "../PlayerState_Idle/PlayerState_Idle.h"
#include "../PlayerState_RunEnd/PlayerState_RunEnd.h"
#include "../PlayerState_Dodge/PlayerState_Dodge.h"
#include "../PlayerState_RunAttack/PlayerState_RunAttack.h"

// 名前空間のエイリアス
namespace PP = PlayerParam;

void PlayerState_Run::Enter(Player& player)
{
	m_phase = RunPhase::Start;

	// 走り出しアニメーション
	if (player.GetModel() && player.GetModel()->GetData())
	{
		// PLAYER_ANIM_RUN_START -> PP::Anim::RUN_START
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PP::Anim::RUN_START), false);
	}
}

void PlayerState_Run::Update(Player& player, float deltaTime)
{
	Math::Matrix cameraRotYMat = player.GetCameraWorldMatrix();
	Math::Vector3 inputDir;
	bool moveFlg = false, runFlg = false;

	// 入力情報の取得
	player.UpdateInPut(cameraRotYMat, inputDir, moveFlg, runFlg);

	// --- 1. 割り込み処理 ---
	if (KdInputManager::Instance().IsPress(APP_BTN_DODGE)) {
		player.ChangeState(std::make_shared<PlayerState_Dodge>());
		return;
	}
	if (KdInputManager::Instance().IsPress(APP_BTN_ATTACK)) {
		player.AutoTargetClosestEnemy();
		player.ChangeState(std::make_shared<PlayerState_RunAttack>());
		return;
	}

	// --- 2. 停止判定 ---
	if (!moveFlg) {
		player.ChangeState(std::make_shared<PlayerState_RunEnd>());
		return;
	}

	// --- 3. アニメーションの動的決定 ---
	auto animator = player.GetAnimator();

	// ANIM_RUN_START_TO_LOOP_TIME -> PP::Anim::Transition::RUN_S_TO_L
	if (m_phase == RunPhase::Start && animator->GetTime() >= (PP::Anim::Transition::RUN_S_TO_L / 60.0f)) {
		m_phase = RunPhase::Loop;
	}

	if (m_phase == RunPhase::Loop) {
		// デフォルト：前走り (PLAYER_ANIM_RUN_LOOP_F -> PP::Anim::RUN_LOOP_F)
		std::string animName = PP::Anim::RUN_LOOP_F;

		if (player.IsLockOn()) {
			// ロックオン中の 8 方向移動アニメーション判定
			bool W = player.IsMovingForward();
			bool S = player.IsMovingBackward();
			bool A = player.IsMovingLeft();
			bool D = player.IsMovingRight();

			// 8方向アニメーション名のリネーム対応
			if (S) {
				if (A)      animName = PP::Anim::RUN_LOOP_BL;
				else if (D) animName = PP::Anim::RUN_LOOP_BR;
				else        animName = PP::Anim::RUN_LOOP_B;
			}
			else if (W) {
				if (A)      animName = PP::Anim::RUN_LOOP_FL;
				else if (D) animName = PP::Anim::RUN_LOOP_FR;
				else        animName = PP::Anim::RUN_LOOP_F;
			}
			else if (A)     animName = PP::Anim::RUN_LOOP_L;
			else if (D)     animName = PP::Anim::RUN_LOOP_R;
		}

		// アニメーションが切り替わった時のみ再設定
		if (animator->GetCurrentAnimationName() != animName) {
			animator->SetAnimation(player.GetModel()->GetData()->GetAnimation(animName), true);
		}
	}

	// --- 4. 移動・回転の実行 ---
	player.UpdateMovementAndPhysics(inputDir, moveFlg, runFlg);
}

void PlayerState_Run::Exit(Player& player) {}