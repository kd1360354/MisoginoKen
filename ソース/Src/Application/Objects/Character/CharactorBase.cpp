#include "CharactorBase.h"
#include "../../Scene/SceneManager.h"
#include "../../Constants/GameConstants.h"

void CharacterBase::Init() {}

/**
 * @brief キャラクター共通の毎フレーム更新
 * 理由：ヒットストップ（手応え演出のための時間停止）のカウントダウンを管理し、
 * 停止中はアニメーションや物理移動を中断させる。
 */
void CharacterBase::Update()
{
	const float deltaTime = DeltaTimer::Instance().GetDeltaTime();

	// --- 💡 ヒットストップ（時間停止）の制御 ---
	if (m_HitStopTimer > 0.0f)
	{
		m_HitStopTimer -= deltaTime;

		// 視覚的な「止まっている感」を出すためにエフェクト速度を落とす
		KdEffekseerManager::GetInstance().SetSpeed(0.01f);

		if (m_HitStopTimer <= 0.0f)
		{
			m_HitStopTimer = 0.0f;
			KdEffekseerManager::GetInstance().SetSpeed(1.0f); // 復帰
		}

		// ヒットストップ中はこれ以降の処理（移動・重力・アニメ更新）を行わない
		return;
	}
}

/**
 * @brief モデル描画
 */
void CharacterBase::DrawLit()
{
	if (m_spModel) {
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

/**
 * @brief 影用深度情報の書き込み
 */
void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spModel) {
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}
/**
 * @brief 重力計算の適用（死亡時・落下用）
 */
void CharacterBase::ApplyGravity()
{
	m_Gravity += m_GravityPow;
	m_pos.y -= m_Gravity;
}

/**
 * @brief 地面との接地判定 (Ray判定)
 * 理由：足元から下向きにレイを飛ばし、地面との距離を測定する。
 * 段差(RAY_STEP_HEIGHT_LIMIT)内であれば自動的に座標を地面に吸い付かせ、
 * 接地フラグを更新する。
 */
void CharacterBase::RayPostUpdate()
{
	KdCollider::RayInfo _RayInfo;
	static const float _EnableStepHighb = RAY_STEP_HEIGHT_LIMIT; // 乗り越えられる段差の高さ

	_RayInfo.m_pos = m_pos;
	_RayInfo.m_pos.y = _RayInfo.m_pos.y - m_AdjustHeight + _EnableStepHighb;
	_RayInfo.m_dir = Math::Vector3::Down;
	// 現在の重力または最小射程のうち、長い方を判定距離にする
	_RayInfo.m_range = std::max(m_Gravity + _EnableStepHighb, RAY_MIN_RANGE);
	_RayInfo.m_type = KdCollider::Type::TypeGround;

	std::list<KdCollider::CollisionResult> l_reyRayList;
	for (auto& obj : SceneManager::Instance().GetObjList()) {
		obj->Intersects(_RayInfo, &l_reyRayList);
	}

	bool isHit = false;
	float maxOverLap = 0.0f;
	Math::Vector3 groundPos = Math::Vector3::Zero;

	// 最も高い（重なりが大きい）地面を採用する
	for (auto& ret : l_reyRayList) {
		if (maxOverLap < ret.m_overlapDistance) {
			maxOverLap = ret.m_overlapDistance;
			groundPos = ret.m_hitPos;
			isHit = true;
		}
	}

	if (isHit) {
		m_pos = groundPos;
		m_pos.y += m_AdjustHeight;
		m_Gravity = 0;
		m_isGround = true;
	}
	else {
		m_isGround = false;
	}
}

/**
 * @brief 壁や障害物との衝突押し戻し判定 (Sphere判定)
 * 理由：キャラクターを上下2つの球体（下半身・上半身）で包み、
 * ステージ上の壁や他のオブジェクトとの重なりを解消（押し戻し）して、
 * めり込みを防止する。
 */
void CharacterBase::SpherePushBack(Math::Vector3 LowSpherePos, float SphereRadius, Math::Vector3 upperSpherePos)
{
	std::list<KdCollider::CollisionResult> combinedCollisionResults;

	// --- 球体1: 下半身の判定球 ---
	KdCollider::SphereInfo lowerSphere;
	lowerSphere.m_sphere.Center = m_pos + LowSpherePos;
	lowerSphere.m_sphere.Radius = SphereRadius;
	lowerSphere.m_type = KdCollider::Type::TypeDamage | KdCollider::Type::TypeGround;

	// --- 球体2: 上半身の判定球 ---
	KdCollider::SphereInfo upperSphere;
	upperSphere.m_sphere.Center = m_pos + upperSpherePos;
	upperSphere.m_sphere.Radius = SphereRadius;
	upperSphere.m_type = KdCollider::Type::TypeGround;

	// 判定実行
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj.get() == this) continue;
		obj->Intersects(lowerSphere, &combinedCollisionResults);
		obj->Intersects(upperSphere, &combinedCollisionResults);
	}

	Math::Vector3 totalPushBack = Math::Vector3::Zero;
	float maxOverlap = 0.0f;

	// 衝突結果を合算して押し戻しベクトルを計算
	for (auto& ret : combinedCollisionResults)
	{
		totalPushBack += ret.m_hitDir * ret.m_overlapDistance;
		if (ret.m_overlapDistance > maxOverlap) {
			maxOverlap = ret.m_overlapDistance;
		}
	}

	// 押し戻し量が一定以上あれば適用する
	if (totalPushBack.LengthSquared() > PUSHBACK_MIN_SQUARED_LENGTH)
	{
		// Y軸の移動はRay判定に任せるため、水平方向のみに限定
		totalPushBack.y = 0;

		// 方向を確定させて、最大のめり込み量（maxOverlap）分だけ座標を戻す
		totalPushBack.Normalize();

		// ★ ここを修正しました（totalVelocity -> totalPushBack）
		m_pos += totalPushBack * maxOverlap;
	}
}

/**
 * @brief アニメーション時間の進行
 * 理由：速度(speed)に応じてアニメーションを進め、
 * モデルの各ボーン（ノード）の行列を再計算して最新のポーズにする。
 */
void CharacterBase::AnimeUpdate(float speed)
{
	if (!m_spAnimator) return;

	// モデルの各ノードに対しアニメーション時間を進める
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), speed);

	// アニメーション結果をモデル全体の行列へ反映
	m_spModel->CalcNodeMatrices();
}

void CharacterBase::AnimeUpdate() { AnimeUpdate(1.0f); }

/**
 * @brief JSONデータからのパラメータロード
 * 理由：最大HP、スタミナ、重力設定などを外部ファイルから一元管理するため。
 */
void CharacterBase::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);

	// HP読み込み（数値型であれば整数・小数問わずdoubleで受けてキャスト）
	if (_j.contains("maxHp")) {
		m_MaxPlayerHp = (float)_j["maxHp"].get<double>();
	}
	else {
		m_MaxPlayerHp = (float)PLAYER_MAX_HP_DEFAULT;
	}

	// スタミナ読み込み
	if (_j.contains("maxStamina") && _j["maxStamina"].is_number()) {
		m_MaxPlayerStamina = (float)_j["maxStamina"].get<double>();
	}
	else {
		m_MaxPlayerStamina = (float)PLAYER_MAX_STAMINA_DEFAULT;
	}
	m_Hp = m_MaxPlayerHp;
	m_Stamina = m_MaxPlayerStamina;

	// 重力設定
	if (_j.contains("gravity") && _j["gravity"].is_number()) {
		m_Gravity = (float)_j["gravity"].get<double>();
	}

	// リムライト設定（カラー情報の配列をVector3に変換）
	if (_j.contains("rimLightColor")) {
		auto r = _j["rimLightColor"];
		m_rimLightColor = { (float)r[0], (float)r[1], (float)r[2] };
	}
}