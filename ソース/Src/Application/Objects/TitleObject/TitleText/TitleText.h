#pragma once
#include"../../../Constants/SceneConstants/TitleConstants/TitleConstants.h"
// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class TitleText :public KdGameObject
{
public:
	void Init() override;
	void DrawSprite() override;
	void Update() override;
	const char* GetTypeName()const override { return "TitleText"; }
protected:
	std::shared_ptr<KdTexture> m_Tex[static_cast<int>(TitleParam::TexIndex::Max)];
	bool m_change_loading = false;
	float m_blink_timer = 0.0f;
	// 定数を使用
	const float m_blink_speed = BLINK_SPEED_DEFAULT;
	const float m_blink_cycle = BLINK_CYCLE_DEFAULT;
};