#include "Torii.h"

void Torii::Init()
{
	// ★ MAP_NAME_TORII を使用
	SetName(MapParam::ToriiParam::NAME);
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
		// ★ TORII_MODEL_PATH を使用
		m_spModel->Load(m_ModelPath.empty() ? MapParam::ToriiParam::MODEL_PATH : m_ModelPath);
		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		// ★ MAP_NAME_TORII を使用
		m_pCollider->RegisterCollisionShape(MapParam::ToriiParam::NAME, m_spModel, KdCollider::Type::TypeGround);
	}
}

void Torii::DrawLit()
{
	MapBase::DrawLit();
}

void Torii::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
