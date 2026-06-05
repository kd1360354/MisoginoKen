#include "Fence.h"

// =========================================================================
// 静的メンバ変数の実体定義（これがないと未解決の外部参照エラーになります）
// =========================================================================
std::shared_ptr<KdModelData> Fence::s_spModel = nullptr;

void Fence::Init()
{
	// 自分の型名をセット
	SetName(MapParam::FenceParam::NAME);
	std::string path = "Asset/Prefabs/" + GetName() + ".json";
	std::ifstream ifs(path);
	if (ifs.is_open())
	{
		nlohmann::json j;
		ifs >> j;
		LoadParameters(j); // これで json 版が呼ばれる！
	}
	// モデルがまだロードされていない場合だけロードする（フライウェイトパターン）
	if (!s_spModel)
	{
		s_spModel = std::make_shared<KdModelData>();
		s_spModel->Load(m_ModelPath.empty() ? MapParam::FenceParam::MODEL_PATH : m_ModelPath);
	}

	// 各インスタンスのメンバ変数にも共有モデルをセット
	m_spModel = s_spModel;

	// 当たり判定の設定
	m_pCollider = std::make_unique<KdCollider>();
	// 共有モデルを使って判定を登録
	m_pCollider->RegisterCollisionShape(MapParam::FenceParam::NAME, m_spModel, KdCollider::Type::TypeGround);
}

void Fence::DrawLit()
{
	// 親クラス（MapBase）の描画処理を呼び出す
	MapBase::DrawLit();
}

void Fence::LoadParameters(const nlohmann::json& _j)
{
	KdGameObject::LoadParameters(_j);
}
