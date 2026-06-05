#pragma once

#include "../../../Framework/Interface/IDamageable.h"


class WeaponBase;
class KdGameObject;

/**
 * @class CharacterBase
 * @brief プレイヤーと敵の共通基底クラス
 * 理由：移動、重力、アニメーション、衝突判定といった「キャラクターとしての基本機能」を
 * 一括管理し、子クラスでの重複実装を防ぐ。
 */
class CharacterBase : virtual public KdGameObject, public IDamageable
{
public:
	CharacterBase() {}
	~CharacterBase() {}

	// --- I. ライフサイクル (Override) ---
	void Init() override;
	void Update() override;
	void DrawLit() override;
	//void DrawUnLit() override;
	void GenerateDepthMapFromLight() override;
	void ApplyGravity();
	// --- II. 衝突判定・座標補正 (PostUpdate系) ---
	/** @brief レイ判定による接地・段差乗り越え処理 */
	virtual	void RayPostUpdate();
	/** @brief 衝突後の個別応答処理（必要に応じて子クラスで実装） */
	virtual void HitPostUpdate() {}
	/** @brief 球体判定による壁・押し戻し処理 */
	virtual void SpherePushBack(Math::Vector3 LowSpherePos = PUSHBACK_SPHERE_LOW_POS_DEFAULT,
		float SphereRadius = PUSHBACK_SPHERE_RADIUS_DEFAULT,
		Math::Vector3 upperSpherePos = PUSHBACK_SPHERE_UPPER_POS_DEFAULT);

	// --- III. アニメーション制御 ---
	/** @brief 速度指定ありのアニメーション更新 */
	void AnimeUpdate(float speed);
	/** @brief デフォルト速度(1.0)でのアニメーション更新 */
	void AnimeUpdate();

	// --- IV. パラメータ管理・外部操作 ---
	void LoadParameters(const nlohmann::json& _j) override;
	void SetHitStop(float duration) { m_HitStopTimer = duration; };
	virtual void OnHit(float damage) override = 0; // 純粋仮想関数：子クラスで固有の被弾処理を強制

	// リスト登録
	void RegistHitObject(const std::shared_ptr<KdGameObject>& object) { m_wpHitObjectList.push_back(object); }
	void SetOwner(const std::shared_ptr<KdGameObject>& owner) { m_wpOwner = owner; }

	// ゲッター群
	std::shared_ptr<KdModelWork>& GetModel() { return m_spModel; }
	float GetDissolveThreshold() const { return m_dissolveThreshold; }
	const Math::Vector3& GetRimLightColor() const { return m_rimLightColor; }
	float GetRimLightIntensity() const { return m_rimLightIntensity; }
	bool IsRimLightEnabled() const { return m_isRimLightEnabled; }
	float GetHp() const { return m_Hp; }
protected:
	// --- A. 物理・時間制御 ---
	float m_HitStopTimer = 0.0f;           // 攻撃ヒット時の時間停止タイマー
	const float m_GravityPow = GRAVITY_ACCELERATION; // 重力加速度（定数）
	float m_Gravity = 0.0f;                // 現在の下向きの速度

	// --- B. リソース・コンポーネント ---
	std::weak_ptr<KdGameObject>  m_wpOwner;
	std::shared_ptr<KdModelWork> m_spModel = nullptr;
	std::shared_ptr<KdAnimator>  m_spAnimator = nullptr;

	// --- C. アニメーション・旋回パラメータ ---
	float m_animBlendRate = 0.0f;
	const float m_animBlendSpeed = ANIM_BLEND_SPEED;
	const float m_AdjustHeight = GROUND_ADJUST_HEIGHT; // 地面からの浮かせ幅
	float m_NowAngle = 0;
	Math::Vector3 m_Scale = { 1.0f,1.0f,1.0f };
	bool m_isGround = true;


	// --- D. 基本ステータス ---
	float m_Hp = 0;
	float m_MaxPlayerHp = PLAYER_MAX_HP_DEFAULT;
	float m_Stamina = 0;
	float m_MaxPlayerStamina = PLAYER_MAX_STAMINA_DEFAULT;
	bool m_isSuperArmor = false;           // 被弾によるのけぞりを無効化するか

	// --- E. 演出・ビジュアル ---
	float m_dissolveThreshold = 0.0f;      // 消滅エフェクトの閾値
	Math::Vector3 m_rimLightColor = { 1.0f, 1.0f, 1.0f };
	float m_rimLightIntensity = 0.0f;
	bool m_isRimLightEnabled = false;

	// 判定用リスト
	std::vector<std::weak_ptr<KdGameObject>> m_wpHitObjectList{};
};