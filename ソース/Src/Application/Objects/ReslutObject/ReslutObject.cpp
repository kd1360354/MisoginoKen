#include "ReslutObject.h"
#include"../../Constants/SceneConstants/ResultConstants/ResultConstants.h"
#include"../../Objects/Character/Player/Player.h"
#include"../../Scene/SceneManager.h"

void ResultObject::Init()
{
	// ★ Enum名 (RESULT_TEX_...) と定数パス名 (RESULT_TEX_PATH_...) を使用

	m_Tex[static_cast<int>(ResultParam::TexIndex::Background)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(ResultParam::TexIndex::Background)]->Load(ResultParam::Path::BACKGROUND);

	m_Tex[static_cast<int>(ResultParam::TexIndex::ClickToTitle)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(ResultParam::TexIndex::ClickToTitle)]->Load(ResultParam::Path::CLICK_TO_TITLE);
	m_Tex[static_cast<int>(ResultParam::TexIndex::LeftArrow)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(ResultParam::TexIndex::LeftArrow)]->Load(ResultParam::Path::LEFT_ARROW);

	m_Tex[static_cast<int>(ResultParam::TexIndex::ThankYou)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(ResultParam::TexIndex::ThankYou)]->Load(ResultParam::Path::THANK_YOU);
	m_Tex[static_cast<int>(ResultParam::TexIndex::GameOver)] = std::make_shared<KdTexture>();
	m_Tex[static_cast<int>(ResultParam::TexIndex::GameOver)]->Load(ResultParam::Path::GAMEOVER);
}

void ResultObject::DrawSprite()
{
	{
		Math::Rectangle BackGroundRc;

		// ★ 定数を使用
		BackGroundRc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

		Math::Vector2 pivot = { 0.5f, 0.5f };
		Math::Color color = { 1, 1, 1, 1 };

		// Enum名 (RESULT_TEX_BACKGROUND) を使用
		if (m_Tex[static_cast<int>(ResultParam::TexIndex::Background)])
		{
			// ★★★ ClickToTitle の点滅処理 ★★★

			const float MIN_ALPHA = BLINK_ALPHA_MIN;
			const float MAX_ALPHA = BLINK_ALPHA_MAX;

			// 2. 点滅の変動幅
			const float BLINK_RANGE = MAX_ALPHA - MIN_ALPHA;

			// sin関数で滑らかに [0, 1] の範囲で変化する値を計算 (修正版のロジックを適用)
			float normalizedSin = (sin(m_blinkTimer) + 1.0f) / 2.0f;
			float alpha = (normalizedSin * BLINK_RANGE) + MIN_ALPHA;

			// 計算したアルファ値を適用した色
			Math::Color blinkColor = { 1, 1, 1, alpha };

			// 1. 背景 (通常色)
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_Tex[static_cast<int>(ResultParam::TexIndex::Background)], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &BackGroundRc, &color, pivot);

			// 2. Click to Title (点滅色)
			// Enum名 (RESULT_TEX_CLICK_TO_TITLE) と座標定数 (RESULT_CLICK_TO_TITLE_POS) を使用
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_Tex[static_cast<int>(ResultParam::TexIndex::ClickToTitle)], ResultParam::UI::CLICK_TO_TITLE_POS.x, ResultParam::UI::CLICK_TO_TITLE_POS.y,
				SCREEN_WIDTH, SCREEN_HEIGHT, &BackGroundRc, &blinkColor, pivot);

			// 3. 左矢印 (点滅色)
			// Enum名 (RESULT_TEX_LEFT_ARROW) と座標定数 (RESULT_LEFT_ARROW_POS) を使用
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_Tex[static_cast<int>(ResultParam::TexIndex::LeftArrow)], ResultParam::UI::LEFT_ARROW_POS.x, ResultParam::UI::LEFT_ARROW_POS.y,
				ARROW_SIZE, ARROW_SIZE, nullptr, &blinkColor, pivot);

			// 4. Thank you for Playing (通常色)
			// Enum名 (RESULT_TEX_THANK_YOU) と座標定数 (RESULT_THANK_YOU_POS) を使用
			std::shared_ptr<Player> spPlayer = SceneManager::Instance().GetPlayer();
			if (spPlayer->GetHP()<=0)
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(
					m_Tex[static_cast<int>(ResultParam::TexIndex::GameOver)], {}, {},
					GAMEOVER_SIZE_W, GAMEOVER_SIZE_H, nullptr, &color, pivot);
				
			}
			else
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(
					m_Tex[static_cast<int>(ResultParam::TexIndex::ThankYou)], ResultParam::UI::THANK_YOU_POS.x, ResultParam::UI::THANK_YOU_POS.y,
					SCREEN_WIDTH, SCREEN_HEIGHT, &BackGroundRc, &color, pivot);
			}
		}
	}
}

void ResultObject::Update()
{
	// ★ 定数 BLINK_SPEED_DEFAULT と BLINK_CYCLE_DEFAULT を使用
	m_blinkTimer += BLINK_SPEED_DEFAULT;
	if (m_blinkTimer > BLINK_CYCLE_DEFAULT) {
		m_blinkTimer -= BLINK_CYCLE_DEFAULT;
	}
}