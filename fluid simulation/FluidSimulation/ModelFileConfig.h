 #pragma once
 #include <string>
 #include "common/HiveConfig.h"
 #include "common/ConfigParser.h"
 #include "common/Singleton.h"
 
 namespace hiveFluidSimulation
 {
 	const std::string MODELPATH = "ModelPath";
 
 	class CModelFileConfig : public hiveConfig::CHiveConfig, public hiveCommon::CSingleton<CModelFileConfig>
 	{
 	public:
 		~CModelFileConfig();
 
 		virtual bool parseV(const std::string& vModelFile) {return hiveParseConfig(vModelFile, hiveConfig::CONFIG_XML, this);}
 
 	protected:
 		CModelFileConfig();
 
 		friend class hiveCommon::CSingleton<CModelFileConfig>;
 	};
 
 	CModelFileConfig::CModelFileConfig()
 	{
 		defineAttribute(MODELPATH, hiveConfig::ATTRIBUTE_STRING);
 	}
 
 	CModelFileConfig::~CModelFileConfig()
 	{
 	}
 }