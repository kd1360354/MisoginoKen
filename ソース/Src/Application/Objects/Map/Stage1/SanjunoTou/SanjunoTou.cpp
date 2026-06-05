#include "SanjunoTou.h"

void SanjunoTou::Init()
{
	// ★ SetName が抜けていたため追加
	SetName(MapParam::SanjuParam::NAME);
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
		// ★ SANJU_PAGODA_MODEL_PATH を使用
		m_spModel->Load(m_ModelPath.empty() ? MapParam::SanjuParam::MODEL_PATH : m_ModelPath);
		if (m_pos == Math::Vector3::Zero) {
			SetPos(MapParam::SanjuParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
		}
		else {
			SetPos(m_pos); // JSON から読み込んだ座標をセット
		}

		m_pCollider = std::make_unique<KdCollider>();
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		// ★ MAP_NAME_SANJU_PAGODA を使用
		m_pCollider->RegisterCollisionShape(MapParam::SanjuParam::NAME, m_spModel, KdCollider::Type::TypeGround);
	}
}

void SanjunoTou::DrawLit()
{
	MapBase::DrawLit();
}

void SanjunoTou::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
