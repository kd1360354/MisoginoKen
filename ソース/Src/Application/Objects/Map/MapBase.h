#pragma once
class MapBase :public KdGameObject
{
public:

	MapBase() {}
	~MapBase() override {}
	void DrawLit() override;
	void GenerateDepthMapFromLight()override;
	void Init() override;


protected:

	std::shared_ptr<KdModelData> m_spModel = nullptr;


};