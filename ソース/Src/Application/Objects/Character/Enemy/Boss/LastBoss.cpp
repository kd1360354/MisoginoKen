#include "LastBoss.h"

#include "../../../Character/Player/Player.h"
#include "../../../Ui/EnemyUi/EnemyUi.h"

#include"../../../../Constants/CharactorConstants/Enemy/Boss/LastBossConstants.h"
#include "../../../Camera/TPSCamera/TPSCamera.h"
#include"../../../../../Framework/AI/BehaviorTree/Core/BTComposite.h"
#include"../../../../../Framework/AI/BehaviorTree/Core/BTRandomSelector.h"
#include"../../../../../Framework/AI/BehaviorTree/Actions/BTAction_Chase.h"
#include"../../../../../Framework/AI/BehaviorTree/Actions/BTAction_Death.h"
#include"../../../../../Framework/AI/BehaviorTree/Actions/BTAction_Dissolve.h"
#include"../../../../../Framework/AI/BehaviorTree/Actions/BTAction_Hit.h"
#include"../../../../../Framework/AI/BehaviorTree/Actions/BTAction_Idle.h"
#include"../../../../../Framework/AI/BehaviorTree/Conditions/BTCondition_HPCheck.h"
#include"../../../../../Framework/AI/BehaviorTree/Conditions/BTCondition_IsHit.h"
#include"../../../../../Framework/AI/BehaviorTree/Conditions/BTCondition_RangeCheck.h"
#include"../../../../../Framework/AI/BehaviorTree/EnemySpecific/BTAction_Attack.h"
#include"../../../../../Framework/AI/BehaviorTree/Actions/BTWait.h"
#include"../../../../../Framework/AI/BehaviorTree/Conditions/BTCondition_OnceFlag.h"
#include"../../../../../Framework/AI/BehaviorTree/EnemySpecific/BTAction_Roar.h"
#include"../../../../../Framework/AI/BehaviorTree/EnemySpecific/BTAction_JumpAttack.h"



/**
 * @brief 初期化処理
 * 理由：専用モデルのロード、ステータスの設定、およびボス専用の特殊な行動ツリーを構築するため。
 */
void LastBoss::Init()
{
	SetName(LastBossParam::NAME);

	// 1. パラメータロード
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j);
	}

	if (m_pos == Math::Vector3::Zero) {
		SetPos(LastBossParam::SPAWN_POS);
	}

	// 2. モデルロード
	if (!m_spModel) {
		m_spModel = std::make_shared<KdModelWork>();
		std::string modelPath = m_ModelPath.empty() ? LastBossParam::MODEL_PATH : m_ModelPath;
		m_spModel->SetModelData(modelPath);
	}

	EnemyBase::Init();

	if (m_spModel->GetData()) {
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation(EnemyParam::Anim::IDLE));
	}

	// 3. ステータス同期（JSON未定義時のフォールバック）
	if (m_MaxHp <= LastBossParam::Dissolve::THRESHOLD) m_MaxHp = LastBossParam::MAX_HP;
	m_Hp = m_MaxHp;
	m_Scale = LastBossParam::INITIAL_SCALE;
	if (m_searchRadius <= LastBossParam::Dissolve::THRESHOLD) m_searchRadius = LastBossParam::SEARCH_RADIUS;
	if (m_attackRadius <= LastBossParam::Dissolve::THRESHOLD) m_attackRadius = LastBossParam::ATTACK_RADIUS;
	m_Speed = EnemyParam::Move::SPEED_DEFAULT;

	// 4. コライダー設定
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("LastBoss", m_spModel, KdCollider::Type::TypeDamage);
	m_pCollider->RegisterCollisionShape("LastBossPushBack", m_spModel, KdCollider::Type::TypeEnemy);

	SetOwner(shared_from_this());
	SetRimLightParams(EnemyParam::Visual::RIM_COLOR, EnemyParam::Visual::RIM_POWER_DEF);

	CharacterBase::Init();

	BuildBehaviorTree();
}

/**
 * @brief ビヘイビアツリーの構築
 * 理由：開幕の咆哮から始まり、HPに応じたフェーズ移行や、ジャンプ攻撃を含む多彩なランダム攻撃を制御するため。
 */
void LastBoss::BuildBehaviorTree()
{
	std::shared_ptr<EnemyBase> spSelf = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
	auto root = std::make_shared<BTSelector>(spSelf);

	// --- 優先順位 1: 死亡 ---
	auto death = std::make_shared<BTSequence>(spSelf);
	death->AddChild(std::make_shared<BTCondition_HPCheck>(spSelf, LastBossParam::Dissolve::THRESHOLD));
	death->AddChild(std::make_shared<BTAction_Death>(spSelf));
	death->AddChild(std::make_shared<BTAction_Dissolve>(spSelf, LastBossParam::Dissolve::SPEED));

	// --- 優先順位 2: 特殊演出（咆哮） ---
	auto specialAction = std::make_shared<BTSelector>(spSelf);

	// 開幕
	auto opening = std::make_shared<BTSequence>(spSelf);
	opening->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_searchRadius));
	opening->AddChild(std::make_shared<BTCondition_OnceFlag>(spSelf, "OpeningRoar"));
	opening->AddChild(std::make_shared<BTAction_Roar>(spSelf));

	// フェーズ移行
	auto phaseChange = std::make_shared<BTSequence>(spSelf);
	phaseChange->AddChild(std::make_shared<BTCondition_HPCheck>(spSelf, GetHpRatio()));
	phaseChange->AddChild(std::make_shared<BTCondition_OnceFlag>(spSelf, "PhaseChangeRoar"));
	phaseChange->AddChild(std::make_shared<BTAction_Roar>(spSelf));

	specialAction->AddChild(opening);
	specialAction->AddChild(phaseChange);

	// --- 優先順位 3: 攻撃シーケンス ---
	auto attackRoot = std::make_shared<BTSequence>(spSelf);
	attackRoot->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_attackRadius));

	auto atkRand = std::make_shared<BTRandomSelector>(spSelf);
	atkRand->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack01, "Attack01", LastBossParam::Attack::RATIO_LIGHT));
	atkRand->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack02, "Attack02", LastBossParam::Attack::RATIO_MEDIUM));
	atkRand->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack03, "Attack03", LastBossParam::Attack::RATIO_HEAVY));
	atkRand->AddChild(std::make_shared<BTAction_JumpAttack>(spSelf));

	attackRoot->AddChild(atkRand);
	attackRoot->AddChild(std::make_shared<BTAction_Wait>(spSelf, LastBossParam::ATTACK_WAIT_TIME));

	// --- 優先順位 4: 追跡 ---
	auto chase = std::make_shared<BTSequence>(spSelf);
	chase->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_searchRadius));
	chase->AddChild(std::make_shared<BTAction_Chase>(spSelf));

	// ルートへ登録
	root->AddChild(death);
	root->AddChild(specialAction);
	root->AddChild(attackRoot);
	root->AddChild(chase);

	m_spRootNode = root;
}

/**
 * @brief メイン更新処理
 * 理由：AIの実行を軸に、攻撃中の判定生成や重力計算、および各状態に応じたアニメーション速度の微調整を行うため。
 */
void LastBoss::Update()
{
	if (m_isExpired) return;

	// 被弾・ノックバック時は硬直を優先
	if (m_state == EnemyAnimState::Hit || m_state == EnemyAnimState::KonckBack)
	{
		CharacterBase::AnimeUpdate();
		if (m_spAnimator->IsAnimationEnd()) m_state = EnemyAnimState::Idle;
		return;
	}

	// アニメーション速度の動的決定
	float animSpeed = LastBossParam::Anim::SPEED_DEFAULT;
	if (m_state == EnemyAnimState::Death) { animSpeed = LastBossParam::Anim::SPEED_DEATH; }
	else if (m_state == EnemyAnimState::WalkLoop || m_state == EnemyAnimState::RunLoop) { animSpeed = LastBossParam::Anim::SPEED_MOVE; }

	CharacterBase::AnimeUpdate(animSpeed);

	EnemyBase::Update();

	UpdateAttackCollision();

	m_Gravity += m_GravityPow;
	m_pos.y -= m_Gravity;

	FinalizeTransform();

	CharacterBase::Update();
}

/**
 * @brief 攻撃判定の生成
 * 理由：ステートに応じて、適切な攻撃部位と攻撃範囲（ジャンプ攻撃時は広範囲など）を指定して当たり判定を発生させるため。
 */
void LastBoss::UpdateAttackCollision()
{
	if (m_state == EnemyAnimState::Attack01)	  SphierAttack(EnemyNode::LeftHand);
	else if (m_state == EnemyAnimState::Attack02) SphierAttack(EnemyNode::RightHand);
	else if (m_state == EnemyAnimState::JumpAttack)
	{
		SphierAttack(EnemyNode::Hips, Math::Vector3::Zero, LastBossParam::Attack::JUMP_RADIUS);
	}
}

/**
 * @brief 行列の最終確定
 * 理由：ボスの現在座標と回転、スケールをワールド行列に統合し、描画の基準点を確定させるため。
 */
void LastBoss::FinalizeTransform()
{
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_Scale);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = scaleMat * rotMat * transMat;
}

void LastBoss::ProcessDeath()
{
	if (m_state == EnemyAnimState::Death) return;

	m_state = EnemyAnimState::Death;
	m_isDefeated = true;

	if (m_spModel && m_spModel->GetData()) {
		auto death_anim = m_spModel->GetData()->GetAnimation(EnemyParam::Anim::DEATH);
		if (death_anim) m_spAnimator->SetAnimation(death_anim, false);
	}

	if (m_deathEffectHandle == EnemyParam::EFFECT_INVALID_HANDLE) {
		auto wp_effect = KdEffekseerManager::GetInstance().Play(LastBossParam::DEATH_EFFECT_NAME, GetPos());
		if (auto sp_effect = wp_effect.lock()) {
			m_deathEffectHandle = sp_effect->GetHandle();
		}
	}
}

/**
 * @brief 更新後処理
 * 理由：アニメーションの進行を確定させ、地面との接地判定およびプレイヤーとのめり込み防止用押し戻し計算を行うため。
 */
void LastBoss::PostUpdate()
{
	CharacterBase::AnimeUpdate();
	KdGameObject::PostUpdate();
	RayPostUpdate();

	SpherePushBack(
		Math::Vector3(0.0f, EnemyParam::Physical::PUSHBACK_LOW_Y , 0.0f),
		EnemyParam::Physical::PUSHBACK_RADIUS,
		Math::Vector3(0.0f, EnemyParam::Physical::PUSHBACK_UPPER_Y, 0.0f)
	);
}

/**
 * @brief 被弾時の処理
 * 理由：HPを減算し、死亡時には専用エフェクトとリザルト移行用のフラグを設定、生存時はスーパーアーマーでない限り被弾硬直へ遷移させるため。
 */
void LastBoss::OnHit(float damage)
{
	m_Hp -= damage;
	if (m_Hp <= LastBossParam::Dissolve::THRESHOLD)
	{
		m_Hp = LastBossParam::Dissolve::THRESHOLD;
		// ★ 死亡処理はすべて ProcessDeath に任せる
		ProcessDeath();
	}
	else if (!m_isSuperArmor && m_state != EnemyAnimState::Death)
	{
		m_state = EnemyAnimState::Hit;
		auto hit_anim = m_spModel->GetData()->GetAnimation(EnemyParam::Anim::HIT);
		if (hit_anim) m_spAnimator->SetAnimation(hit_anim, false);
	}
}

/**
 * @brief 外部データ（JSON）の反映
 * 理由：親クラスの共通ロードに加え、ラスボス特有の「フェーズ移行HP比率」などを取得し、戦闘バランスの調整を容易にするため。
 */
void LastBoss::LoadParameters(const nlohmann::json& _j)
{
	CharacterBase::LoadParameters(_j);
	EnemyBase::LoadParameters(_j);
	auto GetFloat = [&](const std::string& key, float def) {
		return (_j.contains(key) && _j[key].is_number()) ? (float)_j[key].get<double>() : def;
		};

	m_HpRatio = GetFloat("hpRatio", LastBossParam::HP_TRIGGER_PHASE_2);
	m_Hp = m_MaxHp;
}

/**
 * @brief 描画処理
 * 理由：EnemyBaseの標準的な描画フローを利用し、リムライトやディゾルブが適用されたボスのモデルを描画するため。
 */
void LastBoss::DrawLit()
{
	EnemyBase::DrawLit();
}

/**
 * @brief デバッグ情報の表示
 * 理由：開発中に個体別のデバッグワイヤー（判定範囲など）を表示するかどうかをImGui経由で切り替えるため。
 */
void LastBoss::OnImGui()
{
	static bool localShow = false;
	localShow = (m_pDebugWire != nullptr);
	if (ImGui::Checkbox(U8("この個体のデバッグ表示"), &localShow))
	{
		this->SetDebugDisplay(localShow);
	}
}

/**
 * @brief カメラの揺れ演出実行
 * 理由：攻撃の着地時や咆哮時など、ボスの威圧感を表現するためにTPSカメラに対して振動命令を送るため。
 */
void LastBoss::ExecuteCameraShake(float intensity, int frame)
{
	auto spTarget = m_wpTarget.lock();
	if (auto spPlayer = std::dynamic_pointer_cast<Player>(spTarget)) {
		auto spBaseCam = spPlayer->GetCamera();
		auto spTPS = std::dynamic_pointer_cast<TPSCamera>(spBaseCam);
		if (spTPS) {
			spTPS->SetShake(intensity, frame);
		}
	}
}