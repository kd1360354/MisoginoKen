#include "BTAction_Chase.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"
// ★ Player.h と SceneManager.h は不要になるので削除
// #include "../../../../Application/Objects/Character/Player/Player.h"
// #include "../../../../Application/Scene/SceneManager.h"

BTAction_Chase::BTAction_Chase(const std::shared_ptr<EnemyBase>& owner)
	: BTNode(owner)
{
}

BTStatus BTAction_Chase::Execute()
{
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// ★ SceneManager::GetPlayer() → GetTarget() 経由に変更
	auto spTarget = spOwner->GetTarget().lock();
	if (!spTarget) return BTStatus::Failure;

	// --- 1. 距離に基づいた中断判定 ---
	// ★ spPlayer->GetPos() → spTarget->GetPos()
	float dist = (spTarget->GetPos() - spOwner->GetPos()).Length();

	if (dist > spOwner->GetSearchRadius())
	{
		StopChase(spOwner);
		return BTStatus::Failure;
	}

	if (dist <= spOwner->GetAttackRadius())
	{
		return BTStatus::Failure;
	}

	// --- 2. 移動パラメータの決定（発狂状態による分岐） ---
	bool isDual = (spOwner->GetHp() <= (spOwner->GetMaxHP() * spOwner->GetHpRatio()));
	EnemyAnimState nextState = isDual ? EnemyAnimState::RunLoop : EnemyAnimState::WalkLoop;
	std::string animName = isDual ? EnemyParam::Anim::RUN : EnemyParam::Anim::WALK;
	float currentSpeed = isDual ? EnemyParam::Move::SPEED_RUN : EnemyParam::Move::SPEED_WALK;

	spOwner->SetSpeed(currentSpeed);

	// --- 3. ステート・アニメーションの適用 ---
	if (spOwner->GetState() != nextState)
	{
		spOwner->SetState(nextState);
		auto modelData = spOwner->GetModel()->GetData();
		if (modelData)
		{
			auto anim = modelData->GetAnimation(animName);
			if (!anim && isDual) anim = modelData->GetAnimation(EnemyParam::Anim::WALK);
			if (anim) spOwner->GetAnimator()->SetAnimation(anim, true);
		}
	}

	// --- 4. 向き・行列更新 ---
	spOwner->UpdateTransform(spOwner->GetScale());

	return BTStatus::Running;
}

void BTAction_Chase::StopChase(std::shared_ptr<EnemyBase> spOwner)
{
	spOwner->SetState(EnemyAnimState::Idle);
	spOwner->SetSpeed(0.0f);
	auto modelData = spOwner->GetModel()->GetData();
	if (modelData)
	{
		auto anim = modelData->GetAnimation(EnemyParam::Anim::IDLE);
		if (anim) spOwner->GetAnimator()->SetAnimation(anim, true);
	}
}