#include "lantern.h"

void Lantern::Init()
{
	SetName(MapParam::LanternParam::NAME);
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
		m_spModel->Load(m_ModelPath.empty() ? MapParam::LanternParam::MODEL_PATH : m_ModelPath);
		
		m_pCollider = std::make_unique<KdCollider>();
		// NOTE: m_mWorldはSetPos()が呼ばれた後に更新されるべきだが、Init時点ではスケーリングと初期トランスレーションで仮構築
		m_mWorld = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos);
		m_pCollider->RegisterCollisionShape(MapParam::LanternParam::NAME, m_spModel, KdCollider::Type::TypeGround);
	}

	// 💡 Init時に点光源データを作成し、メンバー変数に保持する
	Math::Vector3 lightPos = GetPos();
	// ★ LIGHT_Y_OFFSET を使用
	lightPos.y += MapParam::LanternParam::LIGHT_Y_OFFSET;

	// 初期状態: 消灯 (定数を使用)
	m_pointLight = PointLight(
		MapParam::LanternParam::COLOR_INACTIVE, // 色
		MapParam::LanternParam::RADIUS_INACTIVE,	  // 半径 
		lightPos,				  // 座標
		MapParam::LanternParam::IS_BRIGHT_INACTIVE // IsBright
	);
}

void Lantern::DrawUnLit()
{
	MapBase::DrawUnLit();
}

void Lantern::SetLightActive(bool active)
{
	m_isLightActive = active;
	if (active)
	{
		// ★ 点灯時の定数を使用
		m_pointLight.Color = m_activeColor;
		m_pointLight.Radius = m_activeRadius;
		m_pointLight.IsBright = m_activeIsBright;
	}
	else
	{
		// ★ 消灯時の定数を使用
		m_pointLight.Color = MapParam::LanternParam::COLOR_INACTIVE;
		m_pointLight.Radius = MapParam::LanternParam::RADIUS_INACTIVE;
		m_pointLight.IsBright = MapParam::LanternParam::IS_BRIGHT_INACTIVE;
	}
}

void Lantern::Update()
{
	// ランタンのUpdateロジック（もしあれば）
}

void Lantern::Setparam(Math::Vector3 light_color_parm, float light_radius_parm, int light_is_bright_parm)
{
	m_activeColor = light_color_parm;
	m_activeRadius = light_radius_parm;
	m_activeIsBright = light_is_bright_parm;
}

void Lantern::SetPos(const Math::Vector3& pos)
{
	// 1. 基底クラスの SetPos を呼び出し、m_pos と m_isDirty を更新し、m_mWorldを再計算する
	KdGameObject::SetPos(pos);

	// 2. 点光源の位置を更新
	Math::Vector3 newLightPos = pos;
	
	newLightPos.y += MapParam::LanternParam::LIGHT_Y_OFFSET;

	m_pointLight.Pos = newLightPos;
}

void Lantern::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
	// 2. 数値を安全に取得するためのラムダ
	auto GetFloat = [&](const std::string& key, float def) {
		if (_j.contains(key) && _j[key].is_number()) {
			return (float)_j[key].get<double>();
		}
		return def;
		};

	// 3. ランタン固有のライト設定を読込
	if (_j.contains("activeColor")) {
		auto& c = _j["activeColor"];
		m_activeColor = { (float)c[0].get<double>(), (float)c[1].get<double>(), (float)c[2].get<double>() };
	}
	m_activeRadius = GetFloat("activeRadius", 10.0f);
	m_activeIsBright = (int)GetFloat("activeIsBright", 1.0f);

	// 読み込んだ座標に合わせてライトの位置を更新
	Math::Vector3 lightPos = m_pos;
	lightPos.y += MapParam::LanternParam::LIGHT_Y_OFFSET;
	m_pointLight.Pos = lightPos;
}

void Lantern::OnImGui()
{
	KdGameObject::OnImGui();
	ImGui::SeparatorText("Lantern Settings");

	// 光の色をカラーピッカーで調整
	float col[3] = { m_activeColor.x, m_activeColor.y, m_activeColor.z };
	if (ImGui::ColorEdit3("Light Color", col)) {
		m_activeColor = { col[0], col[1], col[2] };
		SetLightActive(m_isLightActive); // 反映
	}

	// 半径をスライダーで調整
	if (ImGui::SliderFloat("Radius", &m_activeRadius, 0.0f, 50.0f)) {
		SetLightActive(m_isLightActive); // 反映
	}
}
