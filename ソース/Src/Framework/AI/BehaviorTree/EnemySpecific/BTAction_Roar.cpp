#include "BTAction_Roar.h"
#include "../../../../Application/Objects/Character/Enemy/Boss/LastBoss.h"

BTAction_Roar::BTAction_Roar(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner), m_isRoarShaked(false) {
}

BTStatus BTAction_Roar::Execute() {
	// ラスボス固有の機能（スーパーアーマー、カメラシェイク等）にアクセスするためキャスト
	auto spBoss = std::dynamic_pointer_cast<LastBoss>(m_wpOwner.lock());
	if (!spBoss) return BTStatus::Failure;

	// --- 1. アクション開始時の初期化 ---
	if (spBoss->GetState() != EnemyAnimState::Roar) {
		StartRoar(spBoss);
	}

	// アニメーションの正規化時間（0.0～1.0）を取得
	float normTime = spBoss->GetAnimator()->GetNormalizedTime();

	// --- 2. シェイク演出（タイムライン制御） ---
	UpdateRoarShake(spBoss, normTime);

	// --- 3. 終了判定 ---
	if (spBoss->GetAnimator()->IsAnimationEnd()) {
		FinishRoar(spBoss);
		return BTStatus::Success;
	}

	return BTStatus::Running;
}

void BTAction_Roar::StartRoar(std::shared_ptr<LastBoss> spBoss) {
	spBoss->SetState(EnemyAnimState::Roar);

	auto modelData = spBoss->GetModel()->GetData();
	if (modelData) {
		auto anim = modelData->GetAnimation(EnemyParam::BossSpecial::ANIM_ROAR);
		if (anim) spBoss->GetAnimator()->SetAnimation(anim, false);
	}

	// 咆哮中に攻撃を受けても怯まないように設定
	spBoss->SetSuperArmor(true);
	m_isRoarShaked = false;
}

void BTAction_Roar::UpdateRoarShake(std::shared_ptr<LastBoss> spBoss, float normTime) {
	// ① 叫びの瞬間の「ガツン」という大きな衝撃 (開始閾値：35%地点)
	if (!m_isRoarShaked && normTime >= EnemyParam::BossSpecial::Roar::SHAKE_START) {
		spBoss->ExecuteCameraShake(EnemyParam::BossSpecial::Roar::SHAKE_POWER_MAX, 20); // 定数を使用
		m_isRoarShaked = true;
	}

	// ② 咆哮が空気を震わせている継続的な「ビリビリ」感 (35% ～ 70%の間)
	if (normTime >= EnemyParam::BossSpecial::Roar::SHAKE_START && normTime <= EnemyParam::BossSpecial::Roar::SHAKE_END) {
		// 小刻みな揺れを維持
		spBoss->ExecuteCameraShake(EnemyParam::BossSpecial::Roar::SHAKE_POWER_MIN, 2);
	}
}

void BTAction_Roar::FinishRoar(std::shared_ptr<LastBoss> spBoss) {
	spBoss->SetSuperArmor(false);
	spBoss->SetState(EnemyAnimState::Idle);

	auto modelData = spBoss->GetModel()->GetData();
	if (modelData) {
		auto anim = modelData->GetAnimation(EnemyParam::Anim::IDLE);
		if (anim) spBoss->GetAnimator()->SetAnimation(anim, true);
	}
}