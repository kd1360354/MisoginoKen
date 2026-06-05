// =================================================================
// Katana.cpp (Player::GetComboCount() を利用するため修正)
// =================================================================
#include "LeftWeapon.h"
#include"../../../../Constants/WeaponConstants/EnemyWeaponConstants/EnemyWeaponConstants.h"
#include"../../../../Constants/CharactorConstants/Enemy/MidBoss/MidBossConstants.h"
void LeftWeapon::Init()
{

	m_spModel = std::make_shared<KdModelData>();
	// ★ KATANA_MODEL_PATH を使用
	m_spModel->Load(WeaponParam::Enemy::LeftParam::MODEL_PATH);
	// 💡 修正点: m_mLocal にローカル回転を設定する
	m_pos = { MidBossParam::Weapon::DEFAULT_TRNS_X,
		  MidBossParam::Weapon::DEFAULT_TRNS_Y,  // -0.125f
		  MidBossParam::Weapon::DEFAULT_TRNS_Z };
	Math::Matrix rot = Math::Matrix::CreateRotationX(
		DirectX::XMConvertToRadians(MidBossParam::Weapon::DEFAULT_ROT_X));
	Math::Matrix Trns = Math::Matrix::CreateTranslation(m_pos);
	m_WeaponRot =  rot* Trns; // m_mWorld ではなく m_mLocal に設定
	WeaponBase::Init();
}
