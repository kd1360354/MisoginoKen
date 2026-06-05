#include "MidBoss.h"
#include"../../Player/Player.h"
#include "../../../Weapon/DualWeapon/LeftWeapon/LeftWeapon.h"
#include "../../../Weapon/DualWeapon/RigftWeapon/RightWeapon.h"
#include "../../CharactorBase.h" 
#include "../../../../Constants/CharactorConstants/Enemy/MidBoss/MidBossConstants.h"

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


/**
 * @brief 初期化処理
 * 理由：パラメータの読み込み、二本の武器の生成、および複雑な分岐を持つビヘイビアツリーを構築するため。
 */
void MidBoss::Init()
{
	SetName(MidBossParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j);
	}
	
	// 初期位置設定
	if (m_pos == Math::Vector3::Zero) {
		SetPos(MidBossParam::SPAWN_POS);
	}

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		std::string modelPath = m_ModelPath.empty() ? MidBossParam::MODEL_PATH : m_ModelPath;
		m_spModel->SetModelData(modelPath);
	}

	EnemyBase::Init();

	if (m_spModel->GetData())
	{
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation(EnemyParam::Anim::IDLE));
	}

	// 武器の初期化
	m_spRightWeapon = std::make_shared<RightWeapon>();
	m_spRightWeapon->Init();
	m_spRightWeapon->SetOwner(shared_from_this());
	m_spRightWeapon->SetHand(Hand::Hip);

	m_spLeftWeapon = std::make_shared<LeftWeapon>();
	m_spLeftWeapon->Init();
	m_spLeftWeapon->SetOwner(shared_from_this());
	m_spLeftWeapon->SetHand(Hand::Hip);

	m_MaxHp = MidBossParam::MAX_HP;
	m_Hp = m_MaxHp;
	m_searchRadius = MidBossParam::SEARCH_RADIUS;
	m_attackRadius = MidBossParam::ATTACK_RADIUS;
	m_drawType = eDrawTypeLit;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(MidBossParam::Collider::DAMAGE_TAG, m_spModel, KdCollider::Type::TypeDamage);
	m_pCollider->RegisterCollisionShape(MidBossParam::Collider::PUSHBACK_TAG, m_spModel, KdCollider::Type::TypeEnemy);

	SetOwner(shared_from_this());
	CharacterBase::Init();
	SetRimLightParams(EnemyParam::Visual::RIM_COLOR, EnemyParam::Visual::RIM_POWER_DEF);

	BuildBehaviorTree();
}

/**
 * @brief ビヘイビアツリーの構築
 * 理由：HP比率によって「通常攻撃」と「二刀流による強力な攻撃」をセレクターで切り替え、ボスのフェーズ進行を表現するため。
 */
void MidBoss::BuildBehaviorTree()
{
	std::shared_ptr<EnemyBase> spSelf = std::dynamic_pointer_cast<EnemyBase>(shared_from_this());
	if (!spSelf) return;

	auto rootSelector = std::make_shared<BTSelector>(spSelf);

	// 1. 死亡シーケンス
	auto deathSeq = std::make_shared<BTSequence>(spSelf);
	deathSeq->AddChild(std::make_shared<BTCondition_HPCheck>(spSelf, 0.0f));
	deathSeq->AddChild(std::make_shared<BTAction_Death>(spSelf));
	deathSeq->AddChild(std::make_shared<BTAction_Dissolve>(spSelf, 0.02f));

	// 2. 被弾シーケンス
	auto hitSeq = std::make_shared<BTSequence>(spSelf);
	hitSeq->AddChild(std::make_shared<BTCondition_IsHit>(spSelf));
	hitSeq->AddChild(std::make_shared<BTAction_Hit>(spSelf));

	// 3. 攻撃メインツリー
	auto attackFullSeq = std::make_shared<BTSequence>(spSelf);
	attackFullSeq->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_attackRadius));

	auto attackModeSelector = std::make_shared<BTSelector>(spSelf);

	// 【二刀流フェーズ】
	auto dualAttackBranch = std::make_shared<BTSequence>(spSelf);
	dualAttackBranch->AddChild(std::make_shared<BTCondition_HPCheck>(spSelf, GetHpRatio())); 
	auto dualRandom = std::make_shared<BTRandomSelector>(spSelf);
	dualRandom->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack04, "Attack04", 1.2f));
	dualRandom->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack05, "Attack05", 1.2f));
	dualRandom->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack06, "Attack06", 1.5f));
	dualAttackBranch->AddChild(dualRandom);

	// 【通常フェーズ】
	auto normalRandom = std::make_shared<BTRandomSelector>(spSelf);
	normalRandom->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack01, "Attack01", 1.0f));
	normalRandom->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack02, "Attack02", 1.0f));
	normalRandom->AddChild(std::make_shared<BTAction_Attack>(spSelf, EnemyAnimState::Attack03, "Attack03", 1.1f));

	attackModeSelector->AddChild(dualAttackBranch);
	attackModeSelector->AddChild(normalRandom);

	attackFullSeq->AddChild(attackModeSelector);
	attackFullSeq->AddChild(std::make_shared<BTAction_Wait>(spSelf, 0.5f));

	// 4. 追跡シーケンス
	auto chaseSeq = std::make_shared<BTSequence>(spSelf);
	chaseSeq->AddChild(std::make_shared<BTCondition_InRange>(spSelf, m_searchRadius));
	chaseSeq->AddChild(std::make_shared<BTAction_Chase>(spSelf));

	rootSelector->AddChild(deathSeq);
	rootSelector->AddChild(hitSeq);
	rootSelector->AddChild(attackFullSeq);
	rootSelector->AddChild(chaseSeq);

	m_spRootNode = rootSelector;
}

/**
 * @brief フレーム更新処理
 * 理由：AIの意思決定を行う前に攻撃判定を更新することで、アニメーション終了直前の判定漏れを防ぎ、二本の武器の姿勢を同期させるため。
 */
void MidBoss::Update()
{
	CharacterBase::AnimeUpdate();
	EnemyBase::SearchAttackSphere(m_searchRadius, Math::Vector3::Zero, m_attackRadius);

	// 二刀流状態の判定
	const bool isDualWield = (m_Hp <= (m_MaxHp * GetHpRatio()));

	// 1. 攻撃判定の生成
	UpdateAttackCollision(isDualWield);

	// 2. 武器のアタッチ（手に持つか、腰に置くか）の制御
	UpdateWeaponAttachment(isDualWield);

	// 3. AI・基底クラス更新
	EnemyBase::Update();

	// 4. 武器の見た目（角度・位置）をステートに合わせて補正
	UpdateWeaponLocalMatrices(isDualWield);

	// 5. 物理・移動計算
	m_Gravity += m_GravityPow;
	m_pos.y -= m_Gravity;

	EnemyBase::UpdateTransform(MidBossParam::UPDATE_SCALE);

	if (m_spRightWeapon) m_spRightWeapon->Update();
	if (m_spLeftWeapon) m_spLeftWeapon->Update();

	CharacterBase::Update();
}

/**
 * @brief 攻撃判定の生成
 * 理由：各ステートに対応した部位から判定を出し、特に二刀流攻撃時は両手の武器位置からオフセット計算を行って判定を発生させるため。
 */
void MidBoss::UpdateAttackCollision(bool isDualWield)
{
	if (m_state == EnemyAnimState::Attack01)
	{
		SphierAttack(EnemyNode::RightHand, Math::Vector3::Zero, MidBossParam::Collider::PUSHBACK_RADIUS);
	}
	else if (m_state == EnemyAnimState::Attack02)
	{
		SphierAttack(EnemyNode::LeftFoot, Math::Vector3::Zero, MidBossParam::Collider::PUSHBACK_RADIUS);
	}
	else if (m_state == EnemyAnimState::Attack03)
	{
		SphierAttack(EnemyNode::LeftHand, Math::Vector3::Zero, MidBossParam::Collider::ATTACK_SPHERE_RADIUS);
	}
	else if ((m_state >= EnemyAnimState::Attack04 && m_state <= EnemyAnimState::Attack06) && isDualWield)
	{
		SphierAttack(EnemyNode::RightHand, MidBossParam::Collider::ATTACK_L_WEAPON_POS, MidBossParam::Collider::ATTACK_SPHERE_RADIUS);
		SphierAttack(EnemyNode::LeftHand, MidBossParam::Collider::ATTACK_L_WEAPON_POS, MidBossParam::Collider::ATTACK_SPHERE_RADIUS);
	}
}

/**
 * @brief 武器の装着状態更新
 * 理由：通常時は腰に、特定攻撃や発狂モード時は両手に武器が装備されるように、武器クラスの装着部位フラグを動的に書き換えるため。
 */
void MidBoss::UpdateWeaponAttachment(bool isDualWield)
{
	Hand leftHand = Hand::Hip;
	Hand rightHand = Hand::Hip;

	if (isDualWield)
	{
		// 発狂モード時は基本的に両手
		leftHand = Hand::Left;
		rightHand = Hand::Right;
	}
	else{}

	if (m_spRightWeapon) m_spRightWeapon->SetHand(rightHand);
	if (m_spLeftWeapon)  m_spLeftWeapon->SetHand(leftHand);
}

/**
 * @brief 武器のローカル行列計算
 * 理由：マジックナンバーを排除し、定数に基づいて「構え」「ナルト走り」「斬撃」などのポーズごとに武器の行列を正しく構築するため。
 */
void MidBoss::UpdateWeaponLocalMatrices(bool isDualWield)
{
	if (!m_spLeftWeapon || !m_spRightWeapon) return;

	// デフォルト行列（腰などに格納されている時）
	Math::Matrix rotDefault = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(MidBossParam::Weapon::DEFAULT_ROT_X));
	Math::Matrix trnsDefault = Math::Matrix::CreateTranslation(MidBossParam::Weapon::DEFAULT_TRNS_X, MidBossParam::Weapon::DEFAULT_TRNS_Y, MidBossParam::Weapon::DEFAULT_TRNS_Z);
	Math::Matrix defaultMat = trnsDefault * rotDefault;

	if (isDualWield)
	{
		if (m_state == EnemyAnimState::RunLoop)
		{
			// 走りポーズ（ナルト走り）
			Math::Matrix runRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(MidBossParam::Weapon::Run::ADJ_X)) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(MidBossParam::Weapon::Run::ADJ_Y));
			Math::Matrix runTrns = Math::Matrix::CreateTranslation(MidBossParam::Weapon::Run::TRANS_X, MidBossParam::Weapon::Run::TRANS_Y, MidBossParam::Weapon::Run::OFFSET_Z);
			
			m_spLeftWeapon->SetLocalMatrix(runTrns * runRot * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(MidBossParam::Weapon::Run::ADJ_Z)));
			m_spRightWeapon->SetLocalMatrix(runTrns * runRot * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(-MidBossParam::Weapon::Run::ADJ_Z)));
		}
		else if (m_state == EnemyAnimState::Idle || m_state == EnemyAnimState::WalkLoop || m_state == EnemyAnimState::Hit ||
			     (m_state >= EnemyAnimState::Attack04 && m_state <= EnemyAnimState::Attack06))
		{
			// 戦闘構え・攻撃中
			Math::Matrix leftRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::ROT_X)) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::ROT_Y)) * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::ROT_Z_L));
			Math::Matrix rightRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::ROT_X)) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::ROT_Y)) * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::ROT_Z_R));
			Math::Matrix attackTrns = Math::Matrix::CreateTranslation(MidBossParam::Weapon::Attack::TRANS_X, MidBossParam::Weapon::Attack::TRANS_Y, MidBossParam::Weapon::Attack::TRNS_Z);

			if (m_state == EnemyAnimState::Attack06)
			{
				Math::Matrix swingRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(MidBossParam::Weapon::Attack::DOWN_SWING_ROT));
				m_spLeftWeapon->SetLocalMatrix(attackTrns * leftRot * swingRot);
				m_spRightWeapon->SetLocalMatrix(attackTrns * rightRot * swingRot);
			}
			else
			{
				m_spLeftWeapon->SetLocalMatrix(attackTrns * leftRot);
				m_spRightWeapon->SetLocalMatrix(attackTrns * rightRot);
			}
		}
	}
	else
	{
		m_spLeftWeapon->SetLocalMatrix(defaultMat);
		m_spRightWeapon->SetLocalMatrix(defaultMat);
	}
}

/**
 * @brief 更新後処理
 * 理由：地形との接地判定や、他キャラクターとの押し戻し処理を、中ボス固有のコライダーサイズ（定数）で実行するため。
 */
void MidBoss::PostUpdate()
{
	KdGameObject::PostUpdate();
	RayPostUpdate();
	SpherePushBack(MidBossParam::Collider::PUSHBACK_POS_LOW, MidBossParam::Collider::PUSHBACK_RADIUS, MidBossParam::Collider::PUSHBACK_POS_UPPER);
}

/**
 * @brief 描画処理
 * 理由：プレイヤーのロックオン状態を判定してリムライトの色を変更し、二本の武器と本体に適切なシェーダー設定を施して描画するため。
 */
void MidBoss::DrawLit()
{
	if (!m_spModel) return;
	Math::Vector3 finalColor = EnemyParam::Visual::RIM_COLOR;
	auto spTarget = m_wpTarget.lock();
	if (auto spPlayer = std::dynamic_pointer_cast<Player>(spTarget))
	{
		if (spPlayer->IsLockOn() && spPlayer->GetLockTarget().lock().get() == this)
		{
			finalColor = { 1.0f, 0.8f, 0.0f };
		}
	}

	auto applyRim = [&]() {
		auto& shader = KdShaderManager::Instance().m_StandardShader;
		shader.SetLimLightEnable(true);
		shader.SetLimLight(finalColor);
		shader.SetLimLightIntensity(5.0f);
		};

	applyRim(); EnemyBase::DrawLit();
	applyRim(); if (m_spRightWeapon) m_spRightWeapon->DrawLit();
	applyRim(); if (m_spLeftWeapon)  m_spLeftWeapon->DrawLit();

	KdShaderManager::Instance().m_StandardShader.SetLimLightEnable(false);
	KdShaderManager::Instance().m_StandardShader.SetDissolve(0.0f);
}
/**
 * @brief 外部データ（JSON）の反映
 * 理由：親クラスのロード処理に加え、中ボス特有の「発狂モード移行HP比率」などの数値を安全に取得し反映させるため。
 */
void MidBoss::LoadParameters(const nlohmann::json& _j)
{
	CharacterBase::LoadParameters(_j);
	EnemyBase::LoadParameters(_j);
	auto GetFloat = [&](const std::string& key, float def) {
		return (_j.contains(key) && _j[key].is_number()) ? (float)_j[key].get<double>() : def;
	};

	m_HpRatio = GetFloat("hpRatio", MidBossParam::DUAL_WIELD_RATIO);
	m_Hp = m_MaxHp;
}

/**
 * @brief デバッグ情報の表示
 * 理由：実行中に索敵範囲をスライダーで調整したり、個体別のデバッグ表示のON/OFFを切り替えて挙動を確認するため。
 */
void MidBoss::OnImGui()
{
	ImGui::Text("Speed: %.2f", m_Speed);
	ImGui::SliderFloat("Search Radius", &m_searchRadius, 0.0f, 20.0f);
	
	static bool localShow = false;
	localShow = (m_pDebugWire != nullptr);
	if (ImGui::Checkbox(U8("この個体のデバッグ表示"), &localShow))
	{
		this->SetDebugDisplay(localShow);
	}
}