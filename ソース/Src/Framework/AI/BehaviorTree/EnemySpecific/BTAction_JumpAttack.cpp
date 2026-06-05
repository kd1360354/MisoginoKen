#include "BTAction_JumpAttack.h"
#include "../../../../Application/Objects/Character/Enemy/Boss/LastBoss.h"

BTAction_JumpAttack::BTAction_JumpAttack(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner), m_hasWarped(false), m_hasEffect(false)
{
}

BTStatus BTAction_JumpAttack::Execute() {
	// ラスボス固有の機能（スーパーアーマー等）にアクセスするためキャスト
	auto spBoss = std::dynamic_pointer_cast<LastBoss>(m_wpOwner.lock());
	if (!spBoss) return BTStatus::Failure;

	// --- 1. アクション開始処理（初フレーム） ---
	if (spBoss->GetState() != EnemyAnimState::JumpAttack) {
		StartJumpAttack(spBoss);
	}

	// アニメーション情報の取得
	float currentTime = spBoss->GetAnimator()->GetTime();
	float normTime = spBoss->GetAnimator()->GetNormalizedTime();

	// --- 2. 空中ワープ演出 ---
	// 理由：プレイヤーの頭上へ瞬間移動し、回避不能感を演出するため。
	if (!m_hasWarped && currentTime >= LastBossParam::Attack::JUMP_WARP_TIME) {
		ExecuteAirWarp(spBoss);
	}

	// --- 3. 着地衝撃（エフェクト・振動・ダメージ判定） ---
	// 理由：着地の瞬間に視覚・聴覚・ゲーム性のフィードバックを集中させ、攻撃の重みを表現するため。
	if (!m_hasEffect && normTime >= EnemyParam::BossSpecial::JumpAttack::HIT_THRESHOLD) {
		ExecuteLandImpact(spBoss);
	}

	// --- 4. 終了判定 ---
	if (spBoss->GetAnimator()->IsAnimationEnd()) {
		FinishJumpAttack(spBoss);
		return BTStatus::Success;
	}

	return BTStatus::Running;
}

void BTAction_JumpAttack::StartJumpAttack(std::shared_ptr<LastBoss> spBoss) {
	spBoss->SetState(EnemyAnimState::JumpAttack);

	auto modelData = spBoss->GetModel()->GetData();
	if (modelData) {
		auto anim = modelData->GetAnimation(EnemyParam::BossSpecial::ANIM_JUMP_ATTACK);
		if (anim) spBoss->GetAnimator()->SetAnimation(anim, false);
	}

	// 攻撃の中断を防ぐためスーパーアーマーを付与
	spBoss->SetSuperArmor(true);

	m_hasWarped = false;
	m_hasEffect = false;
	spBoss->SetHasHitPlayerInCurrentAttack(false);
}

void BTAction_JumpAttack::ExecuteAirWarp(std::shared_ptr<LastBoss> spBoss) {
	auto spTarget = spBoss->GetTarget().lock();
	if (spTarget) {
		Math::Vector3 targetPos = spTarget->GetPos();
		targetPos.y += LastBossParam::Attack::JUMP_WARP_HEIGHT;

		spBoss->SetPos(targetPos);
		m_hasWarped = true;
	}
}

void BTAction_JumpAttack::ExecuteLandImpact(std::shared_ptr<LastBoss> spBoss) {
	// 演出：カメラ振動
	spBoss->ExecuteCameraShake(EnemyParam::BossSpecial::JumpAttack::SHAKE_POWER, EnemyParam::BossSpecial::JumpAttack::SHAKE_DURATION);

	// 演出：エフェクト
	KdEffekseerManager::GetInstance().Play(EnemyParam::BossSpecial::EFK_JUMP_WAVE, spBoss->GetPos(), 1.5f);

	// ゲーム性：威力倍率の設定
	spBoss->SetCurrentAttackPower(EnemyParam::BossSpecial::JumpAttack::DAMAGE_MULT);

	// 判定：広範囲の球体攻撃
	spBoss->SphierAttack(EnemyNode::Hips, Math::Vector3::Zero, EnemyParam::BossSpecial::JumpAttack::RADIUS);

	m_hasEffect = true;
}

void BTAction_JumpAttack::FinishJumpAttack(std::shared_ptr<LastBoss> spBoss) {
	spBoss->SetSuperArmor(false);
	spBoss->SetState(EnemyAnimState::Idle);

	auto modelData = spBoss->GetModel()->GetData();
	if (modelData) {
		auto anim = modelData->GetAnimation(EnemyParam::Anim::IDLE	);
		if (anim) spBoss->GetAnimator()->SetAnimation(anim, true);
	}
}