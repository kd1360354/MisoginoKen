#pragma once
#include"../SkyBase.h"
class Midnight_Stars :public SkyBase
{
public:
	Midnight_Stars() {}
	~Midnight_Stars() override {}
	void Init() override;
	void Update() override;
	void DrawUnLit() override;
	const char* GetTypeName() const override { return "Midnight_Stars"; }
private:


};