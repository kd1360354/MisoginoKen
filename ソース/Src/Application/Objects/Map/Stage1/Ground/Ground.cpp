#include "Ground.h"

void Ground::Init()
{
	SetName(MapParam::GroundParam::NAME);
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
		m_spModel->Load(m_ModelPath.empty() ? MapParam::GroundParam::MODEL_PATH : m_ModelPath);
		if (m_pos == Math::Vector3::Zero) {
			SetPos(MapParam::GroundParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
		}
		else {
			SetPos(m_pos); // JSON から読み込んだ座標をセット
		}
		if (m_scale == Math::Vector3::Zero) {
			SetScale(MapParam::GroundParam::INITIAL_SCALE);
		}
		else
		{
			SetScale(m_scale);
		}
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape(MapParam::GroundParam::NAME, m_spModel, KdCollider::TypeGround | KdCollider::TypeBump);
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
	}
}

void Ground::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.SetUVTiling(MapParam::GroundParam::UV_TILING);
	MapBase::DrawLit();
}

void Ground::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}


