#include "main.h"
#include"../Application/Scene/SceneManager.h"
#include"../Framework/EditableObject/EditableObject.h"

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// エントリーポイント
// アプリケーションはこの関数から進行する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR , _In_ int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		CoUninitialize();
		
		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginUpdate()
{
	// 入力状況の更新
	KdInputManager::Instance().Update();

	// ImGuiフレーム開始
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostUpdate()
{
	// 3DSoundListnerの行列を更新
	KdAudioManager::Instance().SetListnerMatrix(KdShaderManager::Instance().GetCameraCB().mView.Invert());

	m_window.ResetMouseWheelVal();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginDraw(bool usePostProcess)
{
	//バックバッファクリア
	KdDirect3D::Instance().ClearBackBuffer();

	//ポストプロセスを使う場合は、そのためのテクスチャもクリア
	if (!usePostProcess) return;
	KdShaderManager::Instance().m_postProcessShader.Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostDraw()
{
	//クリエイトモードのときだけデバッグGUIのロジックを呼び出す
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{

		if (auto* scene = SceneManager::Instance().GetCurrentScene())
		{
			m_debugGui.GuiProcess(*scene);
		}

	}

	//ImGuiの描画コマンドを生成し、レンダリング
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	// BackBuffer -> 画面表示
	KdDirect3D::Instance().WorkSwapChain()->Present(0, 0);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション初期設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "3D GameProgramming", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
//	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
//		bFullScreen = true;
//	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;

	deviceDebugMode = true;


	// Direct3D初期化
	std::string errorMsg;
	if (KdDirect3D::Instance().Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		HRESULT hr;

		hr = KdDirect3D::Instance().SetFullscreenState(TRUE, 0);
		if (FAILED(hr))
		{
			MessageBoxA(m_window.GetWndHandle(), "フルスクリーン設定失敗", "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	//===================================================================
	// シェーダー初期化
	//===================================================================
	KdShaderManager::Instance().Init();

	//===================================================================
	// KdInputManagerセットアップ
	//===================================================================
// --- 1. キーボード + マウス の入力設定 ---

	// ZLボタンの閾値定義を関数内部に移動
	const BYTE TRIGGER_THRESHOLD = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
	// ★修正1: std::make_unique で作成 (生ポインタを使用しない)
	auto spKeyMouseCollector = std::make_unique<KdInputCollector>();

	// 1-1. 軸入力
	// ★修正箇所 (生ポインタ -> shared_ptr)
	spKeyMouseCollector->AddAxis(APP_AXIS_MOVE,
		std::make_shared<KdInputAxisForWindows>(
			std::initializer_list<int>{ 'W' },
			std::initializer_list<int>{ 'D' },
			std::initializer_list<int>{ 'S' },
			std::initializer_list<int>{ 'A' }
		)
	);
	spKeyMouseCollector->AddAxis(APP_AXIS_CAMERA,
		std::make_shared<KdInputAxisForWindowsMouse>()
	);
	spKeyMouseCollector->AddAxis(APP_AXIS_WHEEL,
		std::make_shared<KdInputAxisForWindowsWheel>()
	);

	// 1-2. ボタン 

	spKeyMouseCollector->AddButton(APP_BTN_ATTACK, std::make_shared<KdInputButtonForWindows>(VK_LBUTTON));
	spKeyMouseCollector->AddButton(APP_BTN_GUARD, std::make_shared<KdInputButtonForWindows>(VK_RBUTTON));
	spKeyMouseCollector->AddButton(APP_BTN_DODGE, std::make_shared<KdInputButtonForWindows>('C'));
	spKeyMouseCollector->AddButton(APP_BTN_JUMP, std::make_shared<KdInputButtonForWindows>(VK_SPACE));
	spKeyMouseCollector->AddButton(APP_BTN_POWER_ATK, std::make_shared<KdInputButtonForWindows>('Q'));
	spKeyMouseCollector->AddButton(APP_BTN_POTION, std::make_shared<KdInputButtonForWindows>('E'));
	spKeyMouseCollector->AddButton(APP_BTN_LOCKON, std::make_shared<KdInputButtonForWindows>('T'));
	spKeyMouseCollector->AddButton(APP_BTN_TARGET_SW, std::make_shared<KdInputButtonForWindows>('G'));
	spKeyMouseCollector->AddButton(APP_BTN_MODE_CHANGE, std::make_shared<KdInputButtonForWindows>(VK_F1));
	spKeyMouseCollector->AddButton(APP_BTN_CAMERA_RESET, std::make_shared<KdInputButtonForWindows>(VK_MBUTTON));
	spKeyMouseCollector->AddButton(APP_BTN_LALT, std::make_shared<KdInputButtonForWindows>(VK_MENU));
	spKeyMouseCollector->AddButton(APP_BTN_RALT, std::make_shared<KdInputButtonForWindows>(VK_MENU));
	spKeyMouseCollector->AddButton(APP_BTN_RCLICK, std::make_shared<KdInputButtonForWindows>(VK_RBUTTON));
	spKeyMouseCollector->AddButton(APP_BTN_MOVE_FRONT, std::make_shared<KdInputButtonForWindows>('W'));
	spKeyMouseCollector->AddButton(APP_BTN_MOVE_BACK, std::make_shared<KdInputButtonForWindows>('S'));
	spKeyMouseCollector->AddButton(APP_BTN_MOVE_LEFT, std::make_shared<KdInputButtonForWindows>('A'));
	spKeyMouseCollector->AddButton(APP_BTN_MOVE_RIGHT, std::make_shared<KdInputButtonForWindows>('D'));

	// ★修正2: unique_ptrの所有権を std::move を使って移譲
	KdInputManager::Instance().AddDevice("KeyMouse", std::move(spKeyMouseCollector));


	// --- 2. ゲームパッド (XInput) の入力設定 ---

	// ★修正3: std::make_unique で作成
	auto spGamePadCollector = std::make_unique<KdInputCollectorForGamePad>(0);

	// 2-1. 軸入力 
	// ★修正箇所 (生ポインタ -> shared_ptr)
	spGamePadCollector->AddAxis(APP_AXIS_MOVE,
		std::make_shared<KdInputAxisForGamePad>(KdInputAxisForGamePad::StickType::Left)
	);
	spGamePadCollector->AddAxis(APP_AXIS_CAMERA,
		std::make_shared<KdInputAxisForGamePad>(KdInputAxisForGamePad::StickType::Right)
	);

	// 2-2. ボタン入力 
	// ★修正箇所 (生ポインタ -> shared_ptr)
	spGamePadCollector->AddButton(APP_BTN_ATTACK, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_Y));
	spGamePadCollector->AddButton(APP_BTN_GUARD, std::make_shared<KdInputButtonForGamePad>(TRIGGER_THRESHOLD, true)); // ZL/LT
	spGamePadCollector->AddButton(APP_BTN_DODGE, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_A));
	spGamePadCollector->AddButton(APP_BTN_JUMP, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_B));
	spGamePadCollector->AddButton(APP_BTN_POTION, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_X));
	spGamePadCollector->AddButton(APP_BTN_LOCKON, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_RIGHT_THUMB));
	spGamePadCollector->AddButton(APP_BTN_TARGET_SW, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_LEFT_THUMB));
	spGamePadCollector->AddButton(APP_BTN_MODE_CHANGE, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_START));
	spGamePadCollector->AddButton(APP_BTN_LALT, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_START));
	spGamePadCollector->AddButton(APP_BTN_RALT, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_START));
	spGamePadCollector->AddButton(APP_BTN_POWER_ATK, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_LEFT_THUMB));
	spGamePadCollector->AddButton(APP_BTN_CAMERA_RESET, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_LEFT_SHOULDER));
	spGamePadCollector->AddButton(APP_BTN_RCLICK, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_RIGHT_THUMB));
	spGamePadCollector->AddButton(APP_BTN_MOVE_FRONT, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_DPAD_UP));
	spGamePadCollector->AddButton(APP_BTN_MOVE_BACK, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_DPAD_DOWN));
	spGamePadCollector->AddButton(APP_BTN_MOVE_LEFT, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_DPAD_LEFT));
	spGamePadCollector->AddButton(APP_BTN_MOVE_RIGHT, std::make_shared<KdInputButtonForGamePad>(XINPUT_GAMEPAD_DPAD_RIGHT));

	// ★修正4: unique_ptrの所有権を std::move を使って移譲
	KdInputManager::Instance().AddDevice("GamePad0", std::move(spGamePadCollector));
	//===================================================================
	// オーディオ初期化
	//===================================================================
	KdAudioManager::Instance().Init();

	//===================================================================
	// imgui初期化
	//===================================================================
	m_debugGui.GuiInit();

	//シーンマネージャー初期化
	SceneManager::Instance().Init();

	//デルタタイムの初期化
	DeltaTimer::Instance().Init();

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション実行
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Execute()
{
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);

	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (Application::Instance().Init(atoi(sizeData[0].c_str()), atoi(sizeData[1].c_str())) == false) {
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	m_fpsController.Init();

	// ループ
	while (1)
	{
		// 処理開始時間Get
		m_fpsController.UpdateStartTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
//			if (MessageBoxA(m_window.GetWndHandle(), "本当にゲームを終了しますか？",
//				"終了確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
			{
				End();
			}
		}

		//=========================================
		//
		// アプリケーション更新処理
		//
		//=========================================

		//デルタタイム

		DeltaTimer::Instance().Update();

		KdBeginUpdate();
		{
			SceneManager::Instance().PreUpdate();

			SceneManager::Instance().Update();

			SceneManager::Instance().PostUpdate();
		}
		KdPostUpdate();

		//=========================================
		//
		// アプリケーション描画処理
		//
		//=========================================

		KdBeginDraw();
		{
			SceneManager::Instance().PreDraw();

			SceneManager::Instance().Draw();

			SceneManager::Instance().PostDraw();

			SceneManager::Instance().DrawSprite();
		}
		KdPostDraw();

		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		m_fpsController.Update();
		//std::string titleBar = "ゲーム FPS=" + std::to_string(m_fpsController.m_nowfps);
		std::string titleBar = "祓ノ剣 FPS" + std::to_string(m_fpsController.m_nowfps);
		SetWindowTextA(m_window.GetWndHandle(), titleBar.c_str());
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}

// アプリケーション終了
void Application::Release()
{

	SceneManager::Instance().Release();

	m_debugGui.GuiRelease();

	KdInputManager::Instance().Release();

	KdShaderManager::Instance().Release();

	KdAudioManager::Instance().Release();

	KdDirect3D::Instance().Release();

	// ウィンドウ削除
	m_window.Release();
}
