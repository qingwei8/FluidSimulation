#pragma once
#include "Particle.h"

namespace hiveFluidSimulation
{
	class CFluidParticle : public CParticle
	{
	public:
		CFluidParticle();
		~CFluidParticle();

		virtual void predictInfoV() override;
		virtual void correctInfoV() override;
		virtual void updateInfoV() override;

		void computeXSPHViscosity();
		float getLamda()   const {return m_Lamda;}
		float getDensity() const {return m_Density;}

	private:
		FLUIDSIMULATION_DLL_EXPORT void __predictInitialVelocity(const glm::vec3& vExtAcceleration);
		void __predictInitialPosition();
		void __confineToBox();
		void __calDensities();
		void __calLambda();
		void __calDeltP();
		void __applyDeltaP();
		void __updateVelocity();
		void __calAccelerationFromRigid();

		float __calSCorr(const glm::vec3& vPositionJ);
		glm::vec3 __calVorticityForce();
		glm::vec3 __calXsphViscosity();
		glm::vec3 __calEta(float vVorticityMag);

		

	private:
		float     m_Density;
		float     m_Lamda;
		glm::vec3 m_AccelerationFromRigid;
	};
}