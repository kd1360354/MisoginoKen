#pragma once
#include"../SkyBase.h"
class Dawn_Cloudy :public SkyBase
{
public:
	Dawn_Cloudy() {}
	~Dawn_Cloudy() override {}
	void Init() override;
	void Update() override;
	void DrawUnLit() override;
	const char* GetTypeName() const override { return "Dawn_Cloudy"; }
private:


};