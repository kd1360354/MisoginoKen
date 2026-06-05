#pragma once
#include"../../MapBase.h"

class Honden : public MapBase
{
public:
	Honden() {}
	~Honden() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return "Honden"; }
	void Init() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;

private:

};