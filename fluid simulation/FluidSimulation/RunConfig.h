#pragma once
#include <string>
#include "common/HiveConfig.h"
#include "common/ConfigParser.h"
#include "common/Singleton.h"
#include "Common.h"

namespace hiveFluidSimulation
{
	namespace CONFIG_KEYWORD
	{
		const std::string NUMFRAME             = "NumFrame";
		const std::string BOXLEFTDOWN          = "BoxLeftDown";
		const std::string BOXRIGHTUP           = "BoxRightUp";
		const std::string PARTICLESPACING      = "ParticleSpacing";
		const std::string PARTICLEINITIAL      = "ParticleInitial";
		const std::string CONTAINERMODELFILE   = "ContainerModelFile";
		const std::string FLUIDMODELFILE       = "FluidModelFile";
		const std::string FINDNEIGHBORMETHOD   = "FindNeighborsMethod";
		const std::string DIVIDERANGE          = "DivideRange";
		const std::string UNITLENGTH           = "UnitLength";
	}

	class CRunConfig : public hiveConfig::CHiveConfig, public hiveCommon::CSingleton<CRunConfig>
	{
	public:
		virtual ~CRunConfig();

		virtual bool parseV(const std::string& vRunFile) {return hiveConfig::hiveParseConfig(vRunFile, hiveConfig::CONFIG_XML, this);}

		glm::vec3 getLeftDownPoint() const {return __getPoint(CONFIG_KEYWORD::BOXLEFTDOWN);} 
		glm::vec3 getRightUpPoint()  const {return __getPoint(CONFIG_KEYWORD::BOXRIGHTUP);}
		glm::vec3 getDivideRange()   const {return __getPoint(CONFIG_KEYWORD::DIVIDERANGE);}

	protected:
		CRunConfig();

		bool m_IsGridInfoSpecified;

		virtual void _onLoadedV() override;
		virtual bool _verifyConfigV() const override;
		glm::vec3 __getPoint(const std::string& vName) const;

		friend class hiveCommon::CSingleton<CRunConfig>;
	};
}