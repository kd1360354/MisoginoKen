#pragma once

// アニメーションキー(クォータニオン
struct KdAnimKeyQuaternion
{
	float				m_time = 0;		// 時間
	Math::Quaternion	m_quat;			// クォータニオンデータ
};

// アニメーションキー(ベクトル
struct KdAnimKeyVector3
{
	float				m_time = 0;		// 時間
	Math::Vector3		m_vec;			// 3Dベクトルデータ
};

//============================
// アニメーションデータ
//============================
struct KdAnimationData
{
	// アニメーション名
	std::string		m_name;
	// アニメの長さ
	float			m_maxLength = 0;

	// １ノードのアニメーションデータ
	struct Node
	{
		int			m_nodeOffset = -1;	// 対象モデルノードのOffset値

		// 各チャンネル
		std::vector<KdAnimKeyVector3>		m_translations;	// 位置キーリスト
		std::vector<KdAnimKeyQuaternion>	m_rotations;	// 回転キーリスト
		std::vector<KdAnimKeyVector3>		m_scales;		// 拡縮キーリスト

		void Interpolate(Math::Matrix& rDst, float time);
		bool InterpolateTranslations(Math::Vector3& result, float time);
		bool InterpolateRotations(Math::Quaternion& result, float time);
		bool InterpolateScales(Math::Vector3& result, float time);
	};

	// 全ノード用アニメーションデータ
	std::vector<Node>	m_nodes;
};

class KdAnimator
{
public:

	inline void SetAnimation(const std::shared_ptr<KdAnimationData>& rData, bool isLoop = true)
	{
		m_spAnimation = rData;
		m_isLoop = isLoop;

		m_time = 0.0f;
	}
	float GetNormalizedTime() const
	{
		// アニメーションデータが存在しない場合や、全長が0の場合は0.0fを返す
		if (m_spAnimation == nullptr || m_spAnimation->m_maxLength <= 0.0f) {
			return 0.0f;
		}

		// 正規化時間の計算: (現在の時間 / アニメーションの全長)
		// m_time: 現在の再生時間 (float)
		// m_spAnimation->m_maxLength: アニメーションの全長 (float)
		float normalizedTime = m_time / m_spAnimation->m_maxLength;

		// 値が1.0fを超えないようにクランプする (終了後の処理を考慮)
		// ※ ループ処理を考慮し、正確には fmodf(m_time, m_spAnimation->m_maxLength) / m_spAnimation->m_maxLength で計算すべきですが、
		//    現状のAdvanceTime()のm_time管理に基づき、単純な割り算で実装します。
		return std::min(normalizedTime, 1.0f);
	}
	// ★★★ ここまで ★★★
	void ChangeAnimation(const std::shared_ptr<KdAnimationData>&rNextAnim,float blendtime=0.3f);

	// アニメーションが終了してる？
	bool IsAnimationEnd() const
	{
		if (m_spAnimation == nullptr) { return true; }
		if (m_time >= m_spAnimation->m_maxLength) { return true; }

		return false;
	}

	float GetTime() const
	{
		// 現在の再生時間 (秒) をそのまま返す
		return m_time;
	}
	void SetTime(float time)
	{
		// 受け取った値をそのまま現在の再生時間に設定
		m_time = time;
	}
	float GetAnimationTime() const
	{
		// アニメーションデータが存在しない場合はエラーまたは安全な値 (例: 0.0f) を返す
		if (m_spAnimation == nullptr) {
			return 0.0f;
		}

		// 現在のアニメーションデータの総再生時間 (秒) を返す
		return m_spAnimation->m_maxLength;
	}

	void SetAnimationEnd()
	{
		// アニメーションデータが存在する場合のみ実行
		if (m_spAnimation)
		{
			// アニメーションの総時間を m_time に設定することで、次の IsAnimationEnd() 判定で true を返すようにする
			m_time = m_spAnimation->m_maxLength;
		}
	}
	// アニメーションの更新
	void AdvanceTime(std::vector<KdModelWork::Node>& rNodes, float speed = 1.0f);

	std::string GetCurrentAnimationName() const;

private:

	std::shared_ptr<KdAnimationData>	m_spAnimation = nullptr;	// 再生するアニメーションデータ
	std::shared_ptr<KdAnimationData>	m_spNextAnimation = nullptr;	// 再生するアニメーションデータ

	float m_time = 0.0f;

	float m_nextTime = 0.0f;

	bool m_isLoop = false;

	// ★ブレンド管理
	bool m_isBlending = false;
	float m_blendTimer = 0.0f;
	float m_blendDuration = 0.0f;
};
