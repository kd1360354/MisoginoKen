#pragma once
#include"../../WeaponBase.h"



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class LeftWeapon :public WeaponBase
{
public:
	LeftWeapon() {}
	~LeftWeapon() {}
	void Init()			override;



	const char* GetTypeName() const override { return "LeftWeapon"; }
};