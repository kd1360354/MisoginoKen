#include "KdAnimation.h"
#include "../Direct3D/KdModel.h"

// 二分探索で、指定時間から次の配列要素のKeyIndexを求める関数
// list		… キー配列
// time		… 時間
// 戻り値	… 次の配列要素のIndex
template<class T>
int BinarySearchNextAnimKey(const std::vector<T>& list, float time)
{
	int low = 0;
	int high = (int)list.size();
	while (low < high)
	{
		int mid = (low + high) / 2;
		float midTime = list[mid].m_time;

		if (midTime <= time) low = mid + 1;
		else high = mid;
	}
	return low;
}

bool KdAnimationData::Node::InterpolateTranslations(Math::Vector3& result, float time)
{
	if (m_translations.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_translations, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_translations.front().m_vec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_translations.size()) {
		result = m_translations.back().m_vec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_translations[keyIdx - 1];	// 前のキー
		auto& next = m_translations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(
			prev.m_vec,
			next.m_vec,
			f
		);
	}

	return true;
}

bool KdAnimationData::Node::InterpolateRotations(Math::Quaternion& result, float time)
{
	if (m_rotations.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_rotations, time);
	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_rotations.front().m_quat;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_rotations.size()) {
		result = m_rotations.back().m_quat;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_rotations[keyIdx - 1];	// 前のキー
		auto& next = m_rotations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMQuaternionSlerp(
			prev.m_quat,
			next.m_quat,
			f
		);
	}

	return true;
}

bool KdAnimationData::Node::InterpolateScales(Math::Vector3& result, float time)
{
	if (m_scales.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_scales, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_scales.front().m_vec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_scales.size()) {
		result = m_scales.back().m_vec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_scales[keyIdx - 1];	// 前のキー
		auto& next = m_scales[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(
			prev.m_vec,
			next.m_vec,
			f
		);
	}

	return true;
}

void KdAnimationData::Node::Interpolate(Math::Matrix& rDst, float time)
{
	// ベクターによる拡縮補間
	bool isChange = false;
	Math::Matrix scale;
	Math::Vector3 resultVec;
	if (InterpolateScales(resultVec, time))
	{
		scale = scale.CreateScale(resultVec);
		isChange = true;
	}

	// クォタニオンによる回転補間
	Math::Matrix rotate;
	Math::Quaternion resultQuat;
	if (InterpolateRotations(resultQuat, time))
	{
		rotate = rotate.CreateFromQuaternion(resultQuat);
		isChange = true;
	}

	// ベクターによる座標補間
	Math::Matrix trans;
	if (InterpolateTranslations(resultVec, time))
	{
		trans = trans.CreateTranslation(resultVec);
		isChange = true;
	}

	if (isChange)
	{
		rDst = scale * rotate * trans;
	}
}

void KdAnimator::ChangeAnimation(const std::shared_ptr<KdAnimationData>& rNextAnim, float blendTime)
{
	// 次のアニメーションが現在のものと同じ、またはnullptrの場合はブレンド不要
	if (rNextAnim == nullptr || rNextAnim == m_spAnimation)
	{
		SetAnimation(rNextAnim, m_isLoop); // 単純に切り替える
		return;
	}

	// 次のアニメーションを設定
	m_spNextAnimation = rNextAnim;

	// 現在の再生時間とループ設定を、次のアニメーションの時間に引き継ぐ
	m_nextTime = 0.0f; // 次アニメは最初から再生開始

	// ブレンド設定
	m_isBlending = true;
	m_blendTimer = 0.0f;
	m_blendDuration = blendTime;
}

void KdAnimator::AdvanceTime(std::vector<KdModelWork::Node>& rNodes, float speed)
{
	if (!m_spAnimation) return;

	// ==========================
	// ブレンド中の場合
	// ==========================
	if (m_isBlending && m_spNextAnimation)
	{
		m_blendTimer += speed;
		float t = std::min(m_blendTimer / m_blendDuration, 1.0f);

	for (size_t i = 0; i < m_spAnimation->m_nodes.size(); ++i)
{
    auto& nodeA = m_spAnimation->m_nodes[i];
    auto& nodeB = m_spNextAnimation->m_nodes[i];
    UINT idx = nodeA.m_nodeOffset;

    Math::Matrix poseA, poseB;
    nodeA.Interpolate(poseA, m_time);
    nodeB.Interpolate(poseB, m_nextTime);

	// --- 修正版ここから ---
	Math::Vector3 scaleA, scaleB;
	Math::Quaternion rotA, rotB;
	Math::Vector3 posA, posB;

	poseA.Decompose(scaleA, rotA, posA);
	poseB.Decompose(scaleB, rotB, posB);

	// ★★★ [ADD] 拡縮（Scale）の補間を追加 ★★★
	Math::Vector3 scale = DirectX::XMVectorLerp(scaleA, scaleB, t);

	Math::Vector3 pos = DirectX::XMVectorLerp(posA, posB, t);
	Math::Quaternion rot = DirectX::XMQuaternionSlerp(rotA, rotB, t);

	// ★★★ [MOD] 補間した3要素（S, R, T）を合成 ★★★
	rNodes[idx].m_localTransform =
		Math::Matrix::CreateScale(scale) * // 拡縮
		Math::Matrix::CreateFromQuaternion(rot) * // 回転
		Math::Matrix::CreateTranslation(pos);        // 位置
}


		// 時間更新
		m_time += speed;
		m_nextTime += speed;

		// ブレンド終了判定
		if (t >= 1.0f)
		{
			m_isBlending = false;
			m_spAnimation = m_spNextAnimation;
			m_spNextAnimation = nullptr;
			m_time = m_nextTime;
		}
		return;
	}

	// ==========================
	// 通常アニメーション処理
	// ==========================
	for (auto& rAnimNode : m_spAnimation->m_nodes)
	{
		UINT idx = rAnimNode.m_nodeOffset;
		rAnimNode.Interpolate(rNodes[idx].m_localTransform, m_time);

		if (idx == 0)
		{
			float currentY = rNodes[idx].m_localTransform.Translation().y;
			rNodes[idx].m_localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, currentY, 0.0f);
		}
	}

	m_time += speed;
	if (m_time >= m_spAnimation->m_maxLength)
	{
		m_time = m_isLoop ? 0.0f : m_spAnimation->m_maxLength;
	}
}

std::string KdAnimator::GetCurrentAnimationName() const
{// 1. 現在のアニメーションデータが存在するか確認する
	if (m_spAnimation)
	{
		// 2. 存在すれば、そのデータが持つ名前 (m_name) を返す
		return m_spAnimation->m_name;
	}

	// 3. アニメーションが設定されていない場合は空文字列を返す
	return "";
}
