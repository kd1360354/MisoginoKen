#pragma once
#include "../CharactorBase.h"
#include <map>
#include "../../../../Framework/AI/BehaviorTree/Core/BTNode.h"

class EnemyUi;

enum class EnemyNode {
	None,
	LeftHand,
	RightHand,
	LeftFoot,
	RightFoot,
	Hips
};

enum class EnemyAnimState {
	Idle,
	WalkLoop,
	RunLoop,
	Attack01,
	Attack02,
	Attack03,
	Attack04,
	Attack05,
	Attack06,
	Jump,
	JumpAttack,
	Roar,
	KonckBack,
	Hit,
	Death
};

class EnemyBase : virtual public CharacterBase
{
public:
	EnemyBase() {}
	virtual ~EnemyBase() override {}

	void Init() override;
	void Update() override;
	void DrawLit() override;

	// --- 攻撃・判定関連 ---
	void SphierAttack(EnemyNode Node = EnemyNode::None, Math::Vector3 OffsetPos = {}, float Radius = 1.0f);
	void SearchAttackSphere(float SearthRadius = 1.0f, Math::Vector3 SearthOffsetPos = {}, float AttackRadius = 1.0f, Math::Vector3 AttackOffsetPos = {});
	void OnAttackHitPlayer(const Math::Vector3& playerPos);

	// --- 変形・状態制御 ---
	void UpdateTransform(const Math::Vector3& scale = { 1.0, 1.0, 1.0 });
	void OnHit(float damage) override;

	// --- Getter / Setter ---
	void SetHp(float hp) { m_Hp = hp; }
	void SetBaseAttackPower(float power) { m_enemy_attack_power = power; }
	float GetDisplayHp() { return m_display_hp; }
	float GetMaxHP() { return m_MaxHp; }
	float GetMoveSpeed() const { return m_Speed; }
	float GetRotationSpeed() const { return m_rotation_speed; }
	float GetBaseAttackPower() const { return m_enemy_attack_power; }

	void LoadParameters(const nlohmann::json& _j) override;
	EnemyAnimState GetState() { return m_state; }
	std::shared_ptr<KdAnimator> GetAnimator() { return m_spAnimator; }

	void SetOnDefeatedCallback(std::function<void()> cb) { m_onDefeated = cb; }
	bool IsDefeated();
	void SetEnemyUi(std::shared_ptr<EnemyUi> ui) { m_wpEnemyUi = ui; }
	void SetExpired(bool Expired) { m_isExpired = Expired; }
	void SetSpeed(float speed) { m_Speed = speed; }
	void SetRimLightParams(const Math::Vector3& color, float power) {
		m_rimLightColor = color;  // CharacterBase のメンバを使う
		m_rimPower = power;
	}

	float GetDissolveThreshold() const { return m_dissolveThreshold; }
	void SetDissolveThreshold(float dissolve) { m_dissolveThreshold = dissolve; }
	float GetRimPower() const { return m_rimPower; }

	bool IsInCombat() const {
		return m_state != EnemyAnimState::Death && m_state != EnemyAnimState::Idle;
	}

	bool IsHit() const { return m_isHit; }
	void SetHitFlag(bool flag) { m_isHit = flag; }
	void SetState(EnemyAnimState state) { m_state = state; }
	void SetHasHitPlayerInCurrentAttack(bool hasHit) { m_hasHitPlayerInCurrentAttack = hasHit; }

	float GetSearchRadius() const { return m_searchRadius; }
	float GetAttackRadius() const { return m_attackRadius; }
	int GetDeathEffectHandle() const { return m_deathEffectHandle; }
	const std::unique_ptr<KdCollider>& GetCollider() const { return m_pCollider; }
	float GetHpRatio() const { return m_HpRatio; }
	void SetCurrentAttackPower(float power) { m_currentAttackPower = power; }
	float GetCurrentAttackPower() const { return m_currentAttackPower; }

	void SetDebugDisplay(bool show) override;

	// ★ ターゲット管理
	void SetTarget(std::weak_ptr<CharacterBase> target) { m_wpTarget = target; }
	std::weak_ptr<CharacterBase> GetTarget() const { return m_wpTarget; }

protected:
	void UpdateAttachment();
	void UpdateEffectStatus();
	virtual void ProcessDeath();
	void ExecuteAttackCollision(const Math::Vector3& pos, float radius, float damage);

	// --- メンバ変数 ---
	bool m_isDefeated = false;
	std::weak_ptr<EnemyUi> m_wpEnemyUi;
	int m_deathEffectHandle = -1;

	float m_MaxHp = 0.0f;
	float m_Speed = 0.15f;
	float m_searchRadius = 0.0f;
	float m_attackRadius = 0.0f;
	float m_enemy_attack_power = 100.0f;
	float m_display_hp = 0;
	float m_hit_timer = 0.0f;
	float m_rotation_speed = 10.0f;

	EnemyAnimState m_state = EnemyAnimState::Idle;
	EnemyAnimState m_prevState = EnemyAnimState::Attack01;
	bool m_has_acted_first_time = false;
	bool m_has_roared_50_percent = false;
	bool m_isActivated = false;

	bool m_isHit = false;
	int m_hitEffectHandle = -1;
	bool m_hasHitPlayerInCurrentAttack = false;
	Math::Vector3 m_rimLightColor = EnemyParam::Visual::RIM_COLOR;
	float m_rimPower = EnemyParam::Visual::RIM_POWER_DEF;
	std::shared_ptr<BTNode> m_spRootNode = nullptr;

	bool m_isDissolving = false;
	float m_HpRatio = 0.5f;
	float m_currentAttackPower = 0.0f;
	bool m_isRimLightEnabled = false;
	// 敵の場合はPlayerを、Playerの場合はロックオン対象の敵を入れる
	std::weak_ptr<CharacterBase> m_wpTarget;
private:
	EnemyNode m_Node = EnemyNode::None;
	std::string m_nodeName;

	

	std::function<void()> m_onDefeated;
};