#pragma once
class UiBase :public KdGameObject
{
public:
	// =========================================================================
	// I. 📜 グローバル定数・マジックナンバー (UiBase 固有)
	// =========================================================================

	// ★ リソース配列の最大サイズを定数化
	static constexpr int MAX_TEXTURES = 15;
	static constexpr int MAX_SQUARE_POLYGONS = 10;

	// =========================================================================
	// II. ⚔️ クラス定義
	// =========================================================================
	void Init() override;
	void DrawSprite() override;
	void Update() override;
protected:
	// ★ 定数を使用した配列サイズ定義
	std::shared_ptr<KdTexture> m_Tex[MAX_TEXTURES];
	std::shared_ptr<KdSquarePolygon> m_SquareTex[MAX_SQUARE_POLYGONS];
	float m_display_hp = 0.0f; // UI表示用のHP（初期化子リストで0.0f初期化推奨）
	float m_hp_rate = 1.0f; // 描画用のHP割合
	float m_staminaRate = 1.0f;
};