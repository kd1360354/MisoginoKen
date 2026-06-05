#pragma once
#include"../../MapBase.h"

// =========================================================================
// I. 📜 グローバル定数・マジックナンバー (Sakura 固有)
// =========================================================================


// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class BonFire : public MapBase
{
public:
	BonFire() {}
	~BonFire() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return MapParam::BonfireParam::NAME; }
	void Init() override;
	void Update() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;
	const PointLight& GetPointLight() const { return m_pointLight; }
	void LoadParameters(const nlohmann::json& _j) override;
private:
	PointLight m_pointLight;
	// ★ JSONから読み込んだ値を保持する変数（エディタ調整用）
	Math::Vector3 m_fireColor = { 1.0f, 0.6f, 0.1f };
	float         m_fireRadius = 10.0f;
};