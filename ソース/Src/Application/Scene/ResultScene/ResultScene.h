#pragma once
#include"../BaseScene/BaseScene.h"
class TPSCamera;

class ResultScene :public BaseScene
{
public:
	ResultScene() {}
	~ResultScene()override { Release(); }

	void Init()override;
	void SceneUpdate()override;
	void Release()override;

	//オブジェクトにユニークな名前を与える
	void GenerateUniqueName(const std::shared_ptr<KdGameObject>& obj);

private:
	//オブジェクトの種類ごとに次に使うIDを管理
	std::map<std::string, int> m_objNameCounter;
	std::shared_ptr<TPSCamera> m_spTPSCamera = nullptr;
	std::shared_ptr<KdSoundInstance> _ResultBGM;
	float m_AudioVolume = 0.2f;
};