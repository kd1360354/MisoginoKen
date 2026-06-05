#include "TPSCamera.h"
#include"../../../main.h"
#include"../../Character/Enemy/EnemyBase.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"
constexpr float ZOOM_SPEED = 0.5f;
constexpr float MIN_DISTANCE = 2.0f;
constexpr float MAX_DISTANCE = 10.0f;
// マウス感度を下げてガタつきを軽減
const float MOUSE_SENSITIVITY = 0.10f; // 0.15f -> 0.10f に調整
void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 以前の注視点: Math::Matrix::CreateTranslation(-0.15f, 1.5f, -4.0f);
	// 修正: プレイヤーの真後ろ(X=0.0f)のオフセットにする
	m_mLocalPos = Math::Matrix::CreateTranslation(0.0f, 1.5f, -m_currentDistance); // m_currentDistanceはCameraBaseで4.0f

	SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);

	// 【重要】初期角度をリセットする
	// プレイヤーがZ=1方向を向いている場合、カメラの視線もZ=1方向を向くようにYaw角を0にする
	m_degAng.x = 0.0f; // Pitch (上下)
	m_degAng.y = 0.0f; // Yaw (左右)
	m_degAng.z = 0.0f; // Roll (傾き)

}

void TPSCamera::UpdateCameraControl()
{
	// ターゲットが有効でない場合は、マウス入力のみチェックして終了
	if (m_isLockOn)
	{
		// ロックオン中はカーソル固定は継続
		SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);
		return;
	}

	// =================================================================
	// 【✅ Altキーによるタップ式（トグル）制御の追加】
	// Altキーが押された瞬間に入力制御モードを切り替える
	if (KdInputManager::Instance().IsPress(APP_BTN_MODE_CHANGE))
	{
		m_isCursorReleased = !m_isCursorReleased; // フラグを反転
	}

	// Altキーによるモードで視点移動をロックする場合
	if (m_isCursorReleased)
	{
		// カーソルを解放し、視点移動処理を完全にスキップする

		// マウス慣性速度もリセット (Altキーを離した瞬間に動かないようにするため)
		m_mouseVelocity = { 0.0f, 0.0f };

		// 視点移動を停止するため、以降の処理をスキップ
		// ただし、ズームとリセットは実行する
	}
	// =================================================================


	// ゲームモード/クリエイトモードの時のみ入力を処理
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Game ||
		SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Title ||
		SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Result ||
		SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{

		// -----------------------------------------------------------------
		// 1. マウス入力の排他制御と処理
		// -----------------------------------------------------------------
		POINT nowPos;
		GetCursorPos(&nowPos);

		POINT mouseMove;
		mouseMove.x = nowPos.x - m_fixMousePos.x;
		mouseMove.y = nowPos.y - m_fixMousePos.y;

		// =================================================================
		// 【✅ カーソル固定の制御】
		if (!m_isCursorReleased) // Altキーがタップされていない場合
		{
			// カーソルを中央に固定
			SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);
		}
		// m_isCursorReleased の場合は SetCursorPos() を実行しないため、カーソルは自由に動く
		// =================================================================

		// =================================================================
		// 【✅ 視点移動のスキップ（Altキーでカーソルが解放されている場合）】
		if (m_isCursorReleased)
		{
			// カーソル解放モード中は、マウス/スティック入力による視点操作を一切行わない
			// (ここでは、慣性や角度更新に関わる mouseMove, m_mouseVelocity の計算をスキップする)
		}
		else // Altキーでカーソルが解放されていない場合（通常操作）
		// =================================================================
		{
			// 【💥 慣性・平滑化ロジックの開始】
			const float MOUSE_DEADZONE_PIXELS = 1.0f;
			const float FRICTION_RATE = 0.8f;
			const float VELOCITY_THRESHOLD = 0.001f;

			// Altキーでカーソル解放モードでなければ、mouseMove（移動量）が有効になる
			if (std::abs(mouseMove.x) >= MOUSE_DEADZONE_PIXELS || std::abs(mouseMove.y) >= MOUSE_DEADZONE_PIXELS)
			{
				// マウスがデッドゾーンを超えて動いている場合 (確実な入力)
				m_mouseVelocity.x = mouseMove.x * MOUSE_SENSITIVITY;
				m_mouseVelocity.y = mouseMove.y * MOUSE_SENSITIVITY;
			}
			else
			{
				// ... (マウスの動きが微小（ノイズ）な場合の慣性処理とゲームパッド入力処理)

				m_mouseVelocity *= FRICTION_RATE;

				if (m_mouseVelocity.LengthSquared() < VELOCITY_THRESHOLD * VELOCITY_THRESHOLD)
				{
					m_mouseVelocity = { 0.0f, 0.0f };
				}

				// -----------------------------------------------------------------
				// 2. マウス入力がない場合のみ、コントローラー軸入力を処理
				// -----------------------------------------------------------------
				if (m_mouseVelocity.LengthSquared() == 0.0f)
				{
					Math::Vector2 axisState = KdInputManager::Instance().GetAxisState(APP_AXIS_CAMERA);

					if (axisState.LengthSquared() > 0.0f)
					{
						const float STICK_SENSITIVITY = 2.5f;
						m_degAng.x += axisState.y * STICK_SENSITIVITY * -1.0f;
						m_degAng.y += axisState.x * STICK_SENSITIVITY;
					}
				}
			}
		}

		// -----------------------------------------------------------------
		// 3. 最終的な角度の更新
		// -----------------------------------------------------------------
		// Altキーでカーソルが解放されていると m_mouseVelocity はリセットされているため、
		// この処理で角度が動くことはない
		m_degAng.x += m_mouseVelocity.y;
		m_degAng.y += m_mouseVelocity.x;

		// 回転角度の制限
		m_degAng.x = std::clamp(m_degAng.x, -20.0f, 40.0f);
	}

	// 4. ズームボタンが押されたらリセット
	if (KdInputManager::Instance().IsHold(APP_BTN_CAMERA_RESET))
	{
		m_currentDistance = 4.0f;
	}

	// 5. マウスホイールによるズーム処理
	float wheel_delta = Application::Instance().GetMouseWheelValue();
	UpdateZoomByMouse(wheel_delta);
}

void TPSCamera::Update()
{
	// --- 💡 カメラシェイクの計算 ---
	if (m_shakeTime > 0)
	{
		m_shakeTime--;

		// KdRandom を使用して、-強さ ～ +強さ の範囲でランダムなズレを作る
		m_shakeOffset.x = KdRandom::GetFloat(-m_shakeIntensity, m_shakeIntensity);
		m_shakeOffset.y = KdRandom::GetFloat(-m_shakeIntensity, m_shakeIntensity);
		m_shakeOffset.z = KdRandom::GetFloat(-m_shakeIntensity, m_shakeIntensity);
	}
	else
	{
		m_shakeOffset = Math::Vector3::Zero;
	}
}

// TPSCamera.cpp

void TPSCamera::PostUpdate()
{

	// =================================================================
	// 1. モード分岐：エディター（Create）モードならフリーカメラのみ
	// =================================================================
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{
		FreeCameraControl();
		m_wasCreateMode = true; // ★ここ：エディター中は常にこれを true にしておく
		return;
	}	// =================================================================
	// 2. モード復帰時の即時リセット
	// =================================================================
	if (m_wasCreateMode)
	{
		// ターゲットを「Player型」としてロックし直す
		auto spPlayer = std::dynamic_pointer_cast<Player>(m_wpTarget.lock());

		if (spPlayer)
		{
			// Playerクラスが持っている GetRotateAngle() を呼び出す
			m_degAng.y = spPlayer->GetRotateAngle();
			m_degAng.x = 20.0f; // 角度もリセット（お好みで）
			m_currentDistance = 4.0f; // 距離もリセット
		}

		m_wasCreateMode = false;
	}
	// 1. 入力処理（ロックオン中は UpdateCameraControl 側で入力を弾くようにしてください）
	UpdateCameraControl();

	Math::Matrix _targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock(); // プレイヤー
	std::shared_ptr<EnemyBase> spLockTarget = m_wpLockTarget.lock(); // ロックオン対象

	// ロックオン強制解除チェック（死亡時など）
	if (m_isLockOn && (!spLockTarget || spLockTarget->GetHp() <= 0 || !_spTarget))
	{
		m_isLockOn = false;
		m_wpLockTarget.reset();
	}

	// プレイヤーがいない場合は処理不能
	if (_spTarget)
	{
		// 最新のプレイヤー座標で行列を作成
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}
	else { return; }

	// 2. 基本姿勢の計算（まず理想的なローカル位置と回転を出す）
	m_mLocalPos = Math::Matrix::CreateTranslation(0.0f, 1.5f, -m_currentDistance);
	m_mRotation = GetRotationMatrix();

	// ★ ロックオン追尾：プレイヤーの移動に連動して角度を再計算する
	if (m_isLockOn && spLockTarget)
	{
		Math::Vector3 P = _spTarget->GetPos();
		Math::Vector3 T = spLockTarget->GetPos();
		
		// プレイヤーから見た敵の方向（相対ベクトル）を毎フレーム計算
		Math::Vector3 toEnemy = T - P;
		toEnemy.y = 0.0f; // 仰角(X回転)は手動や別制御に任せるため、Yaw(Y回転)のみ

		if (toEnemy.LengthSquared() > 0.01f)
		{
			// 相対ベクトルから理想のY角度を算出
			float idealAngleDeg = DirectX::XMConvertToDegrees(atan2f(toEnemy.x, toEnemy.z));
			
			// 現在の m_degAng.y との差分を計算（最短距離で回る補正付き）
			float diff = idealAngleDeg - m_degAng.y;
			while (diff > 180.0f)  diff -= 360.0f;
			while (diff < -180.0f) diff += 360.0f;

			// 0.2f 程度の補間をかけることで、プレイヤーの急な移動による「ががが」を吸収
			m_degAng.y += diff * 0.2f; 
		}
		// 角度を更新したので、回転行列を再生成して同期
		m_mRotation = GetRotationMatrix();
	}

	// 壁めり込み判定前の「理想的なワールド座標」を算出
	Math::Matrix m_mIdealWorld = m_mLocalPos * m_mRotation * _targetMat;
	Math::Vector3 finalCameraPos = m_mIdealWorld.Translation();

	// 3. 壁めり込み防止判定 (レイ判定)
	KdCollider::RayInfo rayInfoWall;
	rayInfoWall.m_pos = finalCameraPos; // 計算した理想位置から
	Math::Vector3 rayTarget = _spTarget->GetPos() + Math::Vector3{ 0, 1.1f, 0 }; // プレイヤーの胸元へ飛ばす
	rayInfoWall.m_dir = rayTarget - finalCameraPos;
	rayInfoWall.m_range = rayInfoWall.m_dir.Length();
	rayInfoWall.m_dir.Normalize();
	rayInfoWall.m_type = KdCollider::TypeGround; // 地面属性のみを対象にする

	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (!spGameObj) continue;

		// ★ 超重要：レイ判定が「プレイヤー自身」や「敵」に当たってカメラがワープするのを防ぐ
		if (spGameObj == _spTarget) continue;
		if (std::dynamic_pointer_cast<EnemyBase>(spGameObj)) continue;

		std::list<KdCollider::CollisionResult> retRayList;
		spGameObj->Intersects(rayInfoWall, &retRayList);

		float maxOverLap = 0;
		Math::Vector3 hitPos = {};
		bool hit = false;
		for (auto& ret : retRayList)
		{
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hitPos = ret.m_hitPos;
				hit = true;
			}
		}
		if (hit)
		{
			// 障害物（壁や床）があったら、その衝突点より少し手前に押し戻す
			finalCameraPos = hitPos + rayInfoWall.m_dir * 0.4f;
			break;
		}
	}

	// 4. 最終行列の確定
	// シェイクなどの演出用のズレを最終座標に加算
	Math::Vector3 finalPosWithShake = finalCameraPos + m_shakeOffset;

	m_mWorld = m_mIdealWorld;
	m_mWorld.Translation(finalPosWithShake);

	// 5. 地面めり込み防止 (独立レイ判定による高さ補正)
	KdCollider::RayInfo groundRayInfo;
	// GetPos() ではなく、今計算したばかりの最新座標を使う
	Math::Vector3 currentCamPos = finalPosWithShake;
	const float RAY_START_HEIGHT_OFFSET = 3.0f;

	groundRayInfo.m_pos = currentCamPos + Math::Vector3(0, RAY_START_HEIGHT_OFFSET, 0);
	groundRayInfo.m_dir = Math::Vector3::Down;
	groundRayInfo.m_range = RAY_START_HEIGHT_OFFSET + 2.0f;
	groundRayInfo.m_type = KdCollider::TypeGround;

	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (!spGameObj) continue;
		
		// ここでも自分と敵は無視
		if (spGameObj == _spTarget || std::dynamic_pointer_cast<EnemyBase>(spGameObj)) continue;

		std::list<KdCollider::CollisionResult> retRayList;
		spGameObj->Intersects(groundRayInfo, &retRayList);

		float minHitRange = FLT_MAX;
		Math::Vector3 groundHitPos = {};
		bool hit = false;
		for (auto& ret : retRayList)
		{
			if (ret.m_range < minHitRange)
			{
				minHitRange = ret.m_range;
				groundHitPos = ret.m_hitPos;
				hit = true;
			}
		}
		if (hit)
		{
			const float CAMERA_MIN_Y_OFFSET = 1.0f;
			float minAllowableY = groundHitPos.y + CAMERA_MIN_Y_OFFSET;
			if (currentCamPos.y < minAllowableY)
			{
				// 地面より下にめり込まないように最新のワールド行列を書き換える
				Math::Vector3 safePos = finalPosWithShake;
				safePos.y = minAllowableY;
				m_mWorld.Translation(safePos);
			}
			break;
		}
	}
}

void TPSCamera::FreeCameraControl()
{
	// ImGui操作中は入力を受け付けない
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{
		// 1. 回転はマウスの右クリックを押している間だけ有効にする
		if (KdInputManager::Instance().IsHold(APP_BTN_RCLICK))
		{
			// ★ここ：右クリック中のみカーソルを表示させない、または固定する
			// ShowCursor(FALSE); // 必要なら追加

			POINT nowPos;
			GetCursorPos(&nowPos);
			float dx = (float)(nowPos.x - m_fixMousePos.x) * MOUSE_SENSITIVITY;
			float dy = (float)(nowPos.y - m_fixMousePos.y) * MOUSE_SENSITIVITY;

			// 右クリックを押し続けている間だけ、中央に戻す
			SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);

			m_degAng.y += dx;
			m_degAng.x += dy;
		}
		else
		{
			// ★重要：右クリックを離しているときは何もしない（Windowsにカーソルを任せる）
			// これにより、メッセージループが停滞するのを防ぎます
		}

		// 2. 移動処理（ここは右クリックに関係なくキー入力があれば動く）
		Math::Matrix rot = GetRotationMatrix();
		Math::Vector3 moveVec = Math::Vector3::Zero;

		if (KdInputManager::Instance().IsHold(APP_BTN_MOVE_FRONT)) moveVec += rot.Backward();
		if (KdInputManager::Instance().IsHold(APP_BTN_MOVE_BACK))  moveVec += rot.Forward();
		if (KdInputManager::Instance().IsHold(APP_BTN_MOVE_LEFT))  moveVec += rot.Left();
		if (KdInputManager::Instance().IsHold(APP_BTN_MOVE_RIGHT)) moveVec += rot.Right();

		if (GetAsyncKeyState('E')) moveVec.y += 1.0f;
		if (GetAsyncKeyState('Q')) moveVec.y -= 1.0f;

		if (moveVec.LengthSquared() > 0.001f) // 微小な入力でも動くように
		{
			moveVec.Normalize();
			Math::Vector3 pos = m_mWorld.Translation();
			pos += moveVec * m_freeCamSpeed;
			m_mWorld.Translation(pos);
		}
	}
}
