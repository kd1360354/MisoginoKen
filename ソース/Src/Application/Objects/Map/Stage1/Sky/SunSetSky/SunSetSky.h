#pragma once
#include"../SkyBase.h"
class SunSetSky :public SkyBase
{
public:
		SunSetSky() {}
		~SunSetSky() override {}
		void Init() override;
		void Update() override;
		void DrawUnLit() override;
		const char* GetTypeName() const override { return "SunSetSky"; }
private:
	

};