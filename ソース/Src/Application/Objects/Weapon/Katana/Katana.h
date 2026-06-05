#pragma once
#include"../WeaponBase.h"


// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Katana :public WeaponBase
{
public:
	Katana() {}
	~Katana() {}
	void Init()			override;
	
	const char* GetTypeName() const override { return "Katana"; }
private:
	
};