#pragma once
#include"../../../../Constants/MapConstants/SkyConstants.h"
class SkyBase : public KdGameObject
{
public:

	// =========================================================================
	// II. ⚔️ クラス定義
	// =========================================================================
	SkyBase() {}
	~SkyBase() override {}
	void Init() override;
	void Update() override;
	void DrawUnLit() override;
protected:
	std::shared_ptr<KdModelData> m_spModel = nullptr;
	float m_Angle = 0;
	// ★ SKY_DEFAULT_SCALE を使用
	float m_Scale = SkyParam::Base::SCALE;
};