#include "Player.h"
#include "../../../Constants/GameConstants.h"
#include "../../../Constants/CharactorConstants/Player/PlayerConstants.h"
#include "../../../../Application/main.h"
#include "../../Camera/TPSCamera/TPSCamera.h"
#include "../Src/Application/Scene/SceneManager.h"
#include "../../Weapon/Katana/Katana.h"
#include "../../Weapon/Saya/Saya.h"
#include "../../Weapon/WeaponBase.h"
#include "../Enemy/EnemyBase.h"
#include "../../../Scene/ResultScene/ResultScene.h"
#include "../../../Scene/BaseScene/BaseScene.h"
#include "../../../Scene/GameScene/GameScene.h"

// ステートクラスのインクルード：各アクションの振る舞いを定義
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Idle/PlayerState_Idle.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Hit/PlayerState_Hit.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Death/PlayerState_Death.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Jump/PlayerState_Jump.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Dodge/PlayerState_Dodge.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_PowerAttack/PlayerState_PowerAttack.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Block/PlayerState_Block.h"
#include "../../../../Framework/StateMachine/Player/PlayerState/PlayerState_Potion/PlayerState_Potion.h"

// =========================================================
// I. ライフサイクル：生成・更新・描画
// =========================================================

/**
 * @brief プレイヤーの初期化
 * 理由：モデル生成、武器の装備、JSONからのパラメータ反映を一括で行う。
 */
void Player::Init()
{
	SetName(PlayerParam::NAME);

	// --- パラメータ読み込み ---
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j);
	}

	// --- モデル生成 ---
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		std::string modelPath = m_ModelPath.empty() ? PlayerParam::MODEL_PATH : m_ModelPath;
		m_spModel->SetModelData(modelPath);
		m_spAnimator = std::make_shared<KdAnimator>();
	}

	// --- 座標・ステータス設定 ---
	SetPos(m_pos == Math::Vector3::Zero ? PlayerParam::INITIAL_POS : m_pos);
	m_Hp = (float)m_MaxPlayerHp;
	m_DisplayHp = m_Hp;
	m_PowerAttackCost = PlayerParam::PowerAttack::MAX_COST;
	SetAttackPower(PlayerParam::ATTACK_POWER_DEFAULT);
	m_Stamina = (float)m_MaxPlayerStamina;
	m_AudioVolume = PlayerParam::Visual::Sound::VOLUME_DEFAULT;
	m_drawType = eDrawTypeLit;
	m_HpRecoveryItemNum = 10;

	// --- 武器初期化 ---
	m_spKatana = std::make_shared<Katana>();
	m_spKatana->Init();
	m_spKatana->SetOwner(shared_from_this());
	m_spKatana->SetHand(Hand::Right);
	m_spKatana->SetOnHitCallback([this]() {this->AddPowerCost(10.0f);});
	m_spSaya = std::make_shared<Saya>();
	m_spSaya->Init();
	m_spSaya->SetOwner(shared_from_this());
	m_spSaya->SetHand(Hand::Left);

	// --- 衝突判定登録 ---
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(GetName(), m_spModel, KdCollider::Type::TypeDamage);

	// 初期ステート：待機
	ChangeState(std::make_shared<PlayerState_Idle>());
}

/**
 * @brief 毎フレームの主更新
 */
void Player::Update()
{
	CharacterBase::Update();

	const float deltaTime = DeltaTimer::Instance().GetDeltaTime();

	// 1. 中断判定 (ヒットストップ)
	if (ProcessHitStop(deltaTime)) return;

	// 2. ターゲット制御 (ロックオンの開始・維持・解除)
	UpdateLockOnSystems();

	// 3. 基礎パラメータ更新 (タイマー、スタミナ、HPバー)
	UpdateInternalStatuses(deltaTime);

	// 4. 死亡時処理
	if (m_Hp <= 0) {
		UpdateDeathSequence(deltaTime);
		return;
	}

	// 5. 入力解析
	Math::Vector3 inputDir;
	bool moveFlg, runFlg;
	AnalyzeInput(inputDir, moveFlg, runFlg);

	// 6. 状態遷移のチェック (割り込みアクション)
	if (CheckStateTransitions()) return;

	// 7. 現在のステート更新と共通フラグ設定
	UpdateCurrentState(deltaTime);

	// 8. 旋回（方向）制御
	UpdateRotation(inputDir);

	// 9. 物理・移動計算
	UpdatePhysics(inputDir, moveFlg);

	// 10. 最終的な行列確定と付随物の更新
	FinalizeTransform();
}

/**
 * @brief 更新後の座標補正
 */
void Player::PostUpdate()
{
	RayPostUpdate();
	SpherePushBack();
	KdGameObject::PostUpdate();
}

/**
 * @brief 通常描画：本体と武器のリムライト同期
 */
void Player::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.SetLimLightEnable(true);
		KdShaderManager::Instance().m_StandardShader.SetLimLight(PlayerParam::Visual::RIM_COLOR);
		KdShaderManager::Instance().m_StandardShader.SetLimLightIntensity(5.0f);
		if (m_spKatana) m_spKatana->DrawLit();
		if (m_spSaya)   m_spSaya->DrawLit();
	}
	KdShaderManager::Instance().m_StandardShader.SetLimLightEnable(true);
	KdShaderManager::Instance().m_StandardShader.SetLimLight(PlayerParam::Visual::RIM_COLOR);
	KdShaderManager::Instance().m_StandardShader.SetLimLightIntensity(2.0f);
	CharacterBase::DrawLit();
}

/**
 * @brief シャドウマップ用深度描画
 */
void Player::GenerateDepthMapFromLight()
{
	CharacterBase::GenerateDepthMapFromLight();
	if (m_spKatana) m_spKatana->GenerateDepthMapFromLight();
	if (m_spSaya)   m_spSaya->GenerateDepthMapFromLight();
}

// =========================================================
// II. 入力・物理・アクション詳細
// =========================================================

/**
 * @brief 入力解析：スティック入力をワールド座標系に変換
 */
void Player::UpdateInPut(const Math::Matrix& cameraRotYMat, Math::Vector3& outDir, bool& outMoveFlg, bool& outRunFlg)
{
	outDir = Math::Vector3::Zero;
	outMoveFlg = false;

	Math::Vector2 axisState = KdInputManager::Instance().GetAxisState(APP_AXIS_MOVE);
	outMoveFlg = (axisState.LengthSquared() > MOVE_DIRECTION_SQUARED_THRESHOLD);

	bool isW = (axisState.y > InputThreshold::STICK_NEUTRAL);
	bool isS = (axisState.y < -InputThreshold::STICK_NEUTRAL);
	bool isA = (axisState.x < -InputThreshold::STICK_NEUTRAL);
	bool isD = (axisState.x > InputThreshold::STICK_NEUTRAL);

	if (m_isLockOn)
	{
		m_isMovingForward = isW;
		m_isMovingBackward = isS;
		m_isMovingLeft = isA;
		m_isMovingRight = isD;

		Math::Matrix playerRotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle));
		if (outMoveFlg) {
			if (isW) outDir += Math::Vector3::TransformNormal(INPUT_DIR_FORWARD, playerRotYMat);
			if (isS) outDir += Math::Vector3::TransformNormal(INPUT_DIR_BACK, playerRotYMat);
			if (isA) outDir += Math::Vector3::TransformNormal(INPUT_DIR_LEFT, playerRotYMat);
			if (isD) outDir += Math::Vector3::TransformNormal(INPUT_DIR_RIGHT, playerRotYMat);
		}
	}
	else
	{
		m_isMovingForward = outMoveFlg;
		if (outMoveFlg) {
			Math::Vector3 cameraForward = Math::Vector3::TransformNormal(INPUT_DIR_FORWARD, cameraRotYMat);
			Math::Vector3 cameraRight = Math::Vector3::TransformNormal(INPUT_DIR_RIGHT, cameraRotYMat);
			outDir = cameraForward * axisState.y + cameraRight * axisState.x;
		}
	}

	if (outDir.LengthSquared() > 0.0f) outDir.Normalize();

	if (outMoveFlg) {
		outRunFlg = true;
		m_speed = PlayerParam::Move::SPEED_UP;
	}
	else {
		outRunFlg = false;
		m_speed = 0.0f;
	}
}

/**
 * @brief キャラクターの旋回制御 (旧互換用)
 */
void Player::UpdateMovementAndPhysics(const Math::Vector3& inputDir, bool isMoving, bool isRunning)
{
	if (!isMoving) return;

	float targetAngleDeg = 0.0f;
	std::shared_ptr<EnemyBase> spTarget = m_wpLockTarget.lock();

	if (m_isLockOn && spTarget)
	{
		Math::Vector3 toTarget = spTarget->GetPos() - GetPos();
		toTarget.y = 0;
		if (toTarget.LengthSquared() > 0.01f) targetAngleDeg = DirectX::XMConvertToDegrees(atan2f(toTarget.x, toTarget.z));
	}
	else
	{
		targetAngleDeg = DirectX::XMConvertToDegrees(atan2f(inputDir.x, inputDir.z));
	}

	m_NowAngle = LerpAngle(m_NowAngle, targetAngleDeg, PlayerParam::Move::ROT_INTERPOLATION);
}

/**
 * @brief 武器の座標更新：手のノードに同期
 */
void Player::UpdateWeaponTransform()
{
	if (!m_spModel) return;

	auto pKatanaNode = m_spModel->FindNode("RightWeaponPoint");
	if (pKatanaNode && m_spKatana) {
		m_spKatana->SetWorldMatrix(pKatanaNode->m_worldTransform * m_mWorld);
		m_spKatana->Update();
	}

	auto pSayaNode = m_spModel->FindNode("LeftWeaponPoint");
	if (pSayaNode && m_spSaya) {
		m_spSaya->SetWorldMatrix(pSayaNode->m_worldTransform * m_mWorld);
		m_spSaya->Update();
	}
}

// =========================================================
// III. ユーティリティおよび補助システム
// =========================================================

/**
 * @brief 各種タイマーの更新
 */
void Player::UpdateTimers(float deltaTime)
{
	if (m_hitTimer > 0.0f) m_hitTimer -= deltaTime;
	if (m_StaminaRecoveryDelayTimer > 0.0f) m_StaminaRecoveryDelayTimer -= deltaTime;
	if (m_JustDodgeWindowTimer > 0.0f) m_JustDodgeWindowTimer = std::max(0.0f, m_JustDodgeWindowTimer - deltaTime);
}

/**
 * @brief エフェクト再生と向き同期
 */
void Player::PlayEffect(const std::string& name, const Math::Vector3& pos)
{
	auto wpEffect = KdEffekseerManager::GetInstance().Play(name, pos);
	if (auto spEffect = wpEffect.lock()) KdEffekseerManager::GetInstance().SetWorldMatrix(spEffect->GetHandle(), m_mWorld);
}

/**
 * @brief 必殺技などの範囲判定発生
 */
void Player::GenerateAreaAttack(float radius, float damage)
{
	Math::Vector3 world_attack_pos = m_pos + Math::Vector3{ 0.0f, 1.0f, 0.0f };
	if (m_spKatana) m_spKatana->CheckAttackPowerAttackHit(SceneManager::Instance().GetObjList(), world_attack_pos, radius, damage);
}

/**
 * @brief スタミナ自動回復
 */
void Player::RecoverStamina(float deltaTime)
{
	if (m_StaminaRecoveryDelayTimer <= 0.0f) m_Stamina = std::min(m_Stamina + m_StaminaRecoverRate * deltaTime, m_MaxPlayerStamina);
}

/**
 * @brief ロックオン開始処理
 */
void Player::UpdateLockOn()
{
	if (m_isLockOn) return;
	auto spCamera = std::dynamic_pointer_cast<TPSCamera>(m_wpCamera.lock());
	if (!spCamera) return;

	std::shared_ptr<EnemyBase> spNearestEnemy = FindNearestLockOnTarget();
	if (spNearestEnemy)
	{
		m_isLockOn = true;
		m_wpLockTarget = spNearestEnemy;
		spNearestEnemy->SetRimLightParams(PlayerParam::Visual::LOCKON_COLOR, 8.0f);
		spCamera->SetLockOn(true);
		spCamera->SetTargetEnemy(spNearestEnemy);
		spCamera->SnapToTarget();
	}
}

/**
 * @brief 最寄りのロックオン対象探索
 */
std::shared_ptr<EnemyBase> Player::FindNearestLockOnTarget()
{
	std::shared_ptr<EnemyBase> nearestEnemy = nullptr;
	float nearestDist = 15.0f;
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		auto spEnemy = std::dynamic_pointer_cast<EnemyBase>(obj);
		if (!spEnemy || spEnemy->GetHp() <= 0) continue;
		float dist = (spEnemy->GetPos() - m_pos).Length();
		if (dist < nearestDist) { nearestDist = dist; nearestEnemy = spEnemy; }
	}
	return nearestEnemy;
}

/**
 * @brief ゲージ加算と最大値クランプ
 */
void Player::GainPowerAttackCost(float amount)
{
	m_PowerAttackCost += amount;
	if (m_PowerAttackCost > PlayerParam::PowerAttack::MAX_COST) m_PowerAttackCost = (float)PlayerParam::PowerAttack::MAX_COST;
}

/**
 * @brief ImGuiデバッグ表示
 */
void Player::OnImGui()
{
	ImGui::SeparatorText("Player");
	KdGameObject::OnImGui();
	ImGui::Text("HP: %.1f / %.1f", m_Hp, (float)m_MaxPlayerHp);
	ImGui::Text("Stamina: %.1f", m_Stamina);
	if (m_pCurrentState) ImGui::Text("State: %s", typeid(*m_pCurrentState).name());

	static bool localShow = false;
	localShow = (m_pDebugWire != nullptr);
	if (ImGui::Checkbox(U8("この個体のデバッグ表示"), &localShow)) this->SetDebugDisplay(localShow);
}

/**
 * @brief JSONパラメータ読み込み詳細
 */
void Player::LoadParameters(const nlohmann::json& _j)
{
	CharacterBase::LoadParameters(_j);
	auto GetFloat = [&](const std::string& key, float def) {
		return (_j.contains(key) && _j[key].is_number()) ? (float)_j[key].get<double>() : def;
		};
	m_attackPower = GetFloat("attackPower", PlayerParam::ATTACK_POWER_DEFAULT);
	m_MaxPlayerHp = GetFloat("maxHp", 1260.0f);
	m_MaxPlayerStamina = GetFloat("maxStamina", 196.0f);
	m_speed = GetFloat("moveSpeed", 0.0f);
	m_JumpPower = GetFloat("_jumpPower", 0.8f);
	m_AttackStaminaCost = GetFloat("staminaCost_Attack", 10.0f);
	m_DodgeStaminaCost = GetFloat("staminaCost_Dodge", 25.0f);
	m_BlockHitStaminaCost = GetFloat("staminaCost_BlockHit", 15.0f);
	m_StaminaRecoverRate = GetFloat("staminaRecoverRate", 30.0f);
	m_DodgeMoveSpeed = GetFloat("dodgeMoveSpeed", 0.15f);
	m_maxDodgeMoveDistance = GetFloat("maxDodgeMoveDist", 3.0f);
	m_HpRecoveryItemNum = (int)GetFloat("potionCount", 10.0f);
	m_PotionHealRate = GetFloat("potionHealRate", 0.42f);
	m_AudioVolume = GetFloat("audioVolume_SE", 0.5f);
}

/**
 * @brief 被弾・死亡判定
 */
void Player::OnHit(float damage)
{
	if (m_Hp <= 0 || m_isInvincible) return;
	m_Hp -= damage;
	if (m_Hp < 0) m_Hp = 0;

	if (m_Hp <= 0) ChangeState(std::make_shared<PlayerState_Death>());
	else ChangeState(std::make_shared<PlayerState_Hit>());
}

/**
 * @brief オートターゲットおよび踏み込み移動
 */
void Player::AutoTargetClosestEnemy()
{
	std::shared_ptr<EnemyBase> closestEnemy = FindNearestEnemy();
	if (closestEnemy)
	{
		Math::Vector3 toTarget = closestEnemy->GetPos() - m_pos;
		m_NowAngle = DirectX::XMConvertToDegrees(atan2f(toTarget.x, toTarget.z));
	}
}

void Player::UpdateAttackStep(float normTime)
{
	if (normTime > 0.1f && normTime < 0.4f)
	{
		Math::Vector3 forward = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle)).Backward();
		auto spTarget = FindNearestEnemy();
		if (spTarget && Math::Vector3::Distance(m_pos, spTarget->GetPos()) < 1.5f) return;
		AddVelocity(forward * 0.1f);
	}
}

std::shared_ptr<EnemyBase> Player::FindNearestEnemy()
{
	std::shared_ptr<EnemyBase> nearestEnemy = nullptr;
	float minDistance = 15.0f;
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		auto spEnemy = std::dynamic_pointer_cast<EnemyBase>(obj);
		if (!spEnemy || spEnemy->GetHp() <= 0) continue;
		float dist = Math::Vector3::Distance(m_pos, spEnemy->GetPos());
		if (dist < minDistance) { minDistance = dist; nearestEnemy = spEnemy; }
	}
	return nearestEnemy;
}

// ---------------------------------------------------------
// その他シンプル関数及び補助関数
// ---------------------------------------------------------
void Player::UpdateDisplayHP(float deltaTime)
{
	if (std::abs(m_DisplayHp - m_Hp) > 0.01f) {
		float speed = (m_DisplayHp > m_Hp) ? HP_INTERPOLATION_SPEED_DAMAGE : HP_INTERPOLATION_SPEED_HEAL;
		m_DisplayHp += (m_Hp - m_DisplayHp) * speed * deltaTime;
	}
}

bool Player::ProcessHitStop(float deltaTime)
{
	if (m_HitStopTimer > 0.0f) { m_HitStopTimer -= deltaTime; return true; }
	return false;
}

void Player::ConsumePowerCost(float cost) { m_PowerAttackCost -= cost; if (m_PowerAttackCost < 0) m_PowerAttackCost = 0; }
void Player::AddPowerCost(float gain) { GainPowerAttackCost(gain); }
bool Player::ConsumeStamina(float cost) { if (m_Stamina >= cost) { m_Stamina -= cost; ResetStaminaRecoveryDelay(); return true; } return false; }
void Player::ResetStaminaRecoveryDelay() { m_StaminaRecoveryDelayTimer = m_StaminaRecoveryDelay; }
Math::Matrix Player::GetCameraWorldMatrix() const { return !m_wpCamera.expired() ? m_wpCamera.lock()->GetMatrix() : Math::Matrix::Identity; }
void Player::PlaySound(const std::string& path) { auto spSound = KdAudioManager::Instance().Play(path, false); if (spSound) spSound->SetVolume(m_AudioVolume); }
void Player::ExecuteCommonCancel(float normTime) {
	if (normTime >= PlayerParam::Attack::Cancel::DODGE_TO_ATK_START && normTime <= PlayerParam::Attack::Cancel::DODGE_TO_ATK_END && KdInputManager::Instance().IsPress(APP_BTN_DODGE)) ChangeState(std::make_shared<PlayerState_Dodge>());
	else if (normTime >= 0.5f && IsGround() && KdInputManager::Instance().IsPress(APP_BTN_JUMP)) ChangeState(std::make_shared<PlayerState_Jump>());
}

/**
 * @brief ロックオンシステムの総合管理
 * 意図：入力によるロックオンのON/OFF切り替えと、対象が死亡した際の自動解除を一括で行う。
 */
void Player::UpdateLockOnSystems()
{
	// 入力によるロックオン切り替え
	if (KdInputManager::Instance().IsPress(APP_BTN_LOCKON)) {
		if (m_isLockOn) {
			ReleaseLockOn();
		}
		else {
			UpdateLockOn();
		}
	}

	// ロックオン対象の生存チェック
	if (m_isLockOn) {
		auto spTarget = m_wpLockTarget.lock();
		if (!spTarget || spTarget->GetHp() <= 0) {
			ReleaseLockOn();
		}
	}
}
/**
 * @brief ロックオンの解除処理
 * 意図：カメラ、ターゲットのエフェクト（リムライト）、自身のフラグを安全に初期状態に戻す。
 */
void Player::ReleaseLockOn()
{
	if (auto spTarget = m_wpLockTarget.lock()) spTarget->SetRimLightParams(EnemyParam::Visual::RIM_COLOR, 2.0f);
	m_isLockOn = false;
	m_wpLockTarget.reset();
	if (auto spCamera = std::dynamic_pointer_cast<TPSCamera>(m_wpCamera.lock())) {
		spCamera->SetLockOn(false);
		spCamera->SetTargetEnemy(nullptr);
	}
}
/**
 * @brief 内部ステータス（時間・リソース）の更新
 * 意図：Updateのメインロジックを汚さないよう、タイマーやゲージ回復などの数値計算をまとめる。
 */
void Player::UpdateInternalStatuses(float deltaTime)
{
	UpdateTimers(deltaTime);
	RecoverStamina(deltaTime);
	UpdateDisplayHP(deltaTime);
}
/**
 * @brief 死亡時の更新シーケンス
 * 意図：HPが0の時に必要な最小限の処理（死亡アニメーションと落下）のみを行い、他のアクションを遮断する。
 */
void Player::UpdateDeathSequence(float deltaTime)
{
	if (m_pCurrentState) m_pCurrentState->Update(*this, deltaTime);
	m_Gravity += m_GravityPow;
	m_pos.y -= m_Gravity;

	FinalizeTransform(); // 行列更新と武器更新
}
/**
 * @brief 入力の解析と方向変換
 * 意図：カメラの向きやロックオン状態を考慮して、生のアナログスティック入力を「移動すべきベクトル」に加工する。
 */
void Player::AnalyzeInput(Math::Vector3& outDir, bool& outMoveFlg, bool& outRunFlg)
{
	Math::Matrix cameraRotYMat = !m_wpCamera.expired() ?
		m_wpCamera.lock()->GetRotationYMatrix() : Math::Matrix::Identity;
	UpdateInPut(cameraRotYMat, outDir, outMoveFlg, outRunFlg);
}
/**
 * @brief 状態遷移（割り込みアクション）の判定
 * 意図：移動中や待機中に、特定のボタン入力があった場合に別アクションへ遷移させる。
 * 戻り値：遷移が発生した場合は true を返し、以降の標準移動処理をキャンセルさせる。
 */
bool Player::CheckStateTransitions()
{
	if (!IsGround()) return false;

	// ポーション
	if (KdInputManager::Instance().IsPress(APP_BTN_POTION) && GetPotionCount() > 0) {
		std::string sName = typeid(*m_pCurrentState).name();
		if (sName.find("Idle") != std::string::npos || sName.find("Run") != std::string::npos) {
			ChangeState(std::make_shared<PlayerState_Potion>());
			return true;
		}
	}
	// ジャンプ
	if (KdInputManager::Instance().IsPress(APP_BTN_JUMP)) {
		ChangeState(std::make_shared<PlayerState_Jump>());
		return true;
	}
	// ガード
	if (KdInputManager::Instance().IsPress(APP_BTN_GUARD)) {
		ChangeState(std::make_shared<PlayerState_Block>());
		return true;
	}
	// 必殺技
	if (KdInputManager::Instance().IsPress(APP_BTN_POWER_ATK) && m_PowerAttackCost >= PlayerParam::PowerAttack::MAX_COST) {
		std::string sName = typeid(*m_pCurrentState).name();
		if (sName.find("Idle") != std::string::npos || sName.find("Run") != std::string::npos) {
			ChangeState(std::make_shared<PlayerState_PowerAttack>());
			return true;
		}
	}
	return false;
}
/**
 * @brief 現在のステート更新および共通パラメータの動的変更
 * 意図：ステート固有の振る舞いを実行しつつ、「無敵判定」などのステートを跨いで参照されるフラグを更新する。
 */
void Player::UpdateCurrentState(float deltaTime)
{
	if (m_pCurrentState) {
		m_pCurrentState->Update(*this, deltaTime);

		// 無敵フラグの更新
		std::string sName = typeid(*m_pCurrentState).name();
		m_isInvincible = (sName.find("PowerAttack") != std::string::npos ||
			sName.find("Dodge") != std::string::npos);
	}
}
/**
 * @brief キャラクターの向き（旋回）制御
 * 意図：移動方向、ロックオン対象、または攻撃対象の方向へスムーズに体を向ける。
 */
void Player::UpdateRotation(const Math::Vector3& inputDir)
{
	bool isAttacking = false;
	bool isDodge = false;
	if (m_pCurrentState) {
		std::string sName = typeid(*m_pCurrentState).name();
		isAttacking = (sName.find("Attack") != std::string::npos);
		isDodge = (sName.find("Dodge") != std::string::npos);
	}

	std::shared_ptr<EnemyBase> spTarget = (m_isLockOn) ? m_wpLockTarget.lock() : FindNearestEnemy();

	if ((m_isLockOn || isAttacking || isDodge) && spTarget && spTarget->GetHp() > 0) {
		Math::Vector3 toTarget = spTarget->GetPos() - m_pos;
		toTarget.y = 0;
		if (toTarget.LengthSquared() > 0.01f) {
			float targetAngle = DirectX::XMConvertToDegrees(atan2f(toTarget.x, toTarget.z));
			float rotSpeed = (isAttacking || isDodge) ? 0.8f : 0.2f;
			m_NowAngle = LerpAngle(m_NowAngle, targetAngle, rotSpeed);
		}
	}
}
/**
 * @brief 物理的な座標移動の計算
 * 意図：ステートによる移動制限、外部からの速度（ノックバック等）、重力を合成して最終的な座標を決定する。
 */
void Player::UpdatePhysics(const Math::Vector3& inputDir, bool moveFlg)
{
	// 1. 移動速度の決定（ステートによる制限）
	float currentMoveSpeed = m_speed;

	
	// 各ステートが持つ CanMove() に「今は動ける？」と問い合わせるだけの美しい処理
	if (m_pCurrentState && !m_pCurrentState->CanMove()) {
		currentMoveSpeed = 0.0f;
	}

	// 2. 水平移動の計算と適用
	if (moveFlg) {
		m_pos += inputDir * currentMoveSpeed;
	}

	// 3. 追加速度（踏み込みなど）の適用
	Math::Vector3 addedVelocity = m_vecVelocity;

	// 近接時の減衰ロジック
	auto spTarget = (m_isLockOn) ? m_wpLockTarget.lock() : FindNearestEnemy();
	if (spTarget) {
		float distSq = Math::Vector3::DistanceSquared(m_pos, spTarget->GetPos());
		if (distSq < 1.3f * 1.3f) addedVelocity *= 0.1f;
	}

	m_pos += addedVelocity;

	// 4. 重力の適用
	CharacterBase::ApplyGravity();

	// 5. 速度リセット
	m_vecVelocity = Math::Vector3::Zero;
}
/**
 * @brief 最終的な行列確定と付随物の更新
 * 意図：座標・回転をワールド行列に焼き付け、それに追従する武器などの位置を同期させる。
 */
void Player::FinalizeTransform()
{
	m_mWorld = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_NowAngle)) * Math::Matrix::CreateTranslation(m_pos);

	UpdateWeaponTransform();
	if (m_spKatana) m_spKatana->Update();
	if (m_spSaya)   m_spSaya->Update();

	CharacterBase::AnimeUpdate();
}