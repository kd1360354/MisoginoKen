#pragma once  
#include<chrono>  
#include <algorithm>  

class DeltaTimer  
{  
public:  
	// タイムポイントの型を定義  
	using Clock = std::chrono::high_resolution_clock;  
	using TimePoint = Clock::time_point;  

	// Singleton インスタンス取得関数の修正  
	static DeltaTimer& Instance()
	{  
		static DeltaTimer instance;
		return instance;  
	}  

private:  
	TimePoint m_lastTime;   // 前回のフレームの時刻  
	float m_deltaTime = 0.0f; // デルタタイム (秒)  

public:  
	void Init()  
	{  
		// 最初のフレームの時刻を記録  
		m_lastTime = Clock::now();  
	}  

	void Update()  
	{  
		// 1. 現在の時刻を取得  
		TimePoint currentTime = Clock::now();  

		// 2. 前回からの経過時間を計算  
		// duration_cast でナノ秒(ns)やマイクロ秒(us)から秒(float)へ変換  
		std::chrono::duration<float> elapsed = currentTime - m_lastTime;  
		m_deltaTime = elapsed.count(); // 経過時間(秒)を取得  

		// 3. 極端に大きな値を除外するクランプ処理 (オプション)  
		// デバッグやロード中の大きなフレームタイムを制限  
		m_deltaTime = std::min(m_deltaTime, 0.1f); // 最大で0.1秒(10FPS)に制限  

		// 4. 次のフレームのために現在の時刻を保存  
		m_lastTime = currentTime;  
	}  

	float GetDeltaTime() const { return m_deltaTime; }  
};