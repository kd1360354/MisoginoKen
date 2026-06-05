#include "KdGameObjectFactory.h"
#include "KdGameObject.h"

void KdGameObjectFactory::RegisterCreateFunction(const std::string_view name, const std::function<std::shared_ptr<KdGameObject>()>& func)
{
	m_createFunctions[name] = func;
}

std::shared_ptr<KdGameObject> KdGameObjectFactory::CreateGameObject(const std::string_view objName)
{
	auto it = m_createFunctions.find(objName);
	if (it != m_createFunctions.end())
	{
		return it->second();
	}
	return nullptr;
}

std::vector<std::string> KdGameObjectFactory::GetCreatableObjectNames() const
{
	std::vector<std::string> names;
	for (const auto& pair : m_createFunctions)
	{
		names.push_back(std::string(pair.first));
	}
	return names;
}
