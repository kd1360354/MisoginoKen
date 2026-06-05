#pragma once
#include"../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera() {}
	~TPSCamera()			override {}

	void Init()				override;

	void UpdateCameraControl();

	void Update()			override;

	void PostUpdate()		override;

	void FreeCameraControl();

	bool m_isMiddleButtonDownLastFrame = false;
	const char* GetTypeName()const override { return "TPSCamera"; }
	const Math::Vector2& GetMouseVelocity() const { return m_mouseVelocity; }
	void SetShake(float intensity, int frame) {
		m_shakeIntensity = intensity;
		m_shakeTime = frame;
	}
	void SetCursorReleased(bool b) { m_isCursorReleased = b; }
	void ResetMouseVelocity() { m_mouseVelocity = { 0,0 }; }
private:
	Math::Vector2 m_mouseVelocity = { 0.0f, 0.0f };
	bool m_isCursorReleased = false; // Altキーで切り替えるフラグ

	// --- カメラシェイク用 ---
	float         m_shakeIntensity = 0.0f; // 揺れの強さ
	int           m_shakeTime = 0;    // 残りフレーム数
	Math::Vector3 m_shakeOffset = Math::Vector3::Zero;

	//ImGui
	float m_freeCamSpeed = 0.5f; // エディター時の移動速度
	bool m_wasCreateMode = false;
};