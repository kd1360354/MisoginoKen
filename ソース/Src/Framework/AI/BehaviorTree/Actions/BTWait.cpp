#include "BTWait.h"
#include "../../../../Application/Objects/Character/Enemy/EnemyBase.h"
// プロジェクトのタイマー管理クラスをインクルード


BTAction_Wait::BTAction_Wait(const std::shared_ptr<EnemyBase>& owner, float waitTime)
	: BTNode(owner), m_waitTime(waitTime), m_timer(0.0f)
{
}

BTStatus BTAction_Wait::Execute() {
	auto spOwner = m_wpOwner.lock();
	if (!spOwner) return BTStatus::Failure;

	// --- 1. 開始時の初期化 ---
	// 理由：タイマーが0（初回）の時だけステートとアニメーションを設定し、無駄な更新負荷を抑える。
	if (m_timer <= 0.0f) {
		PrepareWait(spOwner);
	}

	// --- 2. 経過時間の更新 ---
	// 環境のデルタタイム（1フレームの経過時間）を加算
	m_timer += DeltaTimer::Instance().GetDeltaTime();

	// --- 3. 終了判定 ---
	if (m_timer >= m_waitTime) {
		// 💡 リセット処理
		// 理由：BTSequenceなどでこのノードが再度呼ばれた際、0秒から計測を再開できるようにするため。
		m_timer = 0.0f;
		return BTStatus::Success;
	}

	// 待機継続中
	return BTStatus::Running;
}

void BTAction_Wait::PrepareWait(std::shared_ptr<EnemyBase> spOwner) {
	spOwner->SetState(EnemyAnimState::Idle);

	auto modelData = spOwner->GetModel()->GetData();
	if (modelData) {
		// 定数 ENEMY_ANIM_IDLE を使用
		auto anim = modelData->GetAnimation(EnemyParam::Anim::IDLE);
		if (anim) {
			spOwner->GetAnimator()->SetAnimation(anim, true);
		}
	}
}