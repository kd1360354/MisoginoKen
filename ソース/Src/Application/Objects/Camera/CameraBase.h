#pragma once

class EnemyBase;

class CameraBase:public KdGameObject
{
public:
	CameraBase(){}
	~CameraBase()override{}

	void Init()override;
	void PreDraw()override;

	const char* GetTypeName() const override { return "CameraBase"; }

	void OnImGui()override;

	//追跡ターゲット設定
	void SetTarget(const std::shared_ptr<KdGameObject>& target);
	void SetTargetEnemy(const std::shared_ptr<EnemyBase>& target);

	//KdCameraのインスタンスをかえす
	const std::shared_ptr<KdCamera>& GetCamera()const { return m_spCamera; }

	//回転行列取得
	const Math::Matrix GetRotationMatrix()const;
	//回転行列取得(Y軸)
	const Math::Matrix GetRotationYMatrix()const;

	//TPSカメラのめり込み防止の為の当たり判定対象のオブジェクトの登録
	void RegistHitObject(const std::shared_ptr<KdGameObject>& object);

	void SnapToTarget();

	void SetLockOn(bool lockon) { m_isLockOn = lockon; }
	bool GetLockOn() { return m_isLockOn; }

	void SetAngle(const Math::Vector3& ang) { m_degAng = ang; }
	void SetAngleY(float y) { m_degAng.y = y; }

	const Math::Vector3& GetAngle() const { return m_degAng; }
protected:
	void ToggleLockOn();
	//マウスによる視点操作
	void UpdateRotateByMouse();

	void LookAtTarget();

	void UpdateZoomByMouse(float wheel_delta);
	//カメラ情報
	std::shared_ptr<KdCamera> m_spCamera = nullptr;

	//ターゲット情報
	std::weak_ptr<KdGameObject> m_wpTarget;
	std::weak_ptr<EnemyBase> m_wpLockTarget;

	//当たり判定オブジェクト情報
	std::vector< std::weak_ptr<KdGameObject>> m_wpHitObjList;

	//カメラ座標の中間行列
	Math::Matrix m_mLocalPos = Math::Matrix::Identity;

	//カメラ回転の中間行列
	Math::Matrix m_mRotation = Math::Matrix::Identity;

	//マウス操作用
	Math::Vector3 m_degAng = Math::Vector3::Zero;
	POINT m_fixMousePos{};

	// private: メンバ変数セクションに追加
	bool m_isTKeyDownLastFrame = false;

	bool m_isLockOn = false;

	// ロックオン時のY軸角度の補間速度
	float m_lockOnYawSpeed = 0.1f;

	float m_currentDistance = 4.0f;     // 現在のターゲットからの奥行き距離
	const float MIN_DISTANCE = 1.0f;    // ズームイン限界 (最小距離)
	const float MAX_DISTANCE = 8.0f;     // ズームアウト限界 (最大距離)
	const float ZOOM_SPEED = 0.5f;      // ズーム速度の係数
	// LookAt 用の注視点
	Math::Vector3 m_mLookAtPos = Math::Vector3::Zero;
};