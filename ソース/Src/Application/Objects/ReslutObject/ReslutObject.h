#pragma once
// KdGameObjectやKdTexture、Math::Vector2を使うため、必要なヘッダーを追記する想定
// #include <KdFramework.h> 

//
#include"../../Constants/SceneConstants/ResultConstants/ResultConstants.h"
// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class ResultObject :public KdGameObject
{
public:
	void Init() override;
	void DrawSprite() override;
	void Update() override;
	const char* GetTypeName()const override { return "ResultObject"; }
protected:
	std::shared_ptr<KdTexture> m_Tex[static_cast<int>(ResultParam::TexIndex::Max)];
	float m_blinkTimer = 0.0f;
	// 定数を使用
	const float m_blinkSpeed = BLINK_SPEED_DEFAULT;
	const float m_blinkCycle = BLINK_CYCLE_DEFAULT;
};