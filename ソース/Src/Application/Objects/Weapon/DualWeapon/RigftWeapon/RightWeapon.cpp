// =================================================================
// Katana.cpp (Player::GetComboCount() を利用するため修正)
// =================================================================
#include "RightWeapon.h"
#include"../../../../Constants/WeaponConstants/EnemyWeaponConstants/EnemyWeaponConstants.h"
#include"../../../../Constants/CharactorConstants/Enemy/MidBoss/MidBossConstants.h"
void RightWeapon::Init()
{
	m_spModel = std::make_shared<KdModelData>();
	// ★ KATANA_MODEL_PATH を使用
	m_pos = { MidBossParam::Weapon::DEFAULT_TRNS_X,
		  MidBossParam::Weapon::DEFAULT_TRNS_Y,  // -0.125f
		  MidBossParam::Weapon::DEFAULT_TRNS_Z };
	Math::Matrix rot = Math::Matrix::CreateRotationX(
		DirectX::XMConvertToRadians(MidBossParam::Weapon::DEFAULT_ROT_X));
	Math::Matrix Trns = Math::Matrix::CreateTranslation(m_pos);
	m_WeaponRot = rot * Trns; // m_mWorld ではなく m_mLocal に設定

	m_spModel->Load(WeaponParam::Enemy::RightParam::MODEL_PATH);
	WeaponBase::Init();
}
