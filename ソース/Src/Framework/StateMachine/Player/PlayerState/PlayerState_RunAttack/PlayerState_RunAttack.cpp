#include "PlayerState_RunAttack.h"
#include "../../../../../Application/Objects/Character/Player/Player.h"
#include "../PlayerState_Attack/PlayerState_Attack.h" 
#include "../PlayerState_Run/PlayerState_Run.h"
#include "../PlayerState_Idle/PlayerState_Idle.h"
// EnemyBaseの実体
#include "../../../../../Application/Objects/Character/Enemy/EnemyBase.h"
// 定数定義ファイル
#include "../../../../../Application/Constants/CharactorConstants/Player/PlayerConstants.h"

void PlayerState_RunAttack::Enter(Player& player)
{
	// 1. 初動ターゲット
	player.AutoTargetClosestEnemy();

	// 2. アニメーション設定：PlayerParam::Anim::RUN_ATK
	if (player.GetAnimator() && player.GetModel() && player.GetModel()->GetData()) {
		player.GetAnimator()->SetAnimation(
			player.GetModel()->GetData()->GetAnimation(PlayerParam::Anim::RUN_ATK), false);
	}

	// 3. サウンド再生 (Visual::Sound配下)
	player.PlaySound(PlayerParam::Visual::Sound::KATANA_SWING);

	// 4. 武器判定有効化
	if (auto katana = player.GetKatana()) {
		katana->ResetHit();
		katana->SetHitActive(true);
	}

	m_isComboReserved = false;
	m_isEffectGenerated = false;
}

void PlayerState_RunAttack::Update(Player& player, float deltaTime)
{
	auto animator = player.GetAnimator();
	if (!animator) return;
	float normTime = animator->GetNormalizedTime();

	// 攻撃中は移動を禁止
	player.SetMoveSpeed(0.0f);

	// --- 1. 物理的な踏み込み（慣性移動）：PlayerParam::RunAttack 名前空間 ---
	if (normTime >= PlayerParam::RunAttack::STEP_START && normTime <= PlayerParam::RunAttack::STEP_END) {
		Math::Vector3 forward = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player.GetAngle())).Backward();
		player.AddVelocity(forward * PlayerParam::RunAttack::STEP_POWER);
	}

	// --- 2. エフェクト発生ロジック ---
	if (!m_isEffectGenerated && normTime >= PlayerParam::Attack::EFFECT_DELAY)
	{
		auto spEnemy = player.FindNearestEnemy();

		// エフェクト名は左斬り(ATK_L1)を流用
		std::string effectName = PlayerParam::Visual::Effect::ATK_L1;

		// ヒット判定：PlayerParam::Attack::HIT_CHECK_RANGE
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

	// --- 3. 毎フレーム追従（空振り時のみ） ---
	if (auto spEffect = m_wpEffect.lock())
	{
		Math::Matrix playerRot = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player.GetAngle()));

		// 前方オフセット距離を適用
		Math::Matrix effectWorld =
			Math::Matrix::CreateTranslation(0.0f, 0.0f, PlayerParam::Attack::FORWARD_OFFS) * playerRot * Math::Matrix::CreateTranslation(player.GetPos());

		KdEffekseerManager::GetInstance().SetWorldMatrix(spEffect->GetHandle(), effectWorld);
	}

	// --- 4. 共通キャンセル（回避・ジャンプ） ---
	player.ExecuteCommonCancel(normTime);

	// --- 5. コンボ先行入力・遷移：Attack::Cancel 名前空間 ---
	if (normTime >= PlayerParam::Attack::Cancel::COMBO_START) {
		if (KdInputManager::Instance().IsPress(APP_BTN_ATTACK)) {
			m_isComboReserved = true;
		}
	}

	if (m_isComboReserved && normTime >= PlayerParam::Attack::Cancel::COMBO_START && normTime <= PlayerParam::Attack::Cancel::COMBO_END) {
		player.ChangeState(std::make_shared<PlayerState_Attack>(2)); // 通常2段目へ
		return;
	}

	// --- 6. 終了判定 ---
	if (animator->IsAnimationEnd()) {
		player.ChangeState(std::make_shared<PlayerState_Idle>());
	}
}

void PlayerState_RunAttack::Exit(Player& player)
{
	if (auto katana = player.GetKatana()) {
		katana->SetHitActive(false);
	}
}