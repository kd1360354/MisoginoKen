#pragma once
#include"../../MapBase.h"


class GojunoTou : public MapBase
{
public:
	GojunoTou() {}
	~GojunoTou() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return "GojunoTou"; }
	void Init() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;
	void LoadParameters(const nlohmann::json& _j) override;

private:

};