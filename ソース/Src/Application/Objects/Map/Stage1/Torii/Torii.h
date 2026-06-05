#pragma once
#include"../../MapBase.h"



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Torii : public MapBase
{
public:
	Torii() {}
	~Torii() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return MapParam::ToriiParam::NAME; }
	void Init() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;
private:

};