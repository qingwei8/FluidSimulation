#include "FluidParticle.h"
#include "Common.h"
#include "RigidParticle.h"
#include "RunConfig.h"
#include "Scene.h"
#include "SPHKernels.h"
#include <iostream>
#include <cmath>

using namespace hiveFluidSimulation;

CFluidParticle::CFluidParticle() : m_Density(0.0), m_Lamda(0.0), m_AccelerationFromRigid(glm::vec3(0.0f))
{
}

CFluidParticle::~CFluidParticle()
{
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__calDensities()//change...
{
	m_Density = m_Mass * CCubicKernel::W_zero();
	for (unsigned int i=0; i<m_FluidNeighborSet.size(); ++i)
	{
		glm::vec4 Temp1 = m_NewPosition - m_FluidNeighborSet[i]->getNewPosition();
		m_Density += m_FluidNeighborSet[i]->getMass() * CCubicKernel::W(Eigen::Matrix<float, 3, 1>(Temp1.x, Temp1.y, Temp1.z));
	}

	for (unsigned int k=0; k<m_RigidNeighborSet.size(); ++k)
	{
		glm::vec4 Temp1 = m_NewPosition - m_RigidNeighborSet[k]->getNewPosition();
		m_Density +=  dynamic_cast<CRigidParticle*>(m_RigidNeighborSet[k])->getBoundryPsi() * CCubicKernel::W(Eigen::Matrix<float, 3, 1>(Temp1.x, Temp1.y, Temp1.z));
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__calLambda()//change...
{
	float Eps = 1.0e-6;
	// Evaluate constraint function
	float C = std::max(m_Density / g_Density0 - 1.0, 0.0);			// clamp to prevent particle clumping at surface

	Eigen::Matrix<float, 3, 1> Pos1(m_NewPosition.x, m_NewPosition.y, m_NewPosition.z);
	if (C != 0.0)
	{
		// Compute gradients dC/dx_j 
		float Sum_grad_C2 = 0.0;
		Eigen::Matrix<float, 3, 1> GradC_i(0.0, 0.0, 0.0);

		for (unsigned int j = 0; j < m_FluidNeighborSet.size(); j++)
		{
			glm::vec4 NeighborPos = m_FluidNeighborSet[j]->getNewPosition();
			Eigen::Matrix<float, 3, 1> Pos2(NeighborPos.x, NeighborPos.y, NeighborPos.z);
			Eigen::Matrix<float, 3, 1> GradC_j = -m_Mass / g_Density0 * CCubicKernel::gradW(Pos1 - Pos2);
			Sum_grad_C2 += GradC_j.squaredNorm();
			GradC_i -= GradC_j;
		}

		for (unsigned int k=0; k<m_RigidNeighborSet.size(); k++)
		{
			float BoundryPsi = dynamic_cast<CRigidParticle*>(m_RigidNeighborSet[k])->getBoundryPsi();

			glm::vec4 NeighborPos = m_RigidNeighborSet[k]->getNewPosition();
			Eigen::Matrix<float, 3, 1> Pos2(NeighborPos.x, NeighborPos.y, NeighborPos.z);
			Eigen::Matrix<float, 3, 1> GradC_j = - BoundryPsi / g_Density0 * CCubicKernel::gradW(Pos1 - Pos2);
			Sum_grad_C2 += GradC_j.squaredNorm();
			GradC_i -= GradC_j;
		}

		Sum_grad_C2 += GradC_i.squaredNorm();

		// Compute lambda
		m_Lamda = -C / (Sum_grad_C2 + Eps);
	}
	else
		m_Lamda = 0.0;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__calDeltP()//change...
{
	Eigen::Matrix<float, 3, 1> Corr;
	Corr.setZero();

	for (unsigned int i=0; i<m_FluidNeighborSet.size(); ++i)
	{
		glm::vec4 Temp = m_NewPosition - m_FluidNeighborSet[i]->getNewPosition();
		Eigen::Matrix<float, 3, 1> GradC_j = -m_FluidNeighborSet[i]->getMass() / g_Density0 * CCubicKernel::gradW(Eigen::Matrix<float, 3, 1>(Temp.x, Temp.y, Temp.z));
		Corr -= (m_Lamda + dynamic_cast<CFluidParticle*>(m_FluidNeighborSet[i])->getLamda()) * GradC_j;
	}

	for (unsigned int k=0; k<m_RigidNeighborSet.size(); ++k)
	{
		float BoundryPsi = dynamic_cast<CRigidParticle*>(m_RigidNeighborSet[k])->getBoundryPsi();
		glm::vec4 Temp = m_NewPosition - m_RigidNeighborSet[k]->getNewPosition();
		Eigen::Matrix<float, 3, 1> GradC_j = - BoundryPsi / g_Density0 * CCubicKernel::gradW(Eigen::Matrix<float, 3, 1>(Temp.x, Temp.y, Temp.z));
		Corr -= m_Lamda * GradC_j;
	}

	m_DeltaPosition = glm::vec3(Corr.x(), Corr.y(), Corr.z());
}

//********************************************************************
//FUNCTION:
float hiveFluidSimulation::CFluidParticle::__calSCorr(const glm::vec3& vPositionJ)
{
	float Corr = CSolverFunction::calPoly6(glm::vec3(m_NewPosition), vPositionJ) / g_WDeltaQH;
	return -g_Constant_K * pow(Corr, g_CorrectTime);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__applyDeltaP()
{
	m_NewPosition += glm::vec4(m_DeltaPosition, m_NewPosition.w);
}

//********************************************************************
//FUNCTION:
glm::vec3 hiveFluidSimulation::CFluidParticle::__calVorticityForce()
{
	glm::vec3 Omega;

	for (unsigned int i=0; i<m_FluidNeighborSet.size(); ++i)
	{
		glm::vec3 RelativeVelocity = m_FluidNeighborSet[i]->getVelocity() - m_Velocity;
		glm::vec3 Gradient         = CSolverFunction::calSpiky(glm::vec3(m_NewPosition), glm::vec3(m_FluidNeighborSet[i]->getNewPosition()));
		Omega += glm::cross(RelativeVelocity, Gradient);
	}

	float OmegaLength = length(Omega);
	if (OmegaLength == 0.0f)
	{
		return glm::vec3(0.0f);
	}

	glm::vec3 EtaVal = __calEta(OmegaLength);
	if (EtaVal.x == 0 && EtaVal.y == 0 && EtaVal.z == 0)
	{
		return glm::vec3(0.0f);
	}
	glm::vec3 N = normalize(EtaVal);

	return (glm::cross(N, Omega) * g_VorticityEps);
}

//********************************************************************
//FUNCTION:
glm::vec3 hiveFluidSimulation::CFluidParticle::__calXsphViscosity()
{
	glm::vec3 Viscosity;
	for (unsigned int i=0; i<m_FluidNeighborSet.size(); ++i)
	{
		glm::vec3 RelativeVelocity = m_FluidNeighborSet[i]->getVelocity() - m_Velocity;
		Viscosity += RelativeVelocity * CSolverFunction::calPoly6(glm::vec3(m_NewPosition), glm::vec3(m_FluidNeighborSet[i]->getNewPosition()));
	}

	return Viscosity * g_Constant_C;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__updateVelocity()
{
	glm::vec4 DeltaP = (m_NewPosition - m_OldPosition);
	m_Velocity.x = DeltaP.x / g_DeltaT;
	m_Velocity.y = DeltaP.y / g_DeltaT;
	m_Velocity.z = DeltaP.z / g_DeltaT;

	std::string ParticleInitMethod = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL);
	if (hiveFluidSimulation::INITIALMETHOD::BOX == strupr(const_cast<char*>(ParticleInitMethod.c_str())))
	{
		__confineToBox();
	}

	m_OldPosition = m_NewPosition;
}

//********************************************************************
//FUNCTION:
glm::vec3 hiveFluidSimulation::CFluidParticle::__calEta(float vVorticityMag)
{
	glm::vec3 Eta;
	for (unsigned int i=0; i<m_FluidNeighborSet.size(); ++i)
	{
		Eta += CSolverFunction::calSpiky(glm::vec3(m_NewPosition), glm::vec3(m_FluidNeighborSet[i]->getNewPosition())) * vVorticityMag;
	}

	return Eta;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::computeXSPHViscosity()
{
	// Compute viscosity forces (XSPH)
#pragma omp parallel default(shared)
	{
#pragma omp for schedule(static)
		for (int i=0; i<m_FluidNeighborSet.size(); ++i)
		{
			glm::vec4 NeighborPos = m_FluidNeighborSet[i]->getNewPosition();
			glm::vec3 NeighborVel = m_FluidNeighborSet[i]->getVelocity();
			float NeighborDensity = dynamic_cast<CFluidParticle*>(m_FluidNeighborSet[i])->getDensity();

			glm::vec3 VelTemp = m_Velocity - NeighborVel;
			glm::vec4 PosTemp = m_NewPosition - NeighborPos;

			float T1 = 0.0f;
			if (NeighborDensity != 0.0f)
			{
				T1 = g_Viscosity * (m_FluidNeighborSet[i]->getMass() / NeighborDensity);
			}

			Eigen::Matrix<float, 3, 1> Temp = T1 * Eigen::Matrix<float, 3, 1>(VelTemp.x, VelTemp.y, VelTemp.z) * (float)CCubicKernel::W(Eigen::Matrix<float, 3, 1>(PosTemp.x, PosTemp.y, PosTemp.z)); 
			m_Velocity -= glm::vec3(Temp.x(), Temp.y(), Temp.z());

			std::string ParticleInitMethod = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL);
			if (hiveFluidSimulation::INITIALMETHOD::BOX == strupr(const_cast<char*>(ParticleInitMethod.c_str())))
			{
				__confineToBox();
			}
		}
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__predictInitialVelocity(const glm::vec3& vExtAcceleration)
{
	m_Velocity += g_DeltaT * vExtAcceleration;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__predictInitialPosition()
{
	glm::vec3 Temp = g_DeltaT * m_Velocity;
	m_NewPosition.x += Temp.x;
	m_NewPosition.y += Temp.y;
	m_NewPosition.z += Temp.z;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__confineToBox()
{
	glm::vec3 RangeMin = CRunConfig::getInstance()->getLeftDownPoint();
	glm::vec3 RangeMax = CRunConfig::getInstance()->getRightUpPoint();

	if (m_NewPosition.x < RangeMin.x)
	{
		m_Velocity.x    = 0.0;
		m_NewPosition.x = RangeMin.x + 0.001;
	}else if (m_NewPosition.x > RangeMax.x)
	{
		m_Velocity.x    = 0.0;
		m_NewPosition.x = RangeMax.x - 0.001;
	}

	if (m_NewPosition.y < RangeMin.y)
	{
		m_Velocity.y    = 0.0;
		m_NewPosition.y = 0.001;
	}else if (m_NewPosition.y > RangeMax.y)
	{
		m_Velocity.y    = 0.0;
		m_NewPosition.y = RangeMax.y - 0.001;
	}

	if (m_NewPosition.z < RangeMin.z)
	{
		m_Velocity.z    = 0.0;
		m_NewPosition.z = RangeMin.z + 0.001;
	}else if (m_NewPosition.z > RangeMax.z)
	{
		m_Velocity.z    = 0.0;
		m_NewPosition.z = RangeMax.z - 0.001;
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::predictInfoV()
{
	__predictInitialVelocity(g_Gravity);
	__predictInitialPosition();

	std::string ParticleInitMethod = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL);
	if (hiveFluidSimulation::INITIALMETHOD::BOX == strupr(const_cast<char*>(ParticleInitMethod.c_str())))
	{
		__confineToBox();
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::correctInfoV()
{
	__calDensities();
	__calLambda();
	__calDeltP();
	__applyDeltaP();
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::updateInfoV()
{
	__updateVelocity();
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidParticle::__calAccelerationFromRigid()
{
	if (m_RigidNeighborSet.size() <= 0)		
	{
		m_AccelerationFromRigid = g_Gravity;
		return;
	}	

	if (length(m_AccelerationFromRigid) == 0.0f)
	{
		m_Velocity = -0.5f * m_Velocity;
	}

	m_AccelerationFromRigid = m_AccelerationFromRigid = -g_Gravity;
	float Density      = m_Density;
	float Contribution = 0.0f;

	std::vector<CParticle*> TotalParticle;
	CScene::getInstance()->dumpTotalParticle(TotalParticle);

	for (auto Rigid : m_RigidNeighborSet)
	{
		std::vector<CParticle*> FluidParticleNeighbors, ObstacleParticleNeighbors;
		CScene::getInstance()->dumpNeighbors(Rigid, TotalParticle, g_Radius, FluidParticleNeighbors, ObstacleParticleNeighbors);
		Rigid->setFluidNeighbors(FluidParticleNeighbors);
		Rigid->setObstacleNeighbors(ObstacleParticleNeighbors);
		
		dynamic_cast<CRigidParticle*>(Rigid)->calVolume();
		Contribution = g_RestDensityOfRigid * dynamic_cast<CRigidParticle*>(Rigid)->getVolume();
		Density += Contribution * CSolverFunction::calPoly6(glm::vec3(m_NewPosition), glm::vec3(Rigid->getNewPosition())); 
	}
	float Pressure = g_ParticalStiffness * (Density - g_Density0);
	
	for (auto Rigid : m_RigidNeighborSet)
	{
		Contribution = g_RestDensityOfRigid * dynamic_cast<CRigidParticle*>(Rigid)->getVolume();
		m_AccelerationFromRigid += Contribution * (Pressure / pow(Density, 2)) * CSolverFunction::calSpiky(glm::vec3(Rigid->getNewPosition()), glm::vec3(m_NewPosition));
	}
}