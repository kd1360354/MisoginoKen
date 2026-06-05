// PrefabFactory.h
#pragma once
#include <memory>
#include <string>


class PrefabFactory {
public:
	std::shared_ptr<KdGameObject> LoadPrefab(const std::string& prefabName);
};
