#pragma once
#include"../BaseScene/BaseScene.h"

class TPSCamera;
class FPSCamera;

class TItleScene :public BaseScene
{
public:
	TItleScene() {}
	~TItleScene()override { Release(); }

	void Init()override;
	void SceneUpdate()override;
	void Release()override;

	//オブジェクトにユニークな名前を与える
	void GenerateUniqueName(const std::shared_ptr<KdGameObject>& obj);

private:
	//オブジェクトの種類ごとに次に使うIDを管理
	std::map<std::string, int> m_objNameCounter;

	std::shared_ptr<TPSCamera> m_spTPSCamera = nullptr;
	std::shared_ptr<FPSCamera> m_spFPSCamera = nullptr;
	bool m_BGMPlayed = false;
	std::shared_ptr<KdSoundInstance> _TitleBGM;
	float m_AudioVolume = 0.2f;
};