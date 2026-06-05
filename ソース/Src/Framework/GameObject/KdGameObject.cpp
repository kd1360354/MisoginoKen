	#include "KdGameObject.h"

	void KdGameObject::PostUpdate()
	{
		if (m_isDirty)
		{
			//拡縮行列計算
			Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
		
			//回転行列計算
			Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(
				DirectX::XMConvertToRadians(m_rot.y),
				DirectX::XMConvertToRadians(m_rot.x),
				DirectX::XMConvertToRadians(m_rot.z)
			);

			//座標行列計算
			Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

			//行列合成
			m_mWorld = scaleMat * rotMat * transMat;

			m_isDirty = false;
		}
	}

	void KdGameObject::DrawDebug()
	{
		// 早期リターン
		if (!m_pDebugWire)return;

		m_pDebugWire->Draw();
	}


	void KdGameObject::LoadParameters(const nlohmann::json& _j)
	{
	
		// 🌍 全オブジェクト共通：座標・回転・拡縮
		if (_j.contains("position")) { m_pos = { _j["position"][0], _j["position"][1], _j["position"][2] }; }
		if (_j.contains("rotation")) { m_rot = { _j["rotation"][0], _j["rotation"][1], _j["rotation"][2] }; }
		if (_j.contains("scale")) { m_scale = { _j["scale"][0], _j["scale"][1], _j["scale"][2] }; }

		// 🎨 全オブジェクト共通：モデルのパス
		if (_j.contains("modelPath")) { m_ModelPath = _j["modelPath"]; }
	}

	void KdGameObject::CalcDistSqrFromCamera(const Math::Vector3& camPos)
	{
		m_distSqrFromCamera = (m_mWorld.Translation() - camPos).LengthSquared();
	}

	bool KdGameObject::Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
	{
		if (!m_pCollider) { return false; }

		return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
	}

	bool KdGameObject::Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults)
	{
		if (!m_pCollider) { return false; }

		return m_pCollider->Intersects(targetBox, m_mWorld, pResults);
	}

	bool KdGameObject::Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
	{
		if (!m_pCollider) { return false; }

		return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
	}

	bool KdGameObject::Intersects(const KdCollider::BoxInfo& targetBox, const Math::Matrix& targetWorldMatrix, std::list<KdCollider::CollisionResult>* pResults)
	{
		if (!m_pCollider) { return false; }
		 //自身のコライダーをワールド行列で変換し、相手のコライダーもワールド行列で変換して判定
		return m_pCollider->Intersects(targetBox, m_mWorld, pResults);

	}
