#pragma once
#include"../../MapBase.h"

// =========================================================================
// I. 📜 グローバル定数・マジックナンバー (Sakura 固有)
// =========================================================================



// =========================================================================
// II. ⚔️ クラス定義
// =========================================================================
class Gross : public MapBase
{
public:
	Gross() {}
	~Gross() override {}
	//自分の型名を返す
	const char* GetTypeName()const override { return MapParam::GrassParam::NAME; }
	void Init() override;
	void DrawLit() override;
	void GenerateDepthMapFromLight() override;
private:

};