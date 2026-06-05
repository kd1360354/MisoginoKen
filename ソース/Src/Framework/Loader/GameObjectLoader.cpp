// GameObjectLoader.cpp
#include "GameObjectLoader.h"
#include <fstream>
#include <iostream>

#include "../../Application/Scene/BaseScene/BaseScene.h"
#include "../../Application/Scene/SceneManager.h" // 追加
#include "../GameObject/KdGameObjectFactory.h"
#include "../../Application/Objects/Character/Enemy/EnemyBase.h"
#include "../../Application/Objects/Character/Enemy/NoobEnemy/NoobEnemy.h"
#include "../../Application/Objects/Ui/EnemyUi/EnemyUi.h"
#include "../../Application/Objects/Character/Player/Player.h"
using json = nlohmann::json;

void GameObjectLoader::LoadJson(const std::string& filePath, BaseScene* scene)
{
	std::ifstream ifs(filePath);
	if (!ifs.is_open())
	{
		std::cerr << "Failed to open JSON file: " << filePath << std::endl;
		return;
	}

	json data;
	try
	{
		ifs >> data;
	}
	catch (const json::parse_error& e)
	{
		std::cerr << "JSON Parse Error: " << e.what() << std::endl;
		return;
	}

	if (!data.contains("objects") || !data["objects"].is_array())
	{
		std::cerr << "Invalid JSON format: 'objects' array not found" << std::endl;
		return;
	}

	for (const auto& objJson : data["objects"])
	{
		if (!objJson.is_object()) continue;
		CreateObjectFromJson(objJson, scene);
	}
}

void GameObjectLoader::CreateObjectFromJson(const json& objJson, BaseScene* scene)
{
	// ファクトリを使ってオブジェクト生成
	auto obj = KdGameObjectFactory::Instance().CreateGameObject(objJson.value("type", ""));
	if (!obj) return;

	// 名前
	std::string name = objJson.value("name", "");
	obj->SetName(name);

	// 位置
	if (objJson.contains("position") && objJson["position"].is_array() && objJson["position"].size() == 3)
	{
		obj->SetPos({
			objJson["position"][0].get<float>(),
			objJson["position"][1].get<float>(),
			objJson["position"][2].get<float>()
			});
	}

	// 回転
	if (objJson.contains("rotation") && objJson["rotation"].is_array() && objJson["rotation"].size() == 3)
	{
		obj->SetRot({
			objJson["rotation"][0].get<float>(),
			objJson["rotation"][1].get<float>(),
			objJson["rotation"][2].get<float>()
			});
	}

	// 拡縮
	if (objJson.contains("scale") && objJson["scale"].is_array() && objJson["scale"].size() == 3)
	{
		obj->SetScale({
			objJson["scale"][0].get<float>(),
			objJson["scale"][1].get<float>(),
			objJson["scale"][2].get<float>()
			});
	}
	// 3. ★ 敵 (EnemyBase) だった場合の特殊処理
	if (auto enemy = std::dynamic_pointer_cast<NoobEnemy>(obj))
	{
		// JSON から個別のパラメータをロード（Initの後に呼ぶことで上書き）
		// ※Init() は Factory 内部か、ここで呼ぶ設計に合わせる
		enemy->Init();

		// プレイヤーを取得 (UIのターゲット用)
		auto spPlayer = SceneManager::Instance().GetPlayer();

		// 敵専用 UI を生成
		auto enemyUi = std::make_shared<EnemyUi>();
		enemyUi->Init();
		enemyUi->SetEnemy(enemy);
		enemyUi->SetPlayer(spPlayer);
		enemy->SetEnemyUi(enemyUi);

		// SceneManager の管理リストに追加 (当たり判定やクリア判定用)
		SceneManager::Instance().AddNoobEnemy(enemy);

		// UI をシーンに追加
		scene->AddObject(enemyUi);
	}
	// シーンに追加
	scene->AddObject(obj);
}
