#include"TitleScene.h"
#include"../GameScene/GameScene.h"
#include"../../Objects/Camera/TPSCamera/TPSCamera.h"
#include"../SceneManager.h"
#include"../../Objects/TitleObject/TitleText/TitleText.h"
#include"../SoundManager.h"
void TItleScene::Init()  
{  
	m_spTPSCamera = std::make_shared<TPSCamera>();  
	m_spTPSCamera->Init();  
	AddObject(m_spTPSCamera);  
	std::shared_ptr<TitleText> _TitleText = std::make_shared<TitleText>();  
	_TitleText->Init();  
	AddObject(_TitleText);  
	SetActiveCamera(m_spTPSCamera);  

	SoundManager::Instance().ChangeBGM(SoundManager::BgmState::Title);
	m_BGMPlayed = true;
}


void TItleScene::SceneUpdate()
{
	
	bool isAnyButtonPress =
		KdInputManager::Instance().IsPress(APP_BTN_ATTACK) ||  // 攻撃/決定ボタン (マウスLクリック / Aボタン)
		KdInputManager::Instance().IsPress(APP_BTN_GUARD) ||  // 攻撃/決定ボタン (マウスLクリック / Aボタン)
		KdInputManager::Instance().IsPress(APP_BTN_DODGE) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_POTION) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_POWER_ATK) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_LOCKON) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		//KdInputManager::Instance().IsPress(APP_BTN_TARGET_SW) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_MODE_CHANGE) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_CAMERA_RESET) ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_JUMP);       // ジャンプ/キャンセルボタン (スペース / Bボタン)

	if (isAnyButtonPress)
	{

		SceneManager::Instance().ChangeScene<GameScene>();
		SceneManager::Instance().SetNextScene(SceneManager::SceneMode::Game);
		SoundManager::Instance().StopAll();
	}

}

void TItleScene::Release()
{
}
