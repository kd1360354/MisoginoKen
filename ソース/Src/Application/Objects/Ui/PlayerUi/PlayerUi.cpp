#include "PlayerUi.h"

#include"../../Character/Player/Player.h"

void PlayerUi::Init()

{

	m_Tex[0] = std::make_shared<KdTexture>();

	m_Tex[0]->Load("Asset/Textures/UI/HpBackGround.png");



	m_Tex[1] = std::make_shared<KdTexture>();

	m_Tex[1]->Load("Asset/Textures/UI/HpFrame.png");



	m_Tex[2] = std::make_shared<KdTexture>();

	m_Tex[2]->Load("Asset/Textures/UI/HPBar.png");



	m_Tex[3] = std::make_shared<KdTexture>();

	m_Tex[3]->Load("Asset/Textures/UI/StaminaFrame.png");



	m_Tex[4] = std::make_shared<KdTexture>();

	m_Tex[4]->Load("Asset/Textures/UI/StaminaBar.png");


	m_Tex[5] = std::make_shared<KdTexture>();

	m_Tex[5]->Load("Asset/Textures/UI/heart-bottle.png");



	m_Tex[6] = std::make_shared<KdTexture>();

	m_Tex[6]->Load("Asset/Textures/UI/EKey.png");



	m_Tex[7] = std::make_shared<KdTexture>();

	m_Tex[7]->Load("Asset/Textures/UI/QKey.png");


	m_Tex[8] = std::make_shared<KdTexture>();

	m_Tex[8]->Load("Asset/Textures/UI/CKey.png");



	m_Tex[9] = std::make_shared<KdTexture>();

	m_Tex[9]->Load("Asset/Textures/UI/Dodge.png");



	m_Tex[10] = std::make_shared<KdTexture>();

	m_Tex[10]->Load("Asset/Textures/UI/ougi_base.png");



	m_Tex[11] = std::make_shared<KdTexture>();

	m_Tex[11]->Load("Asset/Textures/UI/ougi_gauge.png");



	m_Tex[12] = std::make_shared<KdTexture>();

	m_Tex[12]->Load("Asset/Textures/UI/ougi_glow.png");



	m_Tex[13] = std::make_shared<KdTexture>();

	m_Tex[13]->Load("Asset/Textures/UI/ougi_Text.png");

	m_TexPad[0] = std::make_shared<KdTexture>();
	m_TexPad[0]->Load("Asset/Textures/UI/BButton.png"); // インデックス 0: 仮にAボタン (Jump)

	m_TexPad[1] = std::make_shared<KdTexture>();
	m_TexPad[1]->Load("Asset/Textures/UI/AButton.png"); // インデックス 1: 仮にBボタン (Dodge)

	m_TexPad[2] = std::make_shared<KdTexture>();
	m_TexPad[2]->Load("Asset/Textures/UI/LButton.png"); // インデックス 2: 仮にXボタン (PowerAttack)

	m_TexPad[3] = std::make_shared<KdTexture>();
	m_TexPad[3]->Load("Asset/Textures/UI/XButton.png"); // インデックス 3: 仮にYボタン (Potion)

	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld); // 既存
	m_pos = { -580,-320 };
}



// PlayerUi.cpp (PlayerUi::DrawSprite 関数)

void PlayerUi::DrawSprite()
{
	// HPとスタミナの更新 (省略なし)
	{
		Math::Rectangle rc;
		rc = { 0,0,512,128 };
		Math::Vector2 pivot = { 0.0f,0.0f };
		Math::Color color = { 1,1,1,1 };
		if (m_Tex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[0], m_pos.x, m_pos.y, 280, 70, &rc, &color, pivot);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[1], m_pos.x, m_pos.y, 280, 70, &rc, &color, pivot);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[2], m_pos.x, m_pos.y, 279 * m_hp_rate, 70, &rc, &color, pivot);

			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[0], m_pos.x, -275, 280, 30, &rc, &color, pivot);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[3], m_pos.x, -275, 280, 30, &rc, &color, pivot);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[4], m_pos.x, -275, 279 * m_staminaRate, 30, &rc, &color, pivot);
		}
	}

	// ★★★ Keyアイコンの描画サイズ切り替えロジック ★★★
	{
		Math::Rectangle KeyRc;
		Math::Rectangle ItemRc;
		Math::Rectangle DodgeRc;
		ItemRc = { 0,0,512,512 };

		// 切り抜き範囲は固定の 14x14
		KeyRc = { 0,0,14,14 };

		DodgeRc = { 0,0,192,192 };
		Math::Vector2 pivot = { 0.5f,0.5f };
		Math::Color color = { 1,1,1,1 };

		// 描画サイズを格納する変数
		int DRAW_SIZE;

		// ★アクティブなテクスチャ配列とインデックスを決定 ★
		const std::shared_ptr<KdTexture>* keyboardTexArray = m_Tex;
		const std::shared_ptr<KdTexture>* gamePadTexArray = m_TexPad;
		const std::shared_ptr<KdTexture>* currentKeyTexArray;

		if (m_activeDevice == InputDevice::GamePad) {
			currentKeyTexArray = gamePadTexArray;
			// Pad使用時: 32x32
			DRAW_SIZE = 32;
		}
		else {
			currentKeyTexArray = keyboardTexArray;
			// キーボード使用時: 64x64
			DRAW_SIZE = 64;
		}

		if (m_Tex[5]) // ベースとなるテクスチャがロードされていることを確認
		{
			// Potionアイコンのベース画像 (m_Tex[5] - 常に同じ)
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[5], 450, -250, 64, 64, &ItemRc, &color, pivot);

			// --- 1. ポーションキー (E / Yボタン) ---
			// DRAW_SIZE (32 or 64) を適用
			KdShaderManager::Instance().m_spriteShader.DrawTex(currentKeyTexArray[m_itemKeyIndex], 450, -300, DRAW_SIZE, DRAW_SIZE, &KeyRc, &color, pivot);

			// --- 2. 必殺技キー (Q / Xボタン) ---
			// DRAW_SIZE (32 or 64) を適用
			KdShaderManager::Instance().m_spriteShader.DrawTex(currentKeyTexArray[m_powerAttackIndex], 550, -300, DRAW_SIZE, DRAW_SIZE, &KeyRc, &color, pivot);

			// --- 3. 回避キー (C / Bボタン) ---
			// DRAW_SIZE (32 or 64) を適用
			KdShaderManager::Instance().m_spriteShader.DrawTex(currentKeyTexArray[m_dodgeIndex], 550, -200, DRAW_SIZE, DRAW_SIZE, &KeyRc, &color, pivot);

			// 回避アクションアイコン（m_Tex[9] - 常に同じ）
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[9], 550, -150, 64, 64, &DodgeRc, &color, pivot);
		}
	}

	//必殺技
	{
		// ... (必殺技ゲージの描画ロジックは変更なし) ...
		// UI全体の描画設定
		const float OUGI_POS_X = 550.0f;
		const float OUGI_POS_Y_CENTER = -250.0f; // アイコン全体の中心Y座標
		const float OUGI_SIZE_W = 64.0f; // 描画幅
		const float OUGI_SIZE_H = 64.0f; // 描画高さ

		// テクスチャのピクセルサイズ (500x500と仮定)
		const int TEX_W = 500;
		const int TEX_H = 500;

		Math::Rectangle OugiRc = { 0,0,TEX_W,TEX_H }; // フルサイズの切り抜き用
		Math::Color color = { 1,1,1,1 };

		// ベースなどの描画に使用する中央ピボット
		Math::Vector2 pivot_center = { 0.5f, 0.5f };
		// ゲージ描画に使用する【中央下ピボット】
		Math::Vector2 pivot_bottom_center = { 0.5f, 1.0f };

		// ★★★ 元の行列を保存 ★★★
		Math::Matrix originalWorldMatrix;// = KdShaderManager::Instance().m_spriteShader.GetMatrix();

		if (m_Tex)
		{
			// 1. Ougi Base (常に描画) - 中央ピボット
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[10], OUGI_POS_X, OUGI_POS_Y_CENTER, OUGI_SIZE_W, OUGI_SIZE_H, &OugiRc, &color, pivot_center);

			// ----------------------------------------------------------------------
			// 2. Ougi Gauge (奥義ゲージ本体) - 180度回転適用

			float gauge_height_px = OUGI_SIZE_H * m_ougiRate;
			float tex_cut_y_start_f = 0.0f;
			float tex_cut_height_f = TEX_H * m_ougiRate;

			Math::Rectangle OugiGaugeRc = {
				0,
				static_cast<long>(tex_cut_y_start_f),
				TEX_W,
				static_cast<long>(tex_cut_height_f)
			};

			// ★★★ 個別オブジェクト専用の変換行列を構築 ★★★

			// 1. 回転 (Z軸周り180度回転でY軸を反転させる)
			Math::Matrix rotZ180 = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(180.0f));

			// 2. 平行移動 (ゲージの中心位置へ)
			// Y座標は奥義ゲージのアイコンの下端に合わせて -285 に固定
			Math::Matrix transMat = Math::Matrix::CreateTranslation(OUGI_POS_X, -285, 0.0f);

			// 3. 最終的なゲージ専用行列 (Mat) を作成: 回転 -> 平行移動
			Math::Matrix OugiGaugeMat = rotZ180 * transMat;

			// 4. ゲージ専用行列をシェーダーに設定
			KdShaderManager::Instance().m_spriteShader.SetMatrix(OugiGaugeMat);

			// 5. ゲージ本体を描画
			// 描画Y座標は 0.0f に固定し、pivot_bottom_center (中央下 0.5, 1.0) を使用
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_Tex[11],
				0.0f, // 描画X座標 (行列に位置情報があるため0)
				0.0f, // 描画Y座標 (行列に位置情報があるため0)
				OUGI_SIZE_W,
				gauge_height_px, // 描画高さをレートで調整
				&OugiGaugeRc,
				&color,
				pivot_bottom_center // ピボットを中央下(0.5, 1.0)に変更
			);

			// ★★★ 6. 行列を元の状態に戻す ★★★
			KdShaderManager::Instance().m_spriteShader.SetMatrix(originalWorldMatrix);
			// ----------------------------------------------------------------------

			// 3. Ougi Glow, 4. Ougi Text (元の行列で描画)
//			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[12], OUGI_POS_X, OUGI_POS_Y_CENTER, OUGI_SIZE_W, OUGI_SIZE_H, &OugiRc, &color, pivot_center);

			Math::Color text_color = { 1, 1, 1, 0.3f };
			if (m_ougiRate >= 1.0f)
			{
				text_color.w = 1.0f;
			}
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[13], OUGI_POS_X, OUGI_POS_Y_CENTER, OUGI_SIZE_W, OUGI_SIZE_H, &OugiRc, &text_color, pivot_center);
		}
	}
}

void PlayerUi::Update()

{

	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer) return;

	float displayhp = spPlayer->GetDisplayHp();
	float maxHp = spPlayer->GetMaxHP();
	float stamina = spPlayer->GetStamina();
	float maxStamina = spPlayer->GetMaxStamina();
	float ougiCost = spPlayer->GetPowerAttackCost();
	float ougiMaxCost = spPlayer->GetPowerAttackMaxCost();

	// ★★★ 必殺技ゲージのレート計算を修正 ★★★

	// コストが0の場合でもわずかにUIを動かすための最小レート
	const float MIN_VISIBLE_RATE = 0.01f;

	// ゲージがフルになるのは ougiMaxCost のとき。
	// コストが0のときに MIN_VISIBLE_RATE になるように計算を調整。
	// m_ougiRate = (ougiCost / ougiMaxCost) * (1.0f - MIN_VISIBLE_RATE) + MIN_VISIBLE_RATE;
	// ただし、コスト0のときは0%表示を維持し、コスト10から30の間の変化をより強調したい場合は、
	// 以前の回答で提案した区分線形関数が最適ですが、今回は元の線形計算にわずかな調整を加えます。

	// GamePadが有効な入力を発しているかチェック（例: スティックの動き、ボタンの押下）
	bool isGamePadInput = KdInputManager::Instance().GetDevice("GamePad0")->IsSomethingInput();

	// Keyboard/Mouseが有効な入力を発しているかチェック
	bool isKeyMouseInput = KdInputManager::Instance().GetDevice("KeyMouse")->IsSomethingInput();

	if (isGamePadInput)
	{
		m_activeDevice = InputDevice::GamePad;
	}
	else if (isKeyMouseInput)
	{
		m_activeDevice = InputDevice::Keyboard;
	}
	else
	{
		// 入力がない場合、前回のアクティブデバイスを維持するか、Noneに戻すかは要件次第。
		// ここでは、直近のデバイスを維持します。
	}

	// 2. 表示するテクスチャインデックスの決定
	if (m_activeDevice == InputDevice::GamePad)
	{
		// GamePadの場合
		m_itemKeyIndex = 3; // GamePad_DPadUp (仮)
		m_powerAttackIndex = 2; // GamePad_X (仮)
		m_dodgeIndex = 1; // GamePad_B (仮)
	}
	else // Keyboard or None (デフォルトはキーボード表示)
	{
		// キーボードの場合 (既存のテクスチャインデックスを使用)
		m_itemKeyIndex = 6; // EKey
		m_powerAttackIndex = 7; // QKey
		m_dodgeIndex = 8; // CKey
	}

	// deltaTime はメインループから渡されるか、DeltaTimer::Instance().GetDeltaTime() で取得
	const float deltaTime = DeltaTimer::Instance().GetDeltaTime();

	// m_keyAnimData が PlayerUi のメンバーとして定義されている前提
	m_keyAnimData.frameTime += deltaTime;

	// 1フレームの表示時間 (m_keyAnimData.frameDuration) を超えたらフレームを進める
	if (m_keyAnimData.frameTime >= m_keyAnimData.frameDuration)
	{
		m_keyAnimData.frameTime -= m_keyAnimData.frameDuration; // 残り時間を保持

		m_keyAnimData.currentFrame++;

		// フレームが総フレーム数を超えたらループさせる
		if (m_keyAnimData.currentFrame >= m_keyAnimData.totalFrames)
		{
			m_keyAnimData.currentFrame = 0;
		}
	}

	if (ougiMaxCost > 0) {
		float rate = ougiCost / ougiMaxCost;

		// コスト10 (rate=0.1) の時に、視覚的な伸びを確保するためにオフセットを導入し、
		// 10から30の間の変化が分かりやすくなるように、レート全体を少し持ち上げます。
		const float VISUAL_OFFSET = 0.05f; // 5%分だけ視覚的に持ち上げる

		m_ougiRate = rate + VISUAL_OFFSET;

		// ただし、MAXを超えないようにクランプし、0未満にならないようにします。
		m_ougiRate = std::min(m_ougiRate, 1.0f);
		m_ougiRate = std::max(m_ougiRate, 0.0f);
	}
	else {
		m_ougiRate = 0.0f;
	}
	// ★★★ 修正終わり ★★★

	m_hp_rate = displayhp / maxHp;
	m_staminaRate = stamina / maxStamina;

}