#pragma once
#include "../../../Framework/../Framework/Interface/IDamageable.h" // インターフェースを忘れずに

enum class Hand { Hip, Left, Right };

class WeaponBase : public KdGameObject
{
public:
	WeaponBase() {}
	~WeaponBase() override {}

	void Init() override;
	void Update() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override {}

	// 当たり判定：引数から余計なものを削り、シンプルに
	void CheckAttackHit(const std::list<std::shared_ptr<KdGameObject>>& targetObjects);

	// 必殺技用
	void CheckAttackPowerAttackHit(const std::list<std::shared_ptr<KdGameObject>>& targetObjects,
		const DirectX::SimpleMath::Vector3& AttackOffsetPos,
		float AttackRadius,
		float PowerDamage);

	void SetLocalMatrix(const Math::Matrix& localMat) { m_WeaponRot = localMat; }
	void SetOwner(const std::shared_ptr<KdGameObject>& owner) { m_wpOwner = owner; }
	void SetHand(Hand _hand) { m_hand = _hand; }

	// 攻撃開始時にこれを呼んでヒットリストをクリアする
	void ResetHit() { m_hitObjects.clear(); }

	void SetHitActive(bool isActive) { m_isHitActive = isActive; m_hitObjects.clear(); }
	bool IsHitActive() const { return m_isHitActive; }

	// 攻撃力の設定・取得
	void SetDamage(float dmg) { m_damage = dmg; }
	float GetDamage() const { return m_damage; }
	void SetWorldMatrix(const Math::Matrix& worldMat) { m_mWorld = worldMat; }
	void ResetHitList() { m_hitObjects.clear(); }
	void OnImGui()override;

	// ★追加: 外部から攻撃識別子（コンボ数など）をセットできるようにする
	void SetAttackIdentifier(int id) { m_current_attack_identifier = id; }

	void SetOnHitCallback(std::function<void()> cb) { m_onHitCallback = cb; }
protected:
	// --- 武器の基本ステータス ---
	float m_damage = 0.0f; // ★ここに追加！

	// 1回の攻撃で同じ敵を二度殴らないためのリスト（重要！）
	std::list<std::weak_ptr<KdGameObject>> m_hitObjects;

	bool m_isHitActive = false;
	std::shared_ptr<KdModelData> m_spModel = nullptr;
	std::weak_ptr<KdGameObject> m_wpOwner;
	Hand m_hand = Hand::Left;
	Math::Matrix m_WeaponRot;
	std::string m_nodeName;
	// ★追加: 子クラスにあった変数を親に移動
	int m_current_attack_identifier = ATTACK_IDENTIFIER_DEFAULT;
	DirectX::BoundingOrientedBox m_local_box; // privateから移動（Katana等でサイズ調整するため）

	std::function<void()> m_onHitCallback = nullptr;
};