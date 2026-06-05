#pragma once
#include "../Constants/GameScenenConstsnts/GameScenenConstsnts.h"

class SoundManager {
public:
	static SoundManager& Instance() { static SoundManager instance; return instance; }

	// ★ Title と Result を追加
	enum class BgmState { Title, Ambient, NoobBattle, MidBoss, LastBoss, Result, Stop };

	void ChangeBGM(BgmState state) {
		// 同じBGMが既に流れている場合は何もしない（フェーズ移行時などの再再生防止）
		if (m_nowState == state && m_currentBgm && m_currentBgm->IsPlaying()) {
			return;
		}

		// 前の音を止める
		if (m_currentBgm) {
			m_currentBgm->Stop();
			m_currentBgm = nullptr;
		}

		m_nowState = state;

		// 次の音を再生
		switch (state) {
		case BgmState::Title:      m_currentBgm = KdAudioManager::Instance().Play("Asset/Audio/BGM/TitleBGM.wav", true); break;
		case BgmState::Ambient:    m_currentBgm = KdAudioManager::Instance().Play(SceneParam::Audio::SE_WIND_AMBIENT, true); break;
		case BgmState::NoobBattle: m_currentBgm = KdAudioManager::Instance().Play(SceneParam::Audio::BGM_NOOB_BATTLE, true); break;
		case BgmState::MidBoss:    m_currentBgm = KdAudioManager::Instance().Play(SceneParam::Audio::BGM_MID_BATTLE, true); break;
		case BgmState::LastBoss:   m_currentBgm = KdAudioManager::Instance().Play(SceneParam::Audio::BGM_BOSS_BATTLE, true); break;
		case BgmState::Result:     m_currentBgm = KdAudioManager::Instance().Play("Asset/Audio/BGM/ResultBGM.wav", true); break;
		case BgmState::Stop:       break;
		}

		// 音量の統一管理（必要に応じて調整）
		if (m_currentBgm) {
			m_currentBgm->SetVolume(0.2f);
		}
	}

	void StopAll() {
		ChangeBGM(BgmState::Stop);
	}

private:
	SoundManager() {}
	~SoundManager() {}
	BgmState m_nowState = BgmState::Stop;
	std::shared_ptr<KdSoundInstance> m_currentBgm = nullptr;
};