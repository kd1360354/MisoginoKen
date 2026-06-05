#pragma once
#include"../WeaponBase.h"



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Saya :public WeaponBase
{
public:
	Saya() {}
	~Saya() {}
	void Init()			override;

	const char* GetTypeName()const override { return "Saya"; }
private:
};