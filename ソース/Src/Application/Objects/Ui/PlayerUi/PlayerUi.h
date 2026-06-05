#pragma once
#include"../UiBase.h"
class Player;
class PlayerUi :public UiBase
{
public:
	void Init() override;
	void DrawSprite() override;
	void Update()override;
	const char* GetTypeName()const override { return "PlayerUI"; }


	void SetPlayer(std::weak_ptr<Player> player) { m_wpPlayer = player; }
	struct AnimationData {
		float frameTime = 0.0f; // 現在のフレームが保持されている時間
		int currentFrame = 0;   // 現在表示されているフレームのインデックス (0, 1, 2, 3...)
		int totalFrames = 4;    // アニメーションの総フレーム数
		float frameDuration = 0.15f; // 1フレームあたりの表示時間 (例: 0.15秒)
	};
private:
	std::weak_ptr<Player> m_wpPlayer; // プレイヤーの参照

	float m_ougiRate = 0.0f; // 必殺技コストのレート (0.0f〜1.0f)
	float m_ougiTexWidth = 500.0f; // ougi_gaugeのテクスチャ元幅 (仮定)
	float m_ougiTexHeight = 500.0f; // ougi_gaugeのテクスチャ元高さ (仮定)

	// 【新規】コントローラー用テクスチャ (例として4つ追加)
	std::shared_ptr<KdTexture> m_TexPad[4]; // A, B, X, Y または L shoulder, R shoulderなど

	// 【新規】デバイス判定用 enum とメンバー
	enum class InputDevice { Keyboard, GamePad, None };
	InputDevice m_activeDevice = InputDevice::None;
	AnimationData m_keyAnimData; // キー/ボタンアニメーションデータ
	// 【新規】キー/ボタンのテクスチャインデックス
	int m_itemKeyIndex;      // Potion (Eキー)
	int m_powerAttackIndex;  // PowerAttack (Qキー)
	int m_dodgeIndex;        // Dodge (Cキー)
};