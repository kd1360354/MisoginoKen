#pragma once
#include"../../MapBase.h"

class Chozuya : public MapBase
{
public:
	Chozuya() {}
	~Chozuya() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return "Chozuya"; }
	void Init() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;
	void LoadParameters(const nlohmann::json& _j) override;

private:

};