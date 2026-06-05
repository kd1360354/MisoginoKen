#pragma once
#include"../../MapBase.h"


// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Sakura : public MapBase
{
public:
	Sakura() {}
	~Sakura() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return MapParam::SakuraParam::NAME; }
	void Init() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;
	void LoadParameters(const nlohmann::json& _j) override;

private:

};