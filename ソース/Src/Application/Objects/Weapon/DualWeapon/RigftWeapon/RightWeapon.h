#pragma once
#include"../../WeaponBase.h"



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class RightWeapon :public WeaponBase
{
public:
	RightWeapon() {}
	~RightWeapon() {}
	void Init()			override;
	const char* GetTypeName() const override { return "RightWeapon"; }
};