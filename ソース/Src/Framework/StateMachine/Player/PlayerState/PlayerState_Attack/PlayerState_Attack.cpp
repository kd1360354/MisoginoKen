#include "PlayerState_Attack.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
// 新しい定数ファイルのインクルード
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"
#include"../../../../../Application/Constants/WeaponConstants/PlayerWeaponConstants/PlayerWeaponConstants.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"
#include "../PlayerState_Run/PlayerState_Run.h"
#include "../PlayerState_Dodge/PlayerState_Dodge.h"
#include "../../../../../Application/Objects/Character/Enemy/EnemyBase.h"


// =========================================================
// I. ライフサイクル
// =========================================================

void PlayerState_Attack::Enter(Player& player)
{
	// 1. 初動ターゲット：コンボの1段目のみ自動ターゲット
	if (m_comboCount == 1) {
		player.AutoTargetClosestEnemy();
	}

	// 2. スタミナ消費
	player.ConsumeStamina(player.GetAttackStaminaCost());
	// 攻撃ステートに入った瞬間に、剣に対して現在のコンボ数を教えてあげる
	if (auto katana = player.GetKatana()) {
		katana->SetAttackIdentifier(m_comboCount);
	}

	// 3. アニメーション設定：Anim名前空間から取得
	std::string animName;
	switch (m_comboCount) {
	case 1:  animName = PlayerParam::Anim::ATK_01; break;
	case 2:  animName = PlayerParam::Anim::ATK_02; break;
	case 3:  animName = PlayerParam::Anim::ATK_03; break;
	case 4:  animName = PlayerParam::Anim::ATK_04; break;
	default: animName = PlayerParam::Anim::ATK_01; break;
	}

	if (player.GetAnimator() && player.GetModel()) {
		player.GetAnimator()->SetAnimation(player.GetModel()->GetData()->GetAnimation(animName), false);
	}

	// 4. サウンド再生 (Visual::Sound配下)
	player.PlaySound(PlayerParam::Visual::Sound::KATANA_SWING);

	// 5. 武器判定の初期化
	if (auto katana = player.GetKatana()) {
		katana->ResetHit();
		katana->SetHitActive(true);
	}

	m_isComboReserved = false;
	m_isEffectGenerated = false;
}

void PlayerState_Attack::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;

	float normTime = animator->GetNormalizedTime();
	player.SetMoveSpeed(0.0f);

	// --- 1. 物理的な踏み込み処理 (Attack::Step配下) ---
	Math::Vector3 forward = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player.GetAngle())).Backward();
	if (normTime >= PlayerParam::Attack::Step::START_TIME && normTime <= PlayerParam::Attack::Step::END_TIME) {
		player.AddVelocity(forward * PlayerParam::Attack::Step::POWER);
	}

	// --- 2. エフェクト発生ロジック (AttackおよびVisual::Effect配下) ---
	if (!m_isEffectGenerated && normTime >= PlayerParam::Attack::EFFECT_DELAY) {
		std::string effectName = GetComboEffectName();
		auto spEnemy = player.FindNearestEnemy();

		// 射程圏内判定
		if (spEnemy && Math::Vector3::Distance(player.GetPos(), spEnemy->GetPos()) <= PlayerParam::Attack::HIT_CHECK_RANGE)
		{
			Math::Vector3 hitPos = spEnemy->GetPos();
			hitPos.y += PlayerParam::Attack::HIT_HEIGHT_OFFS;
			player.PlayEffect(effectName, hitPos);
		}
		else
		{
			m_wpEffect = KdEffekseerManager::GetInstance().Play(effectName, player.GetPos());
		}
		m_isEffectGenerated = true;
	}

	// --- 3. エフェクトの追従更新 ---
	if (auto spEffect = m_wpEffect.lock()) {
		Math::Matrix playerRot = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player.GetAngle()));

		// Attack::FORWARD_OFFS を適用
		Math::Matrix effectWorld =
			Math::Matrix::CreateTranslation(0.0f, 0.0f, PlayerParam::Attack::FORWARD_OFFS) * playerRot * Math::Matrix::CreateTranslation(player.GetPos());

		KdEffekseerManager::GetInstance().SetWorldMatrix(spEffect->GetHandle(), effectWorld);
	}

	// --- 4. キャンセル処理・ステート遷移 ---
	player.ExecuteCommonCancel(normTime);

	// コンボ先行入力 (Attack::Cancel配下)
	if (normTime >= PlayerParam::Attack::Cancel::RESERVE_START) {
		if (KdInputManager::Instance().IsPress(APP_BTN_ATTACK)) {
			m_isComboReserved = true;
		}
	}

	// 次のコンボ段数への遷移
	if (m_isComboReserved && normTime >= PlayerParam::Attack::Cancel::COMBO_START && normTime <= PlayerParam::Attack::Cancel::COMBO_END) {
		if (m_comboCount < PlayerParam::Attack::MAX_NORMAL_COMBO) {
			player.ChangeState(std::make_shared<PlayerState_Attack>(m_comboCount + 1));
			return;
		}
	}

	// アニメーション終了時の遷移
	if (animator->IsAnimationEnd()) {
		if (KdInputManager::Instance().IsHold(APP_BTN_ATTACK)) {
			player.ChangeState(std::make_shared<PlayerState_Attack>(1));
			return;
		}

		Math::Vector2 moveAxis = KdInputManager::Instance().GetAxisState(APP_AXIS_MOVE);
		// MOVE_DIRECTION_SQUARED_THRESHOLD はシステム定数のためそのまま(または適切な場所へ)
		if (moveAxis.LengthSquared() > MOVE_DIRECTION_SQUARED_THRESHOLD) {
			player.ChangeState(std::make_shared<PlayerState_Run>());
			return;
		}
		player.ChangeState(std::make_shared<PlayerState_Idle>());
	}
}

void PlayerState_Attack::Exit(Player& player)
{
	if (auto katana = player.GetKatana()) {
		katana->SetHitActive(false);
	}
}

// =========================================================
// II. 内部補助関数
// =========================================================

std::string PlayerState_Attack::GetComboEffectName()
{
	// Visual::Effect配下から取得
	switch (m_comboCount) {
	case 1:  return PlayerParam::Visual::Effect::ATK_L1;
	case 2:  return PlayerParam::Visual::Effect::ATK_R1;
	case 3:  return PlayerParam::Visual::Effect::ATK_R1;
	case 4:  return PlayerParam::Visual::Effect::ATK_L1;
	default: return PlayerParam::Visual::Effect::ATK_L1;
	}
}