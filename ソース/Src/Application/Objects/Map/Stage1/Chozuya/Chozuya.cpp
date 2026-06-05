#include "Chozuya.h"

void Chozuya::Init()
{
	SetName(MapParam::ChozuyaParam::NAME);
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
		m_spModel->Load(m_ModelPath.empty() ? MapParam::ChozuyaParam::MODEL_PATH : m_ModelPath);
		if (m_pos == Math::Vector3::Zero) {
			SetPos(MapParam::ChozuyaParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
		}
		else {
			SetPos(m_pos); // JSON から読み込んだ座標をセット
		}
		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld =Math::Matrix::CreateTranslation(m_pos);
		m_pCollider->RegisterCollisionShape("Chozuya", m_spModel, KdCollider::Type::TypeGround);
	}
}

void Chozuya::DrawLit()
{
	MapBase::DrawLit();
}

void Chozuya::GenerateDepthMapFromLight()
{
	MapBase::GenerateDepthMapFromLight();
}

void Chozuya::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
