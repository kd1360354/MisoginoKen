#include "Bonfire.h"

void BonFire::Init()
{
	SetName(MapParam::BonfireParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j); // これで json 版が呼ばれる！
	}
	// --- 1. モデルのロード ---
	m_spModel = std::make_shared<KdModelData>();
	// LoadParametersでパスを読み込んでいればそれを使う。空なら定数。
	m_spModel->Load(m_ModelPath.empty() ? MapParam::BonfireParam::MODEL_PATH : m_ModelPath);


	if (m_pos == Math::Vector3::Zero) {
		SetPos(MapParam::BonfireParam::INITIAL_POS); // JSON に座標データがない場合のみ初期位置へ
	}
	else {
		SetPos(m_pos); // JSON から読み込んだ座標をセット
	}

	// --- 2. トランスフォームの確定 ---
	// 既にLoadParametersで値が入っていればそれを使う。
	// もしスケールが0（初期状態）なら、定数から初期値を代入する。
	if (m_scale.x == 0 && m_scale.y == 0) {
		m_scale = MapParam::BonfireParam::INITIAL_SCALE;
	}
	// 座標も同様（必要に応じて。基本はLoadParametersで入るはず）

	m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);

	// --- 3. 当たり判定 ---
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(MapParam::BonfireParam::NAME, m_spModel, KdCollider::Type::TypeGround);

	// --- 4. ライト設定 ---
	// m_fireColor などがLoadParametersで上書きされていればそれ、無ければ初期値
	m_pointLight = PointLight(
		m_fireColor,
		m_fireRadius,
		m_pos + Math::Vector3(0.0f, 0.5f, 0.0f),
		1
	);
}

void BonFire::Update()
{


}


void BonFire::DrawLit()
{
	MapBase::DrawLit();
}

void BonFire::GenerateDepthMapFromLight()
{
	MapBase::GenerateDepthMapFromLight();
}

void BonFire::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
	// あとは自分固有の処理だけ書く（座標の読み込みは書かなくていい！）
	if (_j.contains("fireColor")) {
		m_fireColor = { _j["fireColor"][0], _j["fireColor"][1], _j["fireColor"][2] };
	}

	// 読み込んだ座標（m_pos）を使ってライトを更新
	m_pointLight.Pos = m_pos + Math::Vector3(0.0f, 0.5f, 0.0f);
}
