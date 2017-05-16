#include "stdafx.h"
#include "CppUnitTest.h"
#include <glm/glm.hpp>
#include <string>
#include "../FluidSimulation/FluidSimulationInterface.h"
#include "../FluidSimulation/Particle.h"
#include "../FluidSimulation/BaseFindNeighbors.h"
#include "../FluidSimulation/GridFindNeighbors.h"
#include "../FluidSimulation/GridManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hiveFluidSimulation;
   
namespace UnitTest1
{		
	bool isEqualwith2Vec3(const glm::vec3 vPos1, const glm::vec3 vPos2)
	{
		if(vPos1 == vPos2)
			return true;
	}

	TEST_CLASS(UnitTest1)
	{
	public:
		void initParticleSet(std::vector<CParticle*>& voParticleSet)
		{
			for(int i=0; i<20; ++i)
			{
				CParticle* pParticle = new CParticle();
				( 10!= i) ? pParticle->setPosition(glm::vec4(i, i, i, 1)) : pParticle->setPosition(glm::vec4(2, 1, 3, 0));
				voParticleSet.push_back(pParticle);
			}

			CParticle* pObstacleParticle = new CParticle();
			pObstacleParticle->setPosition(glm::vec4(1, 2, 1, 0));
			voParticleSet.push_back(pObstacleParticle);
		}

		TEST_METHOD(testCalculateDistance)
		{
			float DistanceResult = hiveFluidSimulation::calculateDistance(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
			float Test   = 1.0f;
			Assert::AreEqual(DistanceResult, Test, false);
		};

		TEST_METHOD(testBaseFindNeighbors)
		{
			float Radius=4.0;
			std:: vector<CParticle*> ParticleSet, FluidParticleSet, ObstacleParticleSet;
		    initParticleSet(ParticleSet);

			CBaseFindNeighbors* pFindNeighbor = new CBaseFindNeighbors(); 
			hiveFluidSimulation::testFindNeighbors(pFindNeighbor, ParticleSet[10], Radius, ParticleSet, FluidParticleSet, ObstacleParticleSet);

			////测试邻居粒子为刚体粒子
			bool Test0 = true;
			bool TestSrc0 = isEqualwith2Vec3(glm::vec3(1, 2, 1), glm::vec3(ObstacleParticleSet[0]->getNewPosition()));
			Assert::AreEqual(Test0, TestSrc0, false);

			//测试邻居粒子为流体粒子
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(0, 0, 0), glm::vec3(FluidParticleSet[0]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(1, 1, 1), glm::vec3(FluidParticleSet[1]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(2, 2, 2), glm::vec3(FluidParticleSet[2]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(3, 3, 3), glm::vec3(FluidParticleSet[3]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(4, 4, 4), glm::vec3(FluidParticleSet[4]->getNewPosition())), false);
		};

		TEST_METHOD(testGridFindNeighbors)
		{
			std:: vector<CParticle*> ParticleSet, FluidParticleSet, ObstacleParticleSet;
			initParticleSet(ParticleSet);

			glm::vec3 Range = glm::vec3(21, 21, 21);
			float Length = 4.0, Radius=4.0;

			CBaseFindNeighbors* pFindNeighbor = new CGridFindNeighbors(); 
			dynamic_cast<CGridFindNeighbors*>(pFindNeighbor)->prepareV(Range, Length);
			dynamic_cast<CGridFindNeighbors*>(pFindNeighbor)->updateV(ParticleSet);
			hiveFluidSimulation::testFindNeighbors(pFindNeighbor, ParticleSet[10], Radius, ParticleSet, FluidParticleSet, ObstacleParticleSet);

			////测试邻居粒子为刚体粒子
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(1, 2, 1), glm::vec3(ObstacleParticleSet[0]->getNewPosition())), false);
		
			//测试邻居粒子为流体粒子
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(0, 0, 0), glm::vec3(FluidParticleSet[0]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(1, 1, 1), glm::vec3(FluidParticleSet[1]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(2, 2, 2), glm::vec3(FluidParticleSet[2]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(3, 3, 3), glm::vec3(FluidParticleSet[3]->getNewPosition())), false);
			Assert::AreEqual(true, isEqualwith2Vec3(glm::vec3(4, 4, 4), glm::vec3(FluidParticleSet[4]->getNewPosition())), false);
		};
	};

	TEST_CLASS(UnitTest2)
	{
	public:
		void readDataFromFile(const std::string& vFile , std::vector<glm::vec3>& voPositionSet)
		{
			std::ifstream inFile(vFile);
			if(!inFile) std::cout << "open the Input File error!" << std::endl;

			std::string Line; 
			while(!inFile.eof())
			{
				getline(inFile, Line);
				float a, b , c;
				std::istringstream Ss;
				Ss.clear();
				Ss.str(Line);
				Ss>>a>>b>>c;
				if(Ss.fail()) break;
				voPositionSet.push_back(glm::vec3(a, b, c));
			}
		}
		void initPositionSet(std::vector<glm::vec3>& voPositionSet)
		{
			std::string  FileAddress = "..\\UnitTest1\\test.txt";
			readDataFromFile(FileAddress, voPositionSet);
		}

		void initParticleSet(const std::vector<glm::vec3>& vPostionSet, std::vector<CParticle*>& voParticleSet)
		{
			CParticle* pParticleSet =  new CParticle[vPostionSet.size()]();
			unsigned int Index=0;
			for(auto& Position : vPostionSet)
			{
				_ASSERT(Index <= vPostionSet.size());
				CParticle* pParticle = &pParticleSet[Index++];
				pParticle->setPosition(glm::vec4(Position,1));
				voParticleSet.push_back(pParticle);
			}
		}

		bool isEqualWith2Vector(const std::vector<CParticle*>& ParticleSet1, const std::vector<CParticle*>& ParticleSet2)		
		{
			if(ParticleSet1.size() ==0 && ParticleSet2.size()==0) return true;
			if(ParticleSet1.size() != ParticleSet2.size()) return false;
			else
			{
				for(unsigned int i=0; i<ParticleSet1.size(); ++i)
				{
					for(unsigned int k=0; k<ParticleSet2.size();)
					{
						if(isEqualwith2Vec3(glm::vec3(ParticleSet1[i]->getNewPosition()),glm::vec3(ParticleSet2[k]->getNewPosition())))
							break;
						else
							++k;
						if(k==ParticleSet2.size()) return false;
					}
				}
				return true;
			}
			return false;
		}

		TEST_METHOD(testIsEqualBetweenBaseAndGrid)
		{
			std::vector<glm::vec3> PositionSet;
			initPositionSet(PositionSet);

			float Radius=1.0;
			std::vector<CParticle*> ParticleSet;
			initParticleSet(PositionSet, ParticleSet);
			
			//for循环查找每个粒子的邻居			
			CBaseFindNeighbors* pFindNeighbor1 = new CBaseFindNeighbors(); 
			
			std::vector<std::vector<CParticle*>> FluidParticleSet1;
			std::vector<std::vector<CParticle*>> ObstacleParticleSet1;
			FluidParticleSet1.resize(ParticleSet.size());
			ObstacleParticleSet1.resize(ParticleSet.size());
			for(unsigned int i=0; i<ParticleSet.size(); ++i)
			{			
				hiveFluidSimulation::testFindNeighbors(pFindNeighbor1, ParticleSet[i], Radius, ParticleSet, FluidParticleSet1[i], ObstacleParticleSet1[i]);
			}

			//网格法查找每个粒子的邻居
			float Length = 1.2;
			glm::vec3 Range = glm::vec3(20, 20, 30);
			CBaseFindNeighbors* pFindNeighbor2 = new CGridFindNeighbors();
			dynamic_cast<CGridFindNeighbors*>(pFindNeighbor2)->prepareV(Range, Length);
			dynamic_cast<CGridFindNeighbors*>(pFindNeighbor2)->updateV(ParticleSet);
			
			std::vector<std::vector<CParticle*>> FluidParticleSet2;
			std::vector<std::vector<CParticle*>> ObstacleParticleSet2;
			FluidParticleSet2.resize(ParticleSet.size());
			ObstacleParticleSet2.resize(ParticleSet.size());
			for(unsigned int i=0; i<ParticleSet.size(); ++i)
			{
				hiveFluidSimulation::testFindNeighbors(pFindNeighbor2, ParticleSet[i], Radius, ParticleSet, FluidParticleSet2[i], ObstacleParticleSet2[i]);
			}

			//判断每个粒子使用for循环法与网格法查找的邻居粒子是否相等	
			for(unsigned int i=0; i<ParticleSet.size(); ++i)
			{
				bool Test = isEqualWith2Vector(FluidParticleSet1[i], FluidParticleSet2[i]);
				if(false == Test)
				{
					std::cout << "i:" << i << std::endl;
				}
				Assert::AreEqual(true, Test, false);
				Assert::AreEqual(true, isEqualWith2Vector(ObstacleParticleSet1[i], ObstacleParticleSet2[i]), false);
			}
		};
	};
}