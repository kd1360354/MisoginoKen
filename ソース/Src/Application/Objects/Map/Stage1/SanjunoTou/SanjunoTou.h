#pragma once
#include"../../MapBase.h"



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class SanjunoTou : public MapBase
{
public:
	SanjunoTou() {}
	~SanjunoTou() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return MapParam::SanjuParam::NAME; }
	void Init() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;
private:

};