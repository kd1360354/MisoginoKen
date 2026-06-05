	#pragma once
	class IDamageable {
	public:

		virtual ~IDamageable() {}
		virtual void OnHit(float damage) = 0;
	};