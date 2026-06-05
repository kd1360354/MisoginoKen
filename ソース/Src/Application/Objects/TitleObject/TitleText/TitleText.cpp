#include "TitleText.h"
#include"../../../Constants/SceneConstants/TitleConstants/TitleConstants.h"
// NOTE: 必要なヘッダー（KdTexture, Math::Vector2など）はTitleText.hで定義されている前提

void TitleText::Init()
{
	// ★ Enumと定数パスを使用
	m_Tex[static_cast<int>(TitleParam::TexIndex::Background)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::Background)]->Load(TitleParam::Path::BACKGROUND);

	m_Tex[static_cast<int>(TitleParam::TexIndex::TitleText)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::TitleText)]->Load(TitleParam::Path::TITLE_TEXT);

	m_Tex[static_cast<int>(TitleParam::TexIndex::Logo1)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::Logo1)]->Load(TitleParam::Path::LOGO_1);

	m_Tex[static_cast<int>(TitleParam::TexIndex::Logo2)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::Logo2)]->Load(TitleParam::Path::LOGO_2);

	m_Tex[static_cast<int>(TitleParam::TexIndex::ClickToStart)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::ClickToStart)]->Load(TitleParam::Path::CLICK_TO_START);

	m_Tex[static_cast<int>(TitleParam::TexIndex::LeftArrow)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::LeftArrow)]->Load(TitleParam::Path::LEFT_ARROW);

	m_Tex[static_cast<int>(TitleParam::TexIndex::Percent25)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(TitleParam::TexIndex::Percent25)]->Load(TitleParam::Path::PERCENT_25);

}

void TitleText::DrawSprite()
{
	{
		// ★ ローカル変数名を小文字スネークケースに統一
		Math::Rectangle background_rc;
		Math::Rectangle left_arrow_rc;
		Math::Rectangle now_loading_text_rc;

		// ★ 定数を使用
		background_rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		Math::Vector2  pivot = { 0.5f, 0.5f };
		left_arrow_rc = { 0, 0, ARROW_SIZE, ARROW_SIZE };
		now_loading_text_rc = { 0, 0, (int)TitleParam::UI::LOADING_RECT_SZ.x, (int)TitleParam::UI::LOADING_RECT_SZ.y };

		Math::Color color = { 1, 1, 1, 1 };
		if (m_Tex[static_cast<int>(TitleParam::TexIndex::Background)])
		{
			// ★★★ ここから ClickToStart の点滅処理 ★★★

			const float min_alpha = BLINK_ALPHA_MIN;
			const float max_alpha = BLINK_ALPHA_MAX;

			// 2. 点滅の変動幅
			const float BLINK_RANGE = max_alpha - min_alpha;

			// sinValueは 0.0～1.0
			float sin_value = (sin(m_blink_timer) + max_alpha) / 2.0f;
			// alphaは MIN_ALPHA～MAX_ALPHA
			float alpha = (sin_value * BLINK_RANGE) + min_alpha;

			// 計算したアルファ値を適用した色
			Math::Color blink_color = { 1, 1, 1, alpha };

			// 背景
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::Background)], TitleParam::UI::CENTER_POS.x, TitleParam::UI::CENTER_POS.y, SCREEN_WIDTH, SCREEN_HEIGHT, &background_rc, &color, pivot);

			// ClickToStart (点滅色)
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::ClickToStart)], TitleParam::UI::CENTER_POS.x, TitleParam::UI::CENTER_POS.y, SCREEN_WIDTH, SCREEN_HEIGHT, &background_rc, &blink_color, pivot);

			// ロゴ1
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::Logo1)], TitleParam::UI::CENTER_POS.x, TitleParam::UI::CENTER_POS.y, SCREEN_WIDTH, SCREEN_HEIGHT, &background_rc, &color, pivot);

			// ロゴ2
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::Logo2)], TitleParam::UI::CENTER_POS.x, TitleParam::UI::CENTER_POS.y, SCREEN_WIDTH, SCREEN_HEIGHT, &background_rc, &color, pivot);

			// タイトルテキスト
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::TitleText)], TitleParam::UI::CENTER_POS.x, TitleParam::UI::CENTER_POS.y, SCREEN_WIDTH, SCREEN_HEIGHT, &background_rc, &color, pivot);

			// 左矢印 (点滅色)
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::LeftArrow)], TitleParam::UI::LEFT_ARROW_POS.x, TitleParam::UI::LEFT_ARROW_POS.y, ARROW_SIZE, ARROW_SIZE, &left_arrow_rc, &blink_color, pivot);

			if (m_change_loading) {
				// 25% (ロード中)
				KdShaderManager::Instance().m_spriteShader.DrawTex(m_Tex[static_cast<int>(TitleParam::TexIndex::Percent25)], TitleParam::UI::CENTER_POS.x, TitleParam::UI::CENTER_POS.y, SCREEN_WIDTH, SCREEN_HEIGHT, &background_rc, &color, pivot);
				// Now Loading Text
				KdShaderManager::Instance().m_spriteShader.DrawTex(
					m_Tex[static_cast<int>(TitleParam::TexIndex::NowLoading)],
					TitleParam::UI::LOADING_POS.x, TitleParam::UI::LOADING_POS.y,
					(int)TitleParam::UI::LOADING_RECT_SZ.x, (int)TitleParam::UI::LOADING_RECT_SZ.y,
					&now_loading_text_rc, &color, pivot
				);
			}
		}
	}
}

void TitleText::Update()
{
	m_blink_timer += m_blink_speed;
	if (m_blink_timer > m_blink_cycle) {
		m_blink_timer -= m_blink_cycle;
	}
	//ボタンが押されているかの判定
	bool isAnyButtonPress =
		KdInputManager::Instance().IsPress(APP_BTN_ATTACK)		 ||  
		KdInputManager::Instance().IsPress(APP_BTN_GUARD)		 ||  
		KdInputManager::Instance().IsPress(APP_BTN_DODGE)		 ||     
		KdInputManager::Instance().IsPress(APP_BTN_POTION)		 ||    
		KdInputManager::Instance().IsPress(APP_BTN_POWER_ATK)    || 
		KdInputManager::Instance().IsPress(APP_BTN_LOCKON)       ||      
		KdInputManager::Instance().IsPress(APP_BTN_MODE_CHANGE)  ||      
		KdInputManager::Instance().IsPress(APP_BTN_CAMERA_RESET) ||      
		KdInputManager::Instance().IsPress(APP_BTN_JUMP);      

	if (isAnyButtonPress)
	{
		m_change_loading = true;
	}
	else
	{
		m_change_loading = false;
	}
}