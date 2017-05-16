#pragma once
#include "Particle.h"

namespace hiveFluidSimulation
{
	class CRigidParticle : public CParticle
	{
	public:
		CRigidParticle();
		~CRigidParticle();

		virtual void correctInfoV() override;

		float getVolume()     const {return m_Volume;}
		float getBoundryPsi() const {return m_BoundaryPsi;}

		void calVolume();
		void setBoundryPsi(float vPsi) {m_BoundaryPsi = vPsi;}

	private:
		float m_Volume;
		float m_BoundaryPsi;
	};
}