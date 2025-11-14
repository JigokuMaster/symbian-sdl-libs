#ifndef __DSA_H__
#define __DSA_H__

#include <e32base.h>
#include <w32std.h>
#include "epoc_sdl.h"
#include "sdlepocapi.h"

class CDsa;

NONSHARABLE_CLASS(TOverlay) 
	{
	public:
		MOverlay* iOverlay;
		TInt iPriority;
	};

typedef void (*TCopyFunction)(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);



NONSHARABLE_CLASS(CDsa) : public CBase
	{
	public:
		enum
		{
		ERequestUpdate = EpocSdlEnv::ELastService,
		ELastDsaRequest
		};
		
		inline TBool IsDsaAvailable() const;
   	
		static CDsa* CreateL(RWsSession& aSession);
	
		~CDsa();
   		 	
   		TUint8* LockHwSurface();
   		TInt AllocSurface(TBool aHwSurface, const TSize& aSize, TDisplayMode aMode);
   		inline TDisplayMode DisplayMode() const;
   		
   		TInt SetPalette(TInt aFirst, TInt aCount, TUint32* aPalette);
   		void LockPalette(TBool aLock);
   		TBool AddUpdateRect(const TUint8* aBits, const TRect& aUpdateRect, const TRect& aRect);
   		void UpdateSwSurface();
   		void SetOrientation(CSDL::TOrientationMode aMode);
   		void Wipe();
   		TSize WindowSize() const;
   		void SetSuspend();
   	
   	
   		TBool Stopped() const;
   		RWsSession& Session();
   	
   		void DoStop();
   		
   		void CreateZoomerL(const TSize& aSize);
   		void SetBlitter(MBlitter* aBlitter);
   		
   		TInt AppendOverlay(MOverlay& aOverlay, TInt aPriority);
   		TInt RemoveOverlay(MOverlay& aOverlay);
   		
   		TPoint WindowCoordinates(const TPoint& aPoint) const;
   		
   		virtual void RecreateL();
   		virtual void Free();
   		
   		TInt RedrawRequest();

   	public:
		virtual void ConstructL(RWindow& aWindow, CWsScreenDevice& aDevice);
		virtual void SetUpdating(TBool aUpdate);
		
		virtual TUint8* LockSurface() = 0;
		virtual void UnlockHWSurfaceRequestComplete() = 0;
		virtual void UnlockHwSurface() = 0;
		virtual void Resume() = 0;
		virtual void Stop();
		
   
   	private:
		virtual void CreateSurfaceL() = 0;
		virtual void Wipe(TInt aLength) = 0;
		virtual TInt ExternalUpdate() = 0;
		virtual CBitmapContext& Gc() = 0;

	private:
		void ClipCopy(TUint8* aTarget, const TUint8* aSource, const TRect& aUpdateRect, const TRect& aSourceRect) const;
		static void Copy256(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopySlow(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopyMem(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void Copy256Flip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopyMemFlip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopySlowFlip(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void Copy256Reversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopySlowReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void CopyMemReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt);
		static void Copy256FlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopySlowFlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);
		static void CopyMemFlipReversed(const CDsa& aDsa, TUint32* aTarget, const TUint8* aSource, TInt aBytes, TInt aLineLen);

		void SetCopyFunction();
	
	
   	
	protected:

		inline const TSize& SwSize() const;
		inline const TRect& HwRect() const;
		
		inline const TRect& ScreenRect() const;
		

		TBool Blitter(CFbsBitmap& aBmp);
		void DrawOverlays();	
		CDsa(RWsSession& aSession);
		void SetTargetRect();
		void Start();
	
	protected:
		enum
			{
			ERunning 				= 0x1,
			EUpdating  				= 0x2,
			EPaletteLocked 			= 0x4,
			EUsePalette				= 0x8,
			EOrientation90			= 0x10,
			EOrientation180			= 0x20,
			EOrientationFlags 		= 0x30,
			EOrientationChanged 	= 0x40,
			ESdlThreadSuspend		= 0x100,
			ESdlThreadExplicitStop 	= 0x200,
			EResizeRequest			= 0x400
			};
	
	
		
	private:	
	
		TInt iStateFlags;	
		RWsSession& iSession;	
        TRect iScreenRect;	

		TInt iTargetBpp;
		TInt iSourceBpp;
      	TDisplayMode iSourceMode;
        TDisplayMode iTargetMode;
        TUint32* iLut256;
        TCopyFunction iCopyFunction;
        TUint8* iTargetAddr;
        TCopyFunction iCFTable[12];
        TInt iNewFlags;
        TSize iSwSize;
        MBlitter* iBlitter;
        TRect iTargetRect;
        RArray<TOverlay> iOverlays;
        friend class TDsa;
	};
	
inline TDisplayMode CDsa::DisplayMode() const
	{
	return iTargetMode;
	}

inline const TRect& CDsa::HwRect() const
	{
	return iTargetRect;
	}
	
inline const TSize& CDsa::SwSize() const
	{
	return iSwSize;
	}		
	
inline TBool CDsa::IsDsaAvailable() const
	{
	return iStateFlags & ERunning;
	}
	

	
inline const TRect& CDsa::ScreenRect() const
	{
	return iScreenRect;		
	}
	
#endif	
		
////////////////////////////////////////////////////////////////////////////////////////////////

