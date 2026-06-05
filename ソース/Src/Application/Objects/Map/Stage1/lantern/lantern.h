#pragma once
#include"../../MapBase.h"


class Lantern : public MapBase
{
public:
	Lantern() {}
	~Lantern() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return "Lantern"; }
	void Init() override;
	void DrawUnLit() override;
	void SetLightActive(bool active);
	void Update() override;
	void Setparam(Math::Vector3 light_color_parm= { 1.0f, 0.8f, 0.6f }, float light_radius_parm = 10.0f,int light_is_bright_parm = 1);

	// 💡 PointLightのデータを取得する関数 (GameSceneで集約するために必要)
	const PointLight& GetPointLight() const { return m_pointLight; }
	virtual void SetPos(const Math::Vector3& pos);

	void LoadParameters(const nlohmann::json& _j) override;

	// JSON保存用に現在の設定値を返す関数
	const Math::Vector3& GetActiveColor() const { return m_activeColor; }
	float GetActiveRadius() const { return m_activeRadius; }
	int GetActiveIsBright() const { return m_activeIsBright; }
	void OnImGui() override;
private:
	// 💡 このランタンが持つ点光源データ
	PointLight m_pointLight;

	// 💡 点灯状態 (Updateで光を出す/出さないを判定するため)
	bool m_isLightActive = false;

	Math::Vector3 m_activeColor = MapParam::LanternParam::COLOR_ACTIVE;
	float m_activeRadius = MapParam::LanternParam::RADIUS_ACTIVE;
	int m_activeIsBright = MapParam::LanternParam::IS_BRIGHT_ACTIVE;
};