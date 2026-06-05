#include "CameraBase.h"
#include"../../Scene/SceneManager.h"
#include"../Character/Enemy/EnemyBase.h"
#include"../Character/Player/Player.h"
void CameraBase::Init()
{
	//実体化
	m_spCamera = std::make_shared<KdCamera>();

	//マウスカーソル中央固定
	m_fixMousePos = { 640,360 };
}

void CameraBase::PreDraw()
{
	if (!m_spCamera)return;

	m_spCamera->SetCameraMatrix(m_mWorld);
}

void CameraBase::OnImGui()
{
	KdGameObject::OnImGui();

	ImGui::Spacing();
	ImGui::Text("CameraParameters");
	ImGui::DragFloat3("Angle", &m_degAng.x, 0.1f);

	//ターゲット情報表示
	if (auto target = m_wpTarget.lock())
	{
		ImGui::Text("Target: %s (%p)", target->GetTypeName(), target.get());
	}
	else
	{
		ImGui::Text("Target: None");
	}
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (target)
	{
		m_wpTarget = target;
	}
}

void CameraBase::SetTargetEnemy(const std::shared_ptr<EnemyBase>& target)
{
	if (target)
	{
		m_wpLockTarget = target;
	}
}

const Math::Matrix CameraBase::GetRotationMatrix() const
{
	return Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degAng.y),
		DirectX::XMConvertToRadians(m_degAng.x),
		DirectX::XMConvertToRadians(m_degAng.z)
	);
}

const Math::Matrix CameraBase::GetRotationYMatrix() const
{
	return Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng.y));
}

void CameraBase::RegistHitObject(const std::shared_ptr<KdGameObject>& object)
{
	if (object)
	{
		m_wpHitObjList.push_back(object);
	}
}

void CameraBase::UpdateRotateByMouse()
{
	if (m_isLockOn)
	{
		// マウスカーソルの固定だけは継続する
		SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);
		return;
	}
	//ゲームモードの時のみ有効
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Game|| SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Title|| SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Result)
	{
		POINT nowPos;
		GetCursorPos(&nowPos);

		POINT mouseMove;
		mouseMove.x = nowPos.x - m_fixMousePos.x;
		mouseMove.y = nowPos.y - m_fixMousePos.y;

		SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);

		m_degAng.x += mouseMove.y * 0.15f;
		m_degAng.y += mouseMove.x * 0.15f;

		//回転角度の制限
		m_degAng.x = std::clamp(m_degAng.x, -20.0f, 40.0f);
	}
}

void CameraBase::LookAtTarget()
{
	std::shared_ptr<KdGameObject> spTarget = m_wpTarget.lock();
	if (!spTarget) return;
	
	// 1. ターゲットとカメラの水平方向の差分ベクトルを計算
	Math::Vector3 toTarget = spTarget->GetPos() - m_mWorld.Translation();
	toTarget.x = 0.0f; // 水平方向のみを考慮

	if (toTarget.LengthSquared() < 0.01f * 0.01f) return; // ターゲットが近すぎる場合は無視

	// 2. 差分ベクトルからY軸の回転角度（ヨー角）を計算
	// atan2f(X, Z) は、Z軸から反時計回りのラジアン角を返す
	float targetAngleRad = atan2f(toTarget.x, toTarget.z);
	float targetAngleDeg = DirectX::XMConvertToDegrees(targetAngleRad);

	// 3. カメラの角度（m_degAng.y）をターゲットの方向へ設定
	// これにより、次の Update() でカメラがこの角度に基づいて配置されます
	m_degAng.x = targetAngleDeg;

	// 4. (オプション) 縦の角度 (m_degAng.x, ピッチ角) もターゲットの高さに合わせる
	// これはやや複雑になるため、今回は水平方向のみとします。
	// m_degAng.x = ...
}

void CameraBase::UpdateZoomByMouse(float wheel_delta)
{
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Game|| SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{
		if (std::abs(wheel_delta)>0.01f)
		{
			if (wheel_delta < 0)
			{
				// ホイール手前へ回転 (負の値): ズームアウト (距離を増やす)
				m_currentDistance += ZOOM_SPEED;
			}
			else // wheel_delta > 0
			{
				// ホイール奥へ回転 (正の値): ズームイン (距離を減らす)
				m_currentDistance -= ZOOM_SPEED;
			}
		}
			m_currentDistance = std::clamp(m_currentDistance, MIN_DISTANCE, MAX_DISTANCE);
			if (KdInputManager::Instance().IsPress(APP_BTN_CAMERA_RESET))
			{
				std::shared_ptr<Player>_player = SceneManager::Instance().GetPlayer();
				if (_player) {
					m_currentDistance = 4.0f;
					// 垂直角度を中央 (0度) にリセット
					m_degAng.x =0.0f;
					m_degAng.y = _player->GetRotateAngle();
				}
			}
	}
}

void CameraBase::SnapToTarget()
{
	// プレイヤーのクラスを特定し、角度取得のゲッター（例：GetRotateAngle()）があることを想定

	std::shared_ptr<EnemyBase> spTarget = m_wpLockTarget.lock();
	std::shared_ptr<KdGameObject> spPlayer = m_wpTarget.lock();

	if (!spTarget || !spPlayer) return;

	// プレイヤーの位置 (P) と ターゲットの位置 (T)
	Math::Vector3 P = spPlayer->GetPos();
	Math::Vector3 T = spTarget->GetPos();

	// プレイヤーからターゲットへの中点 (M) を向く視線ベクトルを計算
	Math::Vector3 centerPos = (P + T) / 2.0f;

	// カメラ原点（プレイヤーのカメラ基準高さ）
	Math::Vector3 cameraOriginPos = P + Math::Vector3(0, 1.5f, 0); // プレイヤーのカメラ基準高さ

	// カメラ原点から中点への水平ベクトル (視線方向)
	Math::Vector3 toCenterHorizontal = centerPos - cameraOriginPos;	
	toCenterHorizontal.y = 0.0f;

	if (toCenterHorizontal.LengthSquared() < 0.01f * 0.01f) return;

	// 1. 水平角度（m_degAng.y）をスナップ (YAW角のみを更新)
	float targetAngleRad = atan2f(toCenterHorizontal.x, toCenterHorizontal.z);
	float targetAngleDeg = DirectX::XMConvertToDegrees(targetAngleRad);
	m_degAng.y = targetAngleDeg;

	m_degAng.x = 0.0f;	
	m_degAng.y = 0.0f;


}
void CameraBase::ToggleLockOn()
{
	// ターゲットが存在する場合のみ
	if (m_wpLockTarget.lock())
	{
		m_isLockOn = !m_isLockOn; // フラグを反転

		if (m_isLockOn)
		{
			// ロックオン開始時、カメラをターゲットの方向へ瞬時にスナップ（SnapToTargetは既存関数）
			SnapToTarget();
		}
	}
	else
	{
		m_isLockOn = false; // ターゲットがいなければ解除
	}
}


