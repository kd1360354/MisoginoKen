#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include <string_view>

class KdGameObject;

class KdGameObjectFactory
{
public:
	static KdGameObjectFactory& Instance()
	{
		static KdGameObjectFactory instance;
		return instance;
	}

	void RegisterCreateFunction(const std::string_view name, const std::function<std::shared_ptr<KdGameObject>()>& func);

	std::shared_ptr<KdGameObject> CreateGameObject(const std::string_view objName);
	std::vector<std::string> GetCreatableObjectNames() const;

private:
	KdGameObjectFactory() {}
	~KdGameObjectFactory() {}
	KdGameObjectFactory(const KdGameObjectFactory&) = delete;
	KdGameObjectFactory& operator=(const KdGameObjectFactory&) = delete;

	std::unordered_map<std::string_view, std::function<std::shared_ptr<KdGameObject>()>> m_createFunctions;
};
