#include"ResultScene.h"
#include"../../Scene/SceneManager.h"
#include"../../Scene/TitleScene/TitleScene.h"
#include"../../Objects/ReslutObject/ReslutObject.h"
#include"../../Objects/Camera/TPSCamera/TPSCamera.h"
#include"../SoundManager.h"
void ResultScene::Init()
{
	m_spTPSCamera = std::make_shared<TPSCamera>();
	m_spTPSCamera->Init();
	AddObject(m_spTPSCamera);
	std::shared_ptr  <ResultObject> _ResultObject = std::make_shared<ResultObject>();
	_ResultObject->Init();
	AddObject(_ResultObject);
	SetActiveCamera(m_spTPSCamera);
	SoundManager::Instance().StopAll();
	SoundManager::Instance().ChangeBGM(SoundManager::BgmState::Result);
}

void ResultScene::SceneUpdate()
{
	// 決定ボタンとして一般的な APP_BTN_ATTACK, APP_BTN_JUMP, または APP_BTN_START (仮) をチェック
	bool isAnyButtonPress =
		KdInputManager::Instance().IsPress(APP_BTN_ATTACK)		   ||  // 攻撃/決定ボタン (マウスLクリック / Aボタン)
		KdInputManager::Instance().IsPress(APP_BTN_GUARD)		   ||  // 攻撃/決定ボタン (マウスLクリック / Aボタン)
		KdInputManager::Instance().IsPress(APP_BTN_DODGE)		   ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_POTION)		   ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_POWER_ATK)	   ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_LOCKON)		   ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_MODE_CHANGE)    ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_CAMERA_RESET)   ||       // ジャンプ/キャンセルボタン (スペース / Bボタン)
		KdInputManager::Instance().IsPress(APP_BTN_JUMP)   ;       // ジャンプ/キャンセルボタン (スペース / Bボタン)

	if (isAnyButtonPress)
	{
		SceneManager::Instance().ChangeScene<TItleScene>();
		SceneManager::Instance().SetNextScene(SceneManager::SceneMode::Title);
		SoundManager::Instance().StopAll();
	}
}

void ResultScene::Release()
{
}