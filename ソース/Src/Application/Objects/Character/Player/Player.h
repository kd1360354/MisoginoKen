#pragma once

#include "../CharactorBase.h"
#include "../../../Constants/CharactorConstants/Player/PlayerConstants.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState.h"
#include "../../Weapon/WeaponBase.h"
#include "../../Camera/CameraBase.h"

// 前方宣言
class TPSCamera;
class KdEffekseerObject;
class EnemyBase;

/**
 * @class Player
 * @brief プレイヤーキャラクターの制御クラス
 * 理由：CharacterBaseを継承し、Stateパターンによるアクション管理と
 * 武器、カメラ、エフェクトのサブシステムを統合する。
 */
class Player : virtual public CharacterBase
{
public:
	Player() {}
	~Player() override {}

	// =========================================================
	// I. 基本ライフサイクル (Override)
	// =========================================================
	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;
	void OnImGui() override;

	// =========================================================
	// II. ステートマシン & アクション遷移
	// =========================================================
	/** @brief ステート切り替え実行 */
	void ChangeState(std::shared_ptr<PlayerState> nextState)
	{
		if (m_pCurrentState) m_pCurrentState->Exit(*this);
		m_pCurrentState = nextState;
		m_pCurrentState->Enter(*this);
	}

	/** @brief 地上でのアクション入力による遷移判定 */
	void HandleActionTransitions();
	/** @brief ステートに基づく旋回と物理計算の統合更新 */
	void UpdateRotationAndPhysics(float deltaTime);
	/** @brief 被弾イベント処理 */
	void OnHit(float damage) override;

	// =========================================================
	// III. ステートから利用するアクション・サブシステム
	// =========================================================

	// --- 戦闘制御 ---
	void AutoTargetClosestEnemy();          // 最寄りの敵へ瞬時に向く
	void ExecuteCommonCancel(float normTime); // 回避/ジャンプによるキャンセル
	void UpdateAttackStep(float normTime);    // 攻撃時の踏み込み移動
	void GenerateAreaAttack(float radius, float damage); // 範囲攻撃の発生

	// --- リソース・コスト管理 ---
	bool ConsumeStamina(float cost);        // スタミナ消費（成否を返す）
	void ResetStaminaRecoveryDelay();       // スタミナ回復ディレイの開始
	void ConsumePowerCost(float cost);      // 必殺技ゲージ消費
	void GainPowerAttackCost(float amount); // 必殺技ゲージ加算
	void AddPowerCost(float gain);          // (ラッパー) ゲージ加算

	// --- 演出（サウンド・エフェクト） ---
	void PlayEffect(const std::string& name, const Math::Vector3& pos);
	void PlaySound(const std::string& path);

	// =========================================================
	// IV. パラメータアクセス (Getter / Setter)
	// =========================================================

	// --- 基本ステータス (HP/スタミナ) ---
	int   GetHP() { return (int)m_Hp; }
	int   GetMaxHP() { return (int)m_MaxPlayerHp; }
	void  SetHp(float hp) { m_Hp = hp; }
	float GetDisplayHp() { return m_DisplayHp; }
	float GetStamina() { return m_Stamina; }
	float GetMaxStamina() { return m_MaxPlayerStamina; }
	bool  IsGround() const { return m_isGround; }
	void  SetIsGround(bool isGround) { this->m_isGround = isGround; }

	// --- 物理・座標・角度 ---
	void  AddVelocity(const Math::Vector3& force) { this->m_vecVelocity += force; }
	void  AddActionVelocity(const Math::Vector3& vel) { this->m_actionVelocity += vel; }
	float GetGravity() const { return m_Gravity; }
	void  SetGravity(float gravity) { this->m_Gravity = gravity; }
	float GetAngle() const { return this->m_NowAngle; }
	const float& GetRotateAngle() const { return m_NowAngle; }
	const Math::Matrix& GetWorldMatrix() const { return this->m_mWorld; }
	void  UpdateFinalMatrix();
	void  ApplyGravity();

	// --- 移動・旋回設定 ---
	float GetMoveSpeed() const { return m_speed; }
	void  SetMoveSpeed(float speed) { m_speed = speed; }
	float GetJumpPower() const { return m_JumpPower; }
	void  SetJumpPower(float JumpPower) { m_JumpPower = JumpPower; }
	float GetRotationSpeed() const { return m_rotationSpeed; }
	void  SetRotationSpeed(float RotSpeed) { m_rotationSpeed = RotSpeed; }

	// --- 戦闘・コンボ設定 ---
	void SetAttackPower(float power) { m_attackPower = power; }
	float   GetAttackPower() const { return m_attackPower; }
	int   GetComboCount() const { return m_comboCount; }
	float GetPowerAttackCost() { return m_PowerAttackCost; }
	float GetPowerAttackMaxCost() { return (float)PlayerParam::PowerAttack::MAX_COST; }
	float GetPowerCost() const { return m_powerCost; }
	void  SetInvincible(bool isInv) { m_isInvincible = isInv; }

	// --- 回避・攻撃移動設定 ---
	float GetMaxAttackMoveDistance() const { return m_maxAttackMoveDistance; }
	void  SetMaxAttackMoveDistance(float val) { m_maxAttackMoveDistance = val; }
	float GetAttackMoveSpeed() const { return m_attackMoveSpeed; }
	void  SetAttackMoveSpeed(float val) { m_attackMoveSpeed = val; }
	float GetMaxDodgeMoveDistance() const { return m_maxDodgeMoveDistance; }
	void  SetMaxDodgeMoveDistance(float val) { m_maxDodgeMoveDistance = val; }
	float GetDodgeMoveSpeed() const { return m_DodgeMoveSpeed; }
	void  SetDodgeMoveSpeed(float val) { m_DodgeMoveSpeed = val; }

	// --- スタミナ調整設定 ---
	float GetAttackStaminaCost() const { return m_AttackStaminaCost; }
	void  SetAttackStaminaCost(float val) { m_AttackStaminaCost = val; }
	float GetDodgeStaminaCost() const { return m_DodgeStaminaCost; }
	void  SetDodgeStaminaCost(float val) { m_DodgeStaminaCost = val; }
	float GetBlockHitStaminaCost() const { return m_BlockHitStaminaCost; }
	void  SetBlockHitStaminaCost(float val) { m_BlockHitStaminaCost = val; }
	float GetRunStaminaCostPerSec() const { return m_RunStaminaCostPerSec; }
	void  SetRunStaminaCostPerSec(float val) { m_RunStaminaCostPerSec = val; }
	float GetStaminaRecoveryDelay() const { return m_StaminaRecoveryDelay; }
	void  SetStaminaRecoveryDelay(float val) { m_StaminaRecoveryDelay = val; }
	float GetStaminaRecoverRate() const { return m_StaminaRecoverRate; }
	void  SetStaminaRecoverRate(float val) { m_StaminaRecoverRate = val; }
	float GetBlockRecoveryRate() const { return m_BlockRecoveryRate; }
	void  SetBlockRecoveryRate(float val) { m_BlockRecoveryRate = val; }

	// --- アイテム・音響 ---
	int   GetPotionCount() const { return m_HpRecoveryItemNum; }
	void  SetPotionCount(int PotionCount) { m_HpRecoveryItemNum = PotionCount; }
	float GetPotionHealRate() const { return m_PotionHealRate; }
	void  SetPotionHealRate(float val) { m_PotionHealRate = val; }
	float GetHpDecreaseRate() const { return m_HpDecreaseRate; }
	void  SetHpDecreaseRate(float val) { m_HpDecreaseRate = val; }
	float GetAudioVolume() const { return m_AudioVolume; }
	void  SetAudioVolume(float val) { m_AudioVolume = val; }
	float GetAudioSEVolume() const { return m_AudioSEVolume; }
	void  SetAudioSEVolume(float val) { m_AudioSEVolume = val; }

	// --- 状態フラグ ---
	bool  IsLockOn() const { return this->m_isLockOn; }
	bool  IsMovingForward() const { return m_isMovingForward; }
	bool  IsMovingBackward() const { return m_isMovingBackward; }
	bool  IsMovingLeft() const { return m_isMovingLeft; }
	bool  IsMovingRight() const { return m_isMovingRight; }

	void UpdateLockOnSystems();
	void ReleaseLockOn();
	void UpdateInternalStatuses(float deltaTime);
	void UpdateDeathSequence(float deltaTime);
	void AnalyzeInput(Math::Vector3& outDir, bool& outMoveFlg, bool& outRunFlg);
	bool CheckStateTransitions();
	void UpdateCurrentState(float deltaTime);
	void UpdateRotation(const Math::Vector3& inputDir);
	void UpdatePhysics(const Math::Vector3& inputDir, bool moveFlg);
	void FinalizeTransform();
	// =========================================================
	// V. コンポーネント取得 & システム
	// =========================================================
	std::shared_ptr<WeaponBase> GetKatana() const { return m_spKatana; }
	std::shared_ptr<WeaponBase> GetSaya() const { return m_spSaya; }
	std::shared_ptr<KdAnimator> GetAnimator() { return m_spAnimator; }
	std::weak_ptr<KdEffekseerObject> GetSmokeEffect() const { return m_wpSmokeEffect; }
	const std::weak_ptr<EnemyBase>& GetLockTarget() const { return m_wpLockTarget; }
	const char* GetTypeName() const override { return "Player"; }

	

	// カメラ
	void SetCamera(const std::shared_ptr<CameraBase>& camera) { m_wpCamera = camera; }
	std::shared_ptr<CameraBase> GetCamera() const { return m_wpCamera.lock(); }
	Math::Matrix GetCameraWorldMatrix() const;

	// =========================================================
	// VI. 内部更新・非公開サブシステム
	// =========================================================
	void LoadParameters(const nlohmann::json& _j) override;
	void LoadPrefab(const std::string& jsonFilePath);
	bool ProcessHitStop(float deltaTime);
	void UpdateInPut(const Math::Matrix& cameraRotYMat, Math::Vector3& outDir, bool& outMoveFlg, bool& outRunFlg);
	void UpdateTimers(float deltaTime);
	void RecoverStamina(float deltaTime);
	void UpdateDisplayHP(float deltaTime);
	void UpdateWeaponTransform();
	void UpdateMovementAndPhysics(const Math::Vector3& inputDir, bool isMoving, bool isRunning);
	void UpdateLockOn();

	std::shared_ptr<EnemyBase> FindNearestLockOnTarget();
	std::shared_ptr<EnemyBase> FindNearestEnemy();

private:
	// =========================================================
	// VII. 🛡️ メンバ変数
	// =========================================================

	std::shared_ptr<PlayerState> m_pCurrentState;

	// --- 1. アニメーション & ステート管理 ---
	float m_AnimationNormalizedTime = 0.0f;
	bool  m_KnockDownAnime = true;
	float m_KnockDownTimer = 0.0f;

	// --- 2. 戦闘・コンボ・必殺技 ---
	int   m_attackDamage = 0;
	float   m_attackPower = 0;
	int   m_comboCount = 0;
	bool  m_isComboInputReserved = false;
	bool  m_isSpecialAttackActive = false;
	bool  m_isSpecialAttackHitCalled = false;
	bool  m_isHitOccurredThisAttack = false;
	float m_PowerAttackCost = 0.0f;
	float m_hitTimer = 0.0f;
	bool  m_hasMovedThisAttack = false;
	float m_powerCost = 0.0f;

	// --- 3. ステータス (HP / スタミナ / ガード) ---
	float m_DisplayHp = 0;
	float m_StaminaRecoverRate = 30.0f;
	float m_BlockRecoveryRate = 2.0f;
	float m_GuardHitDamage = 0.0f;
	int   m_GuardHitCount = 0;
	int   m_HpRecoveryItemNum = 10;
	float m_StaminaRecoveryDelayTimer = 0.0f;
	float m_StaminaRecoveryDelay = 1.0f;
	float m_BlockHitStaminaCost = 15.0f;
	float m_AttackStaminaCost = 10.0f;
	float m_PotionHealRate = 0.4225f;
	float m_HpDecreaseRate = 3.0f;

	// --- 4. サウンド & 音量 ---
	float m_AudioVolume = 0.2f;
	float m_AudioSEVolume = 0.5f;
	std::shared_ptr<KdSoundInstance> m_spSlashSound;

	// --- 5. 物理・移動・回避パラメータ ---
	Math::Vector3 m_vecVelocity = Math::Vector3::Zero;
	Math::Vector3 m_actionVelocity = Math::Vector3::Zero;
	float m_speed = 0.0f;
	float m_JumpPower = 0.8f;
	float m_rotationSpeed = 10.0f;
	float m_maxAttackMoveDistance = 1.5f;
	float m_currentAttackMoveDistance = 0.0f;
	float m_attackMoveSpeed = 0.08f;
	float m_maxDodgeMoveDistance = 3.0f;
	float m_currentDodgeMoveDistance = 0.0f;
	float m_DodgeMoveSpeed = 0.15f;
	bool  m_isInvincible = false;
	bool  m_isMovingDuringAttack = false;
	float m_JustDodgeWindowTimer = 0.0f;
	float m_DodgeStaminaCost = 25.0f;
	float m_RunStaminaCostPerSec = 15.0f;
	bool  m_CanComboCancel = false;
	bool  m_CanDodgeCancel = false;

	// --- 6. ロックオン & 方向フラグ ---
	bool  m_isLockOn = false;
	std::weak_ptr<EnemyBase> m_wpLockTarget;
	float m_oldRStickX = 0.0f;
	bool  m_isMovingForward = false;
	bool  m_isMovingBackward = false;
	bool  m_isMovingLeft = false;
	bool  m_isMovingRight = false;

	// --- 7. リソース弱参照 (Camera / Effect) ---
	std::weak_ptr<CameraBase> m_wpCamera;
	std::weak_ptr<KdEffekseerObject> m_wpBlookEffect;
	std::weak_ptr<KdEffekseerObject> m_wpSmokeEffect;
	std::weak_ptr<KdEffekseerObject> m_wpSpecialAttackEffect;
	// 武器実体 (旧構造維持)
	std::shared_ptr<WeaponBase> m_spKatana = nullptr;
	std::shared_ptr<WeaponBase> m_spSaya = nullptr;
};