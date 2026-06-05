#include "GojunoTou.h"

void GojunoTou::Init()
{
	SetName(MapParam::GojuParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j); // これで json 版が呼ばれる！
	}
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load(m_ModelPath.empty() ? MapParam::GojuParam::MODEL_PATH: m_ModelPath);
		if (m_pos == Math::Vector3::Zero) {
			SetPos(MapParam::GojuParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
		}
		else {
			SetPos(m_pos); // JSON から読み込んだ座標をセット
		}
		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		m_pCollider->RegisterCollisionShape("GojunoTou", m_spModel, KdCollider::Type::TypeGround);
	}
}

void GojunoTou::DrawLit()
{
	MapBase::DrawLit();
}

void GojunoTou::GenerateDepthMapFromLight()
{
	MapBase::GenerateDepthMapFromLight();
}

void GojunoTou::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
	
}
