#pragma once
#include"../../MapBase.h"


class Ground : public MapBase
{
public:
	Ground() {}
	~Ground() override{}
	//自分の型名を返す
	const char* GetTypeName()const override { return "Ground"; }
	void Init() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;

private:

};