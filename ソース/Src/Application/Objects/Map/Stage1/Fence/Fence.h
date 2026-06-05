#pragma once
#include"../../MapBase.h"

// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Fence : public MapBase
{
public:
	Fence() {}
	~Fence() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return MapParam::FenceParam::NAME; }
	void Init() override;
	void DrawLit() override;
	void LoadParameters(const nlohmann::json& _j) override;

private:
	// すべてのインスタンスで共有するモデルポインタ
	static std::shared_ptr<KdModelData> s_spModel;
};