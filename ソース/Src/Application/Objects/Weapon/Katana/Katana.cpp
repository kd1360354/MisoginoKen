// =================================================================
// Katana.cpp (Player::GetComboCount() を利用するため修正)
// =================================================================
#include "Katana.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h" 
#include"../../../Constants/WeaponConstants/PlayerWeaponConstants/PlayerWeaponConstants.h"
void Katana::Init()
{
	m_spModel = std::make_shared<KdModelData>();
	// ★ KATANA_MODEL_PATH を使用
	m_spModel->Load(WeaponParam::KatanaParam::MODEL_PATH);
	WeaponBase::Init();
}

