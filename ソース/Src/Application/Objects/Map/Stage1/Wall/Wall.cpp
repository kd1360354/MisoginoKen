#include "Wall.h"

void Wall::Init()
{
	// ★ MAP_NAME_WALL を使用
	SetName(MapParam::WallParam::NAME);
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
		// ★ WALL_MODEL_PATH を使用
		m_spModel->Load(m_ModelPath.empty() ? MapParam::WallParam::MODEL_PATH : m_ModelPath);

		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		// ★ MAP_NAME_WALL を使用
		m_pCollider->RegisterCollisionShape(MapParam::WallParam::NAME, m_spModel, KdCollider::Type::TypeGround);
	}
}

void Wall::DrawLit()
{
	// NOTE: UVタイリングがコメントアウトされているため、定数化はヘッダーに記述のみとします。
	// KdShaderManager::Instance().m_StandardShader.SetUVTiling(WALL_UV_TILING); 
	if (!m_isVisible) return;
	MapBase::DrawLit();
}

void Wall::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
	
}
