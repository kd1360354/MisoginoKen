#pragma once
#include"../../MapBase.h"

class Approach : public MapBase
{
public:
	Approach() {}
	~Approach() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return "Approach"; }
	void Init() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;
	Math::Vector2 GetUVTile() const {return m_uvTiling; }
private:
	Math::Vector2 m_uvTiling = { 1.0f, 1.0f }; // デフォルト値
};