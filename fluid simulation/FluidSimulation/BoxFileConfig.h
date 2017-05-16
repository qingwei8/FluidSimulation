 #pragma once
 #include <glm/glm.hpp>
 #include "common/HiveConfig.h"
 #include "common/ConfigParser.h"
 #include "common/Singleton.h"
 
 namespace hiveFluidSimulation
 {
 	const std::string LEFTDOWN = "LeftDown";
 	const std::string RIGHTUP  = "RightUp";
 
 	class CBoxFileConfig : public hiveConfig::CHiveConfig, public hiveCommon::CSingleton<CBoxFileConfig>
 	{
 	public:
 		~CBoxFileConfig();
 		virtual bool parseV(const std::string& vBoxFile) {return hiveParseConfig(vBoxFile, hiveConfig::CONFIG_XML, this);}
 
 		glm::vec3 getLeftDownPoint() const;
 		glm::vec3 getRightUpPoint() const;
 
 	protected:
 		CBoxFileConfig();
 		glm::vec3 __getPoint(const std::string& vName) const;
 
 		friend class hiveCommon::CSingleton<CBoxFileConfig>;
 	};
 
 	CBoxFileConfig::CBoxFileConfig()
 	{
 		defineAttribute(LEFTDOWN, hiveConfig::ATTRIBUTE_VEC3F);
 		defineAttribute(RIGHTUP,  hiveConfig::ATTRIBUTE_VEC3F);
 	}
 
 	CBoxFileConfig::~CBoxFileConfig()
 	{
 	}
 
 	glm::vec3 CBoxFileConfig::getLeftDownPoint() const
 	{
 		return __getPoint(LEFTDOWN);
 	}
 
 	glm::vec3 CBoxFileConfig::getRightUpPoint() const
 	{
 		return __getPoint(RIGHTUP);
 	}
 
 	glm::vec3 CBoxFileConfig::__getPoint(const std::string& vName) const
 	{
 		_ASSERT(!vName.empty());
 		boost::tuple<double, double, double> Point = getAttribute<boost::tuple<double, double, double>>(vName);
 		return glm::vec3(Point.get<0>(), Point.get<1>(), Point.get<2>());
 	}
 }