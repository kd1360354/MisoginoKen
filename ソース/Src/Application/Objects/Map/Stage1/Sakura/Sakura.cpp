#include "Sakura.h"

void Sakura::Init()
{
	// ★ MAP_NAME_SAKURA を使用
	SetName(MapParam::SakuraParam::NAME);
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
		m_spModel->Load(m_ModelPath.empty() ? MapParam::SakuraParam::MODEL_PATH : m_ModelPath);
		if (m_pos == Math::Vector3::Zero) {
			SetPos(MapParam::SakuraParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
		}
		else {
			SetPos(m_pos); // JSON から読み込んだ座標をセット
		}
		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		// ★ MAP_NAME_SAKURA を使用
		m_pCollider->RegisterCollisionShape(MapParam::SakuraParam::NAME, m_spModel, KdCollider::Type::TypeGround);
	}
}


void Sakura::DrawLit()
{
	MapBase::DrawLit();
}

void Sakura::GenerateDepthMapFromLight()
{
	MapBase::GenerateDepthMapFromLight();
}

void Sakura::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
