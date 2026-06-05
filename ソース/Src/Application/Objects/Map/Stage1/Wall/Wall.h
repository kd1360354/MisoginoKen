#pragma once
#include"../../MapBase.h"

// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Wall : public MapBase
{
public:
	Wall() {}
	~Wall() override {}

	const char* GetTypeName() const override { return MapParam::WallParam::NAME; }
	void Init() override;
	void DrawLit() override;

	// ★ 修正：m_drawTypeではなく、独自のフラグを操作する
	void SetInvisible(bool invisible) {
		m_isVisible = !invisible;
	}

	void LoadParameters(const nlohmann::json& _j) override;

	KdCollider* GetCollider() const { return m_pCollider.get(); }

private:
	// ★ 追加：個体ごとに描画するかどうかを決めるフラグ
	bool m_isVisible = true;
};