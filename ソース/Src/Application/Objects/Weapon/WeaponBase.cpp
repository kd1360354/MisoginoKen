// =================================================================
// WeaponBase.cpp (完全リファクタリング版)
// =================================================================
#include "WeaponBase.h"
#include "../Character/CharactorBase.h"
#include "../../../Framework/Interface/IDamageable.h" // 窓口だけを見る
#include "../../Scene/SceneManager.h"

void WeaponBase::Init()
{
	//m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	m_local_box.Center = Math::Vector3(0.0f, 0.0f, 0.5f);  // 手元から少し前にずらす
	m_local_box.Extents = Math::Vector3(0.1f, 0.1f, 0.6f); // 幅・高さ・長さ

	// 武器自身の攻撃力を初期化（定数などは適宜設定してください）
	m_damage = 37.0f;
}

void WeaponBase::Update()
{
	std::shared_ptr<KdGameObject> sp_owner = m_wpOwner.lock();
	if (sp_owner)
	{
		if (auto sp_character = std::dynamic_pointer_cast<CharacterBase>(sp_owner))
		{
			switch (m_hand)
			{
			case Hand::Left:  m_nodeName = NODE_NAME_LEFT_WEAPON;  break;
			case Hand::Right: m_nodeName = NODE_NAME_RIGHT_WEAPON; break;
			case Hand::Hip:   m_nodeName = NODE_NAME_HIP_WEAPON;   break;
			default:          m_nodeName = NODE_NAME_RIGHT_WEAPON; break;
			}

			const KdModelWork::Node* p_node = sp_character->GetModel()->FindNode(m_nodeName);
			if (p_node)
			{
				m_mWorld = m_WeaponRot * p_node->m_worldTransform * sp_character->GetMatrix();
			}
		}
	}
	// ★ 各武器クラスでバラバラにやっていた当たり判定処理をここに集約！
	if (m_isHitActive)
	{
		CheckAttackHit(SceneManager::Instance().GetObjList());
	}
}

void WeaponBase::DrawLit()
{
	// 1. 持ち主（中ボスやプレイヤー）の情報を取得
	std::shared_ptr<KdGameObject> spOwner = m_wpOwner.lock();
	if (spOwner)
	{
		// 2. 持ち主が CharacterBase（ディゾルブ値を持っているクラス）なら
		if (auto spCharacter = std::dynamic_pointer_cast<CharacterBase>(spOwner))
		{
			// 3. 持ち主のディゾルブ進捗を武器の描画にも適用する
			float threshold = spCharacter->GetDissolveThreshold();
			KdShaderManager::Instance().m_StandardShader.SetDissolve(threshold);
		}
	}

	if (m_pDebugWire) {
		m_pDebugWire->Draw();
	}

	// 4. 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

	// 5. 他のオブジェクトに影響を与えないよう念のためリセット（必要に応じて）
	// KdShaderManager::Instance().m_StandardShader.SetDissolve(0.0f);
}

// 通常攻撃判定
void WeaponBase::CheckAttackHit(const std::list<std::shared_ptr<KdGameObject>>& target_objects)
{
	// 判定Boxの構築
	DirectX::BoundingOrientedBox world_box;
	m_local_box.Transform(world_box, m_mWorld);

	KdCollider::BoxInfo attack_box;
	attack_box.m_type = KdCollider::TypeDamage;
	attack_box.m_Obox = world_box;

	std::list<KdCollider::CollisionResult> results;

	for (auto& target : target_objects)
	{
		// 1. 自分は殴らない
		if (target == m_wpOwner.lock()) continue;

		// 2. 世界全域ヒット防止：刀から 4.0m 以上離れた敵は判定すらしない
		float dist = Math::Vector3::Distance(m_mWorld.Translation(), target->GetPos());
		if (dist > 4.0f) continue;

		// 3. 多段ヒット防止：既に当たったリストに入っているかチェック
		bool isAlreadyHit = false;
		for (auto& wpObj : m_hitObjects) {
			if (wpObj.lock() == target) {
				isAlreadyHit = true;
				break;
			}
		}
		if (isAlreadyHit) continue; // 既に当たっていたらこの敵は無視

		// 4. 判定
		if (target->Intersects(attack_box, DirectX::SimpleMath::Matrix::Identity, &results))
		{
			if (auto damageable = std::dynamic_pointer_cast<IDamageable>(target))
			{
				damageable->OnHit(m_damage); // 正常な攻撃力（10等）が1回だけ入る

				// 5. 当たったリストに追加（これでこの「振り」の間は二度と当たらない）
				m_hitObjects.push_back(target);

				// 6. 必殺技ゲージ加算
				if (m_onHitCallback) {
					m_onHitCallback();
				}
			}
		}
	}
}

// 必殺技判定 (ここも IDamageable に修正！)
void WeaponBase::CheckAttackPowerAttackHit(const std::list<std::shared_ptr<KdGameObject>>& target_objects, const DirectX::SimpleMath::Vector3& attack_world_pos, float attack_radius, float power_damage)
{
	KdCollider::SphereInfo attack_sphere;
	// ★修正：Player側で計算済みの座標をそのまま使う
	attack_sphere.m_sphere.Center = attack_world_pos;
	attack_sphere.m_sphere.Radius = attack_radius;
	attack_sphere.m_type = KdCollider::TypeDamage;

	// デバッグ表示用（当たらない時はこれを出して位置を確認！）
	if (m_pDebugWire) {
		m_pDebugWire->AddDebugSphere(attack_sphere.m_sphere.Center, attack_radius, kRedColor);
	}

	std::list<KdCollider::CollisionResult> results;
	for (auto& target : target_objects)
	{
		if (target == m_wpOwner.lock()) continue;
		if (target->Intersects(attack_sphere, &results))
		{
			if (auto damageable = std::dynamic_pointer_cast<IDamageable>(target))
			{
				damageable->OnHit(power_damage);
			}
		}
	}
}

void WeaponBase::OnImGui()
{
	// 個別のデバッグ表示スイッチ
	static bool localShow = false;
	// 現在 unique_ptr があるかどうかで初期値を決める
	localShow = (m_pDebugWire != nullptr);

	if (ImGui::Checkbox(U8("この個体のデバッグ表示"), &localShow))
	{
		this->SetDebugDisplay(localShow);
	}
}
