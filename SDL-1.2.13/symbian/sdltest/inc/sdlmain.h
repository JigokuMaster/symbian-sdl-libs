#ifndef __SDLMAIN_H__
#define __SDLMAIN_H__

class CApaApplication;

#include<sdlepocapi.h>

NONSHARABLE_CLASS(MSDLMainObs)
	{
public:
	enum TSDLMainObsEvent {EError, ESDLCreated};
	virtual TInt SDLMainEvent(TInt aEvent, TInt aParam, CSDL* aSdl) = 0;
	};

NONSHARABLE_CLASS(SDLEnv)
	{
public:
	enum TSdlEnvFlags
		{
		EFlagsNone 			= 0x0,
		EParamQuery 		= 0x1,
		EAllowConsoleView 	= 0x2,
		EVirtualMouse		= 0x4,
		EParamQueryDialog	= 0x9
		};
	IMPORT_C static TInt SetMain(const TMainFunc& aFunc, TInt aSdlFlags, MSDLMainObs* aObs = NULL, TInt aEnvFlags = EFlagsNone);
 	};



#endif