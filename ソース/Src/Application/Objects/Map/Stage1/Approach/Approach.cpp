#include "Approach.h"

void Approach::Init()
{
	SetName(MapParam::ApproachParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j); // これで json 版が呼ばれる！
	}
	m_uvTiling = MapParam::ApproachParam::UV_TILING; // 定数で定義したデフォルト値をセット
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		std::string path = m_ModelPath.empty() ? MapParam::ApproachParam::MODEL_PATH: m_ModelPath;
		
		m_spModel->Load(path);

		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		m_pCollider->RegisterCollisionShape("Approach", m_spModel, KdCollider::Type::TypeGround);
	}
}

void Approach::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.SetUVTiling(m_uvTiling);
	MapBase::DrawLit();
}

void Approach::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
	
	// 🏁 Approach固有：UVタイリング
	if (_j.contains("uvTiling")) {
		m_uvTiling = { _j["uvTiling"][0], _j["uvTiling"][1] };
	}
}

