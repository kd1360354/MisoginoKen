#include "EnemyBase.h"
#include "../../../../Application/Scene/BaseScene/BaseScene.h"

#include "../../Ui/EnemyUi/EnemyUi.h"
#include "../CharactorBase.h"


/**
 * @brief 初期化処理
 * 理由：アニメーターの生成など、インスタンス生成時に最低限必要なリソースを確保するため。
 */
void EnemyBase::Init()
{
	if (!m_spAnimator)
	{
		m_spAnimator = std::make_shared<KdAnimator>();
	}
}

/**
 * @brief フレーム更新のメインルーチン
 * 理由：SRPに基づき、「アタッチメント更新」「AI実行」「演出管理」の各サブ関数を呼び出す司令塔に特化させるため。
 */
void EnemyBase::Update()
{
	UpdateAttachment();

	if (m_spRootNode)
	{
		m_spRootNode->Execute();
	}

	UpdateEffectStatus();
}

/**
 * @brief 描画処理（不透明物）
 * 理由：デバッグ表示、リムライト設定、ディゾルブ設定など、描画直前のシェーダー状態制御を一括管理するため。
 */
void EnemyBase::DrawLit()
{
	if (m_pDebugWire)
	{
		m_pDebugWire->Draw();
	}

	if (m_spModel && !m_isExpired)
	{
		auto& shader = KdShaderManager::Instance().m_StandardShader;

		shader.SetLimLightEnable(true);
		shader.SetLimLight(m_rimLightColor);
		shader.SetLimLightIntensity(EnemyParam::Visual::RIM_INTENSITY);
		shader.SetLimLightPower(m_rimPower);
		shader.SetDissolve(m_dissolveThreshold);

		CharacterBase::DrawLit();

		shader.SetDissolve(EnemyParam::Visual::DISSOLVE_RESET);
	}
}

/**
 * @brief 攻撃判定の発生座標を決定
 * 理由：アニメーションに連動した部位（手足など）の特定と、判定のオフセット位置を算出するため。
 */
void EnemyBase::SphierAttack(EnemyNode Node, Math::Vector3 OffsetPos, float Radius)
{
	Math::Vector3 attackPos = m_pos;

	if (Node != EnemyNode::None && m_spModel)
	{
		std::string nodeName = "";
		switch (Node)
		{
		case EnemyNode::LeftHand:  nodeName = EnemyParam::Node::LEFT_HAND;  break;
		case EnemyNode::RightHand: nodeName = EnemyParam::Node::RIGHT_HAND; break;
		case EnemyNode::LeftFoot:  nodeName = EnemyParam::Node::LEFT_FOOT;  break;
		case EnemyNode::RightFoot: nodeName = EnemyParam::Node::RIGHT_FOOT; break;
		case EnemyNode::Hips:      nodeName = EnemyParam::Node::HIPS;       break;
		default: break;
		}

		if (auto pNode = m_spModel->FindWorkNode(nodeName))
		{
			Math::Matrix worldMat = pNode->m_worldTransform * m_mWorld;
			attackPos = worldMat.Translation();
		}
	}

	attackPos += OffsetPos;

	float finalDamage = GetBaseAttackPower() * GetCurrentAttackPower();
	ExecuteAttackCollision(attackPos, Radius, finalDamage);
}

/**
 * @brief 索敵および旧AIロジック
 * 理由：BTを使用しない場合の行動決定と、デバッグ用の索敵範囲の可視化を担うため。
 */
void EnemyBase::SearchAttackSphere(float SearthRadius, Math::Vector3 SearthOffsetPos,
	float AttackRadius, Math::Vector3 AttackOffsetPos)
{
	if (m_pDebugWire)
	{
		Math::Vector3 worldPos = m_mWorld.Translation();
		m_pDebugWire->AddDebugSphere(worldPos + SearthOffsetPos, SearthRadius, kGreenColor);
		m_pDebugWire->AddDebugSphere(worldPos + AttackOffsetPos, AttackRadius, kBlueColor);
	}

	if (m_spRootNode || m_isExpired) return;

	// ★ SceneManager::GetPlayer() → m_wpTarget 経由に変更
	auto spTarget = m_wpTarget.lock();
	if (!spTarget) return;

	if (m_state == EnemyAnimState::Death || m_state == EnemyAnimState::Hit
		|| m_state == EnemyAnimState::KonckBack)
	{
		if (m_state == EnemyAnimState::Death) return;
		if (m_spAnimator->IsAnimationEnd()) m_state = EnemyAnimState::Idle;
		return;
	}

	if (m_state >= EnemyAnimState::Attack01 && m_state <= EnemyAnimState::Roar)
	{
		if (!m_spAnimator->IsAnimationEnd()) return;
	}

	// ★ spPlayer->GetPos() → spTarget->GetPos()
	Math::Vector3 toTarget = spTarget->GetPos() - m_pos;
	float distance = toTarget.Length();

	if (distance <= AttackRadius)
	{
		m_state = EnemyAnimState::Attack01;
		m_hasHitPlayerInCurrentAttack = false;
	}
	else if (distance <= SearthRadius)
	{
		m_state = EnemyAnimState::WalkLoop;
	}
	else
	{
		m_state = EnemyAnimState::Idle;
	}
}

/**
 * @brief 座標・回転の確定計算
 * 理由：ターゲットとの位置関係から向きを決定し、最終的なワールド行列を構築するため。
 */
void EnemyBase::UpdateTransform(const Math::Vector3& scale)
{
	// ★ SceneManager::GetPlayer() → m_wpTarget 経由に変更
	auto spTarget = m_wpTarget.lock();
	if (!spTarget) return;

	// ★ spPlayer->GetPos() → spTarget->GetPos()
	Math::Vector3 toTarget = spTarget->GetPos() - m_pos;
	toTarget.y = 0;
	toTarget.Normalize();

	Math::Matrix enemyNowRot = Math::Matrix::CreateRotationY(
		DirectX::XMConvertToRadians(m_NowAngle));
	Math::Vector3 forwardDir = Math::Vector3::TransformNormal(
		Math::Vector3(0, 0, EnemyParam::Move::FORWARD_DIR_Z), enemyNowRot);

	if (m_state == EnemyAnimState::WalkLoop || m_state == EnemyAnimState::RunLoop)
	{
		float dot = std::clamp(forwardDir.Dot(toTarget), -1.0f, 1.0f);
		float angle = DirectX::XMConvertToDegrees(acosf(dot));

		Math::Vector3 cross = forwardDir.Cross(toTarget);
		float rotateAngle = std::min(angle, m_rotation_speed);
		m_NowAngle += (cross.y >= 0.0f) ? rotateAngle : -rotateAngle;

		if (angle < EnemyParam::Move::START_ANGLE_THR)
		{
			m_pos += forwardDir * m_Speed;
		}
	}

	m_mWorld = Math::Matrix::CreateScale(scale)
		* Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle))
		* Math::Matrix::CreateTranslation(m_pos);
}

/**
 * @brief 被弾時の処理
 * 理由：HPの減算だけでなく、被弾アニメーションの再生や死亡判定への分岐を管理するため。
 */
void EnemyBase::OnHit(float Damage)
{
	m_Hp -= Damage;
	if (m_Hp <= 0)
	{
		m_Hp = 0;
		ProcessDeath();
	}
	else
	{
		m_state = EnemyAnimState::Hit;
		if (auto anim = m_spModel->GetData()->GetAnimation("Hit"))
		{
			m_spAnimator->SetAnimation(anim, false);
		}
		m_isHit = true;
	}
}

/**
 * @brief 外部データ（JSON）の反映
 * 理由：索敵範囲、攻撃力、移動速度などの動的なパラメータを、ソースコードを変更せずに調整可能にするため。
 */
void EnemyBase::LoadParameters(const nlohmann::json& _j)
{
	CharacterBase::LoadParameters(_j);

	auto GetFloat = [&](const std::string& key, float def) {
		return (_j.contains(key) && _j[key].is_number()) ? (float)_j[key].get<double>() : def;
		};

	m_searchRadius = GetFloat("searchRadius", EnemyParam::Default::SEARCH_RADIUS);
	m_attackRadius = GetFloat("attackRadius", EnemyParam::Default::ATTACK_RADIUS);
	m_enemy_attack_power = GetFloat("baseattackPower", EnemyParam::Default::ATTACK_POWER);
	m_Speed = GetFloat("moveSpeed", EnemyParam::Move::SPEED_DEFAULT);
	m_rotation_speed = GetFloat("rotationSpeed", EnemyParam::Move::ROTATION_SPEED);

	if (_j.contains("rimLightColor") && _j["rimLightColor"].is_array()
		&& _j["rimLightColor"].size() >= 3)
	{
		auto& r = _j["rimLightColor"];
		m_rimLightColor = { (float)r[0].get<double>(), (float)r[1].get<double>(), (float)r[2].get<double>() };
	}

	m_rimPower = GetFloat("rimPower", EnemyParam::Visual::RIM_POWER_DEF);
	m_display_hp = m_Hp;
}

/**
 * @brief 倒されたかどうかの判定取得
 */
bool EnemyBase::IsDefeated()
{
	return m_isDefeated;
}

/**
 * @brief 攻撃ヒット時の演出再生
 */
void EnemyBase::OnAttackHitPlayer(const Math::Vector3& playerPos)
{
	if (m_hitEffectHandle != EnemyParam::EFFECT_INVALID_HANDLE &&
		KdEffekseerManager::GetInstance().IsPlaying(m_hitEffectHandle)) return;

	Math::Vector3 effectPos = { playerPos.x, playerPos.y + ENEMY_EFFECT_VERTICAL_OFFSET, playerPos.z };

	Math::Matrix playerRot = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle));
	Math::Matrix effectWorldMatrix = playerRot * Math::Matrix::CreateTranslation(effectPos);

	auto wpEffect = KdEffekseerManager::GetInstance().Play("PlayerHitDamage.efk", effectPos);
	if (auto spEffect = wpEffect.lock()) {
		m_hitEffectHandle = spEffect->GetHandle();
		KdEffekseerManager::GetInstance().SetWorldMatrix(m_hitEffectHandle, effectWorldMatrix);
	}
}

/**
 * @brief デバッグ用ワイヤーフレームの表示切替
 */
void EnemyBase::SetDebugDisplay(bool show)
{
	if (show) {
		if (!m_pDebugWire) m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}
	else {
		m_pDebugWire.reset();
	}
}

/**
 * @brief 特定部位へのアタッチ更新
 */
void EnemyBase::UpdateAttachment()
{
	std::shared_ptr<KdGameObject> _spOwner = m_wpOwner.lock();
	if (!_spOwner) return;

	std::shared_ptr<CharacterBase> _spCharacter = std::dynamic_pointer_cast<CharacterBase>(_spOwner);
	if (!_spCharacter) return;

	switch (m_Node)
	{
	case EnemyNode::LeftHand:  m_nodeName = EnemyParam::Node::LEFT_HAND;  break;
	case EnemyNode::RightHand: m_nodeName = EnemyParam::Node::RIGHT_HAND; break;
	case EnemyNode::LeftFoot:  m_nodeName = EnemyParam::Node::LEFT_FOOT;  break;
	case EnemyNode::RightFoot: m_nodeName = EnemyParam::Node::RIGHT_FOOT; break;
	case EnemyNode::Hips:      m_nodeName = EnemyParam::Node::HIPS;       break;
	default: m_nodeName = ""; break;
	}

	if (const auto* _pNode = _spCharacter->GetModel()->FindNode(m_nodeName))
	{
		m_mWorld = _pNode->m_worldTransform * _spCharacter->GetMatrix();
	}
}

/**
 * @brief エフェクト等の状態監視
 */
void EnemyBase::UpdateEffectStatus()
{
	if (m_hitEffectHandle != EnemyParam::EFFECT_INVALID_HANDLE)
	{
		if (!KdEffekseerManager::GetInstance().IsPlaying(m_hitEffectHandle))
		{
			m_hitEffectHandle = EnemyParam::EFFECT_INVALID_HANDLE;
		}
	}
}

/**
 * @brief 死亡シーケンスの実行
 */
void EnemyBase::ProcessDeath()
{
	m_isDefeated = true;
	if (m_state == EnemyAnimState::Death) return;

	m_state = EnemyAnimState::Death;

	// ★ 逆転依存（自分がLastBossかどうか判定するコード）を完全に削除！
	// 敵共通の死亡アニメーションだけを再生します。
	if (auto anim = m_spModel->GetData()->GetAnimation("Death"))
	{
		m_spAnimator->SetAnimation(anim, false);
	}
}

/**
 * @brief 物理的な当たり判定の実行
 * 理由：CharacterBase のインターフェースで統一し、Player 以外もターゲットにできるようにするため。
 */
void EnemyBase::ExecuteAttackCollision(const Math::Vector3& pos, float radius, float damage)
{
	KdCollider::SphereInfo sphere;
	sphere.m_sphere.Center = pos;
	sphere.m_sphere.Radius = radius;
	sphere.m_type = KdCollider::Type::TypeDamage;

	// ★ SceneManager::GetPlayer() → m_wpTarget 経由に変更
	//   CharacterBase に Intersects / OnHit / GetPos が揃っているのでキャスト不要
	auto spTarget = m_wpTarget.lock();
	if (!spTarget || m_hasHitPlayerInCurrentAttack) return;

	std::list<KdCollider::CollisionResult> results;
	if (spTarget->Intersects(sphere, &results))
	{
		spTarget->OnHit(damage);
		m_hasHitPlayerInCurrentAttack = true;
		OnAttackHitPlayer(spTarget->GetPos());
	}
}