/*
    epoc sdl.cpp
    Symbian OS services for SDL

    Markus Mertama
*/

#ifndef EPOCSDL_H
#define EPOCSDL_H


#include<e32def.h>

#include<e32std.h>
#include<gdi.h>
#include<w32std.h>
#include"SDL_config_symbian.h"

#define DEBUG_TRACE_ENABLED

//#undef DEBUG_TRACE_ENABLED
#define TRACE_TO_FILE
#ifdef DEBUG_TRACE_ENABLED
#define SDL_TRACE(error_str) RDebug_Print(error_str, (void*) NULL) 
#define SDL_TRACE1(error_str, param) RDebug_Print(error_str, (void*) (param)) 
#define SDL_MEM() SDL_TRACE1("Free memory=%d", Debug_AvailMem()) 
extern "C"
    {
    void RDebug_Print(char* error_str, void* param);
    void RDebug_Print_b(char* error_str, void* param);
    int Debug_AvailMem();
    }
#else
#define SDL_TRACE(error_str) 
#define SDL_TRACE1(error_str, param) 
#define SDL_MEM()
#endif

#define PANIC(x) Panic(x, __LINE__)
#define PANIC_IF_ERROR(x)  (x == KErrNone || Panic(x, __LINE__))

TInt Panic(TInt aErr, TInt aLine);


NONSHARABLE_CLASS(MEventQueue)
        {
        public:
            virtual TInt Append(const TWsEvent& aEvent) = 0;
           // virtual const TWsEvent& Top() = 0;
            virtual TBool HasData() = 0;
           // virtual TInt Shift() = 0;
           virtual const TWsEvent& Shift() = 0;
       	   virtual void Lock() = 0;
       	   virtual void Unlock() = 0;
        };
 

//class CEikonEnv;

typedef void (*TSdlCleanupOperation) (TAny* aThis);

NONSHARABLE_CLASS(TSdlCleanupItem)
    {
    public:
        TSdlCleanupItem(TSdlCleanupOperation aOperation, TAny* aItem);
    public:
        TSdlCleanupOperation iOperation;
        TAny* iItem;
        TThreadId iThread;
    };

 
NONSHARABLE_CLASS(EpocSdlEnv)
    {
    public:
    enum {EDisableKeyBlocking = 100, ELastService};
    static void Request(TInt aService);
    static MEventQueue& EventQueue();
 // 	static CEikonEnv& EikonEnv();
   	static TSize WindowSize(const TSize& aRequestedSize);
   	static TSize WindowSize();
    static TDisplayMode DisplayMode();
   	static TPointerCursorMode PointerMode();
    static TBool Flags(TInt aFlag);
    static TInt Argc();
    static char** Argv();
    static TBool IsDsaAvailable();
   // static void Dsa(const TPoint& aPoint, const CFbsBitmap& aBmp);
    static TInt AllocSwSurface(const TSize& aSize, TDisplayMode aMode);
    static TInt AllocHwSurface(const TSize& aSize, TDisplayMode aMode);
    static void UnlockHwSurface();
    static TUint8* LockHwSurface(); 
    static void UpdateSwSurface();
    static TBool AddUpdateRect(TUint8* aAddress, const TRect& aUpdateRect, const TRect& aTargetRect);
    static void PanicMain(TInt aErr);
    static void PanicMain(const TDesC& aInfo, TInt aErr);
    static void WaitDeviceChange();
    static TInt SetPalette(TInt aFirstcolor, TInt aColorCount, TUint32* aPalette);
    static void ObserverEvent(TInt aService, TInt aParam = 0);
    static void WaitDsaAvailable();
    static void LockPalette(TBool aLock);
    static void Resume();
    static void Suspend();
    static TInt AppendCleanupItem(const TSdlCleanupItem& aItem);
    static void RemoveCleanupItem(TAny* aItem);
    static void CleanupItems();
    static void SetWaitDsa();
    static TPoint WindowCoordinates(const TPoint& aPoint);
    static void FreeSurface();
   // static TInt SetVolume(TInt aVolume);
   // static TInt Volume();
   // static TInt MaxVolume();
    }; 
    
#endif


