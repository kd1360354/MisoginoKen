#include "NoobEnemy.h"
#include "../EnemyBase.h"
#include "../../../../Constants/CharactorConstants/Enemy/NoobEnemy/NoobEnemyConstants.h"

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




/**
 * @brief 敵の初期化処理
 * 理由：モデルの読み込み、パラメータ設定、コライダーの登録、および行動ロジック（BT）の構築を一括で行うため。
 */
void NoobEnemy::Init()
{
	// 1. 基本設定とパラメータロード
	SetName(NoobEnemyParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j);
	}

	// 2. モデルセットアップ
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		std::string modelPath = m_ModelPath.empty() ? NoobEnemyParam::MODEL_PATH : m_ModelPath;
		m_spModel->SetModelData(modelPath);
	}

	EnemyBase::Init();

	// 3. 初期ステータス設定
	if (m_spModel->GetData())
	{
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation(EnemyParam::Anim::IDLE));
	}
	m_Speed = NoobEnemyParam::Move::SPEED_INIT;
	m_MaxHp = NoobEnemyParam::MAX_HP;
	SetHp(m_MaxHp);
	m_Scale = NoobEnemyParam::INITIAL_SCALE;
	m_searchRadius = NoobEnemyParam::SEARCH_RADIUS;
	m_attackRadius = NoobEnemyParam::ATTACK_RADIUS;
	m_drawType = eDrawTypeLit;

	// 4. 当たり判定設定
	m_pCollider = std::make_unique<KdCollider>();
	// 文字列も定数化されている場合はそれに置き換え（例：NOOB_ENEMY_NAME など）
	m_pCollider->RegisterCollisionShape("NoobEnemyDamage", m_spModel, KdCollider::Type::TypeDamage);
	m_pCollider->RegisterCollisionShape("NoobEnemyPushBack", m_spModel, KdCollider::Type::TypeEnemy);

	SetOwner(shared_from_this());
	CharacterBase::Init();

	// 5. AI構築
	BuildBehaviorTree();
}

/**
 * @brief ビヘイビアツリーの構築
 * 理由：死亡判定や被弾などの緊急事態を最優先しつつ、索敵距離に応じた「攻撃・追跡・待機」の優先順位を定義するため。
 */
void NoobEnemy::BuildBehaviorTree()
{
	std::shared_ptr<EnemyBase> spSelf = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
	if (!spSelf) return;

	auto selector = std::make_shared<BTSelector>(spSelf);

	// 死亡・被弾（優先）
	auto deathSeq = std::make_shared<BTSequence>(spSelf);
	deathSeq->AddChild(std::make_shared<BTCondition_HPCheck>(spSelf, 0.0f));
	deathSeq->AddChild(std::make_shared<BTAction_Death>(spSelf));
	deathSeq->AddChild(std::make_shared<BTAction_Dissolve>(spSelf, NoobEnemyParam::Visual::DISSOLVE_SPEED));

	auto hitSeq = std::make_shared<BTSequence>(spSelf);
	hitSeq->AddChild(std::make_shared<BTCondition_IsHit>(spSelf));
	hitSeq->AddChild(std::make_shared<BTAction_Hit>(spSelf));

	// 攻撃（攻撃範囲内なら最優先）
	auto attackSequence = std::make_shared<BTSequence>(spSelf);
	attackSequence->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_attackRadius));
	attackSequence->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack01, "Attack01", NoobEnemyParam::Attack::RATIO_01));

	// 追跡（索敵範囲内なら移動）
	auto chaseSequence = std::make_shared<BTSequence>(spSelf);
	chaseSequence->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_searchRadius));
	chaseSequence->AddChild(std::make_shared<BTAction_Chase>(spSelf));

	// 待機（プレイヤーが索敵範囲外に逃げた時だけ実行される）
	auto idleAction = std::make_shared<BTAction_Idle>(spSelf);

	selector->AddChild(deathSeq);
	selector->AddChild(hitSeq);
	selector->AddChild(attackSequence);
	selector->AddChild(chaseSequence);
	selector->AddChild(idleAction);

	m_spRootNode = selector;
}

/**
 * @brief フレーム更新処理
 * 理由：AIによる意思決定、アニメーションの更新、物理計算（重力）、および攻撃判定の実行を適切な順序で制御するため。
 */
void NoobEnemy::Update()
{
	// 1. デバッグ可視化更新
	EnemyBase::SearchAttackSphere(m_searchRadius, Math::Vector3::Zero, m_attackRadius, Math::Vector3::Zero);

	// 2. BT実行（意思決定）および共通更新
	EnemyBase::Update();

	// 3. アニメーション時間の更新（BTの結果を即反映）
	CharacterBase::AnimeUpdate();

	// 4. 攻撃判定の実行（ステートに依存）
	if (m_state == EnemyAnimState::Attack01) {
		SphierAttack(EnemyNode::RightHand, Math::Vector3::Zero, NoobEnemyParam::Attack::RADIUS_01);
	}

	// 5. 物理・重力計算
	m_Gravity += m_GravityPow;
	m_pos.y -= m_Gravity;

	// 6. 行列・座標の確定
	FinalizeTransform();

	// 7. 基底クラス最終更新（行列同期など）
	CharacterBase::Update();
}

/**
 * @brief 行列の最終確定
 * 理由：移動ステート時は振り向きを伴う UpdateTransform を行い、攻撃時などは現在の姿勢を維持して描画に反映させるため。
 */
void NoobEnemy::FinalizeTransform()
{
	if (m_state == EnemyAnimState::WalkLoop ||
		m_state == EnemyAnimState::Idle ||
		m_state == EnemyAnimState::RunLoop)
	{
		EnemyBase::UpdateTransform(m_Scale);
	}
	else
	{
		Math::Matrix scaleMat = Math::Matrix::CreateScale(m_Scale);
		Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle));
		Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
		m_mWorld = scaleMat * rotMat * transMat;
	}
}

/**
 * @brief 更新後処理
 * 理由：当たり判定後の押し戻し計算やレイ判定による接地など、物理的な位置補正を最終確定させるため。
 */
void NoobEnemy::PostUpdate()
{
	KdGameObject::PostUpdate();
	RayPostUpdate();
	// 全キャラクター共通の押し戻し定数を使用
	SpherePushBack(EnemyParam::Physical::POS_LOW, EnemyParam::Physical::PUSHBACK_RADIUS, EnemyParam::Physical::POS_UPPER);
}

/**
 * @brief 描画処理
 * 理由：親クラスで設定されたシェーダーパラメータ（リムライト、ディゾルブ等）を用いてモデルを描画するため。
 */
void NoobEnemy::DrawLit()
{
	EnemyBase::DrawLit();
}

/**
 * @brief デバッグ情報の表示
 * 理由：実行中にHPや移動速度、個体別のデバッグワイヤーの表示状態を動的に確認・変更するため。
 */
void NoobEnemy::OnImGui()
{
	ImGui::SeparatorText(GetTypeName());
	KdGameObject::OnImGui();

	// 調整用の感度や最小値も定数化できるが、ImGui内はリテラルでも許容されることが多い
	ImGui::DragFloat("MoveSpeed", &m_Speed, 0.01f, 0.0f, 100.0f);

	ImGui::Text("HP: %f", m_Hp);
	ImGui::Text("AttackPower: %f", m_enemy_attack_power);
	
	// ProgressBarの最小・最大比率計算
	float hpRate = (m_MaxHp > 0.0f) ? (m_Hp / m_MaxHp) : 0.0f;
	ImGui::ProgressBar(hpRate, ImVec2(-1, 0), std::to_string((int)m_Hp).c_str());

	static bool localShow = false;
	localShow = (m_pDebugWire != nullptr);
	if (ImGui::Checkbox(U8("この個体のデバッグ表示"), &localShow))
	{
		this->SetDebugDisplay(localShow);
	}
}