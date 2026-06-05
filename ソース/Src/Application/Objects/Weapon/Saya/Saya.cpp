#include "Saya.h"
#include"../../../Constants/WeaponConstants/PlayerWeaponConstants/PlayerWeaponConstants.h"
void Saya::Init()
{
	m_spModel = std::make_shared<KdModelData>();
	// ★ SAYA_MODEL_PATH を使用
	m_spModel->Load(WeaponParam::SayaParam::MODEL_PATH);
}
