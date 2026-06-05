#pragma once
#include"../SkyBase.h"


// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class MidNight :public SkyBase
{
public:
	MidNight() {}
	~MidNight() override {}
	void Init() override;
	void Update() override;
	void DrawUnLit() override;
	const char* GetTypeName() const override { return SkyParam::MidNightParam::NAME; }
	void LoadParameters(const nlohmann::json& _j) override;
private:

};