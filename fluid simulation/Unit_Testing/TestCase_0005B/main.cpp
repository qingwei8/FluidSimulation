#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "FluidSimulationInterface.h"
#include <time.h>
#include <ctime>

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 23404;
#endif
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		time_t CurrentTime = time(NULL);
		time_t NowTime;
		time(&NowTime);
		hiveCommon::hiveLoadTestingDLLs();

		hiveFluidSimulation::initSimulation("RunInfo.xml");
		hiveFluidSimulation::doSimulation();

		time_t CurrentTime1 = time(NULL);
		time_t NowTime1;
		time(&NowTime1);

		std::cout << CurrentTime1 - CurrentTime << "s" << " " << (CurrentTime1 - CurrentTime)/60 << "min" << std::endl;
		std::cout << NowTime1 - NowTime << "s" << " " << (NowTime1 - NowTime)/60 << "min" << std::endl;
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error.");
	}

	system("pause");
}