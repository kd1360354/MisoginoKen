// GameObjectLoader.h
#pragma once
#include <string>
#include <memory>
#include <json.hpp>

class BaseScene; // 前方宣言

class GameObjectLoader
{
public:
	// JSONファイルを読み込んでオブジェクトをシーンに追加
	static void LoadJson(const std::string& filePath, BaseScene* scene);

private:
	// JSONから1つのオブジェクトを生成するヘルパー
	static void CreateObjectFromJson(const nlohmann::json& objJson, BaseScene* scene);
};
