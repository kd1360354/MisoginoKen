#include "Honden.h"

void Honden::Init()
{
	SetName(MapParam::HondenParam::NAME);
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
		m_spModel->Load(m_ModelPath.empty() ? MapParam::HondenParam::MODEL_PATH : m_ModelPath);
		if (m_pos == Math::Vector3::Zero) {
			SetPos(MapParam::HondenParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
		}
		else {
			SetPos(m_pos); // JSON から読み込んだ座標をセット
		}
		if (m_scale == Math::Vector3::Zero) {
			SetScale(MapParam::HondenParam::INITIAL_SCALE);
		}
		else
		{
			SetScale(m_scale);
		}
		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		m_pCollider->RegisterCollisionShape("Honden", m_spModel, KdCollider::Type::TypeGround);
	}
}

void Honden::DrawLit()
{
	MapBase::DrawLit();
}

void Honden::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
