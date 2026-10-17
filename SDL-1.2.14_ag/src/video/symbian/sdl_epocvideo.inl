
#if 0
inline void EPOC_UpdateNative(_THIS, TInt numrects, SDL_Rect *rects)
{
    // 1. Lock screen bitmap heap
    TBool lockedHeap = EFalse;
    TBitmapUtil lock(Private->EPOC_Bitmap);	
    if (!gHeapIsLocked) {
        lock.Begin(TPoint(0,0));
        lockedHeap = ETrue;
    }

    TUint16* targetBuffer = (TUint16*)Private->EPOC_Bitmap->DataAddress();
    const TInt screenW = _this->screen->w;
    const TInt screenH = _this->screen->h;
    const TInt targetPitch = Private->EPOC_ScreenSize.iWidth;
    const TBool is8Bpp = (_this->screen->format->BitsPerPixel == 8);

    // 2. Process dirty rects
    for (TInt i = 0; i < numrects; ++i) {
        const SDL_Rect& rect = rects[i];

        if (rect.w <= 0 || rect.h <= 0) continue;
        TInt startX = Max(0, (TInt)rect.x);
        TInt startY = Max(0, (TInt)rect.y);
        TInt endX = Min(screenW, rect.x + rect.w);
        TInt endY = Min(screenH, rect.y + rect.h);
        
        TInt copyWidth = endX - startX;
        TInt copyHeight = endY - startY;
        if (copyWidth <= 0 || copyHeight <= 0) continue;

        // -------------------------------------------------------------
        // PATH A: 8 BPP (Indexed Palette -> 16-Bit Display Mode)
        // -------------------------------------------------------------
        if (is8Bpp) {
            const TUint8* srcLine = (const TUint8*)_this->screen->pixels + (startY * screenW) + startX;
            TUint16* dstLine = targetBuffer + (startY * targetPitch) + startX;

            for (TInt y = 0; y < copyHeight; ++y) {
                const TUint8* srcPtr = srcLine;
                TUint16* dstPtr = dstLine;
                TInt width = copyWidth;

                // Unrolled x2 for ARM pipeline optimization
                while (width >= 2) {
                    dstPtr[0] = EPOC_HWPalette_256_to_DisplayMode[srcPtr[0]];
                    dstPtr[1] = EPOC_HWPalette_256_to_DisplayMode[srcPtr[1]];
                    srcPtr += 2;
                    dstPtr += 2;
                    width -= 2;
                }
                if (width > 0) {
                    *dstPtr = EPOC_HWPalette_256_to_DisplayMode[*srcPtr];
                }

                srcLine += screenW;
                dstLine += targetPitch;
            }
        }
        // -------------------------------------------------------------
        // PATH B: 16 BPP (Direct Pixel Copy)
        // -------------------------------------------------------------
        else {
            const TUint16* srcLine = (const TUint16*)_this->screen->pixels + (startY * screenW) + startX;
            TUint16* dstLine = targetBuffer + (startY * targetPitch) + startX;
            const TInt lineByteSize = copyWidth * sizeof(TUint16);

            for (TInt y = 0; y < copyHeight; ++y) {
                Mem::Copy(dstLine, srcLine, lineByteSize);
                srcLine += screenW;
                dstLine += targetPitch;
            }
        }

        // Push region to Window Server
        TRect realRect(startX, startY, endX, endY);
        Private->iWindowCreator->PutBitmap(Private->EPOC_Bitmap, realRect.iTl, realRect);
    }

    // 3. Cleanup & Hardware Flush
    if (lockedHeap) {
        lock.End();
        gHeapIsLocked = EFalse;
    }

    Private->iWindowCreator->UpdateScreen();
#if defined (__WINS__)
    Private->iEikEnv->WsSession().Flush();
#endif
}

#endif


#if 0
inline void EPOC_UpdateNative(_THIS, TInt numrects, SDL_Rect *rects)
{
    if (!gViewVisible || !_this->screen || !_this->screen->pixels)
        return;

    /* Lock screen bitmap heap */
    TBool lockedHeap = EFalse;
    TBitmapUtil lock(Private->EPOC_Bitmap);	
    if (!gHeapIsLocked) {
        lock.Begin(TPoint(0,0));
        lockedHeap = ETrue;
    }

    TUint16* targetBuffer = (TUint16*)Private->EPOC_Bitmap->DataAddress();

    /* Compute 16-bit word strides using actual memory pitch */
    const TInt srcPitchPixels = _this->screen->pitch / 2;
    const TInt targetPitchPixels = Private->EPOC_Bitmap->DataStride() / 2;

    const TInt screenW = _this->screen->w;
    const TInt screenH = _this->screen->h;
    const TBool is8Bpp = (_this->screen->format->BitsPerPixel == 8);

    /* Process dirty rects */
    for (TInt i = 0; i < numrects; ++i) {
        const SDL_Rect& rect = rects[i];

        if (rect.w <= 0 || rect.h <= 0) continue;
        TInt startX = Max(0, (TInt)rect.x);
        TInt startY = Max(0, (TInt)rect.y);
        TInt endX = Min(screenW, (TInt)(rect.x + rect.w));
        TInt endY = Min(screenH, (TInt)(rect.y + rect.h));

        TInt copyWidth = endX - startX;
        TInt copyHeight = endY - startY;
        if (copyWidth <= 0 || copyHeight <= 0) continue;

        /* PATH A: 16 BPP Direct Copy */
        if (!is8Bpp) {
            const TUint16* srcLine = (const TUint16*)_this->screen->pixels + (startY * srcPitchPixels) + startX;
            TUint16* dstLine = targetBuffer + (startY * targetPitchPixels) + startX;
            const TInt lineByteSize = copyWidth * sizeof(TUint16);

            for (TInt y = 0; y < copyHeight; ++y) {
                Mem::Copy(dstLine, srcLine, lineByteSize);
                srcLine += srcPitchPixels;
                dstLine += targetPitchPixels;
            }
        }
        /* PATH B: 8 BPP Indexed Palette Conversion */
        else {
            const TUint8* srcLine = (const TUint8*)_this->screen->pixels + (startY * _this->screen->pitch) + startX;
            TUint16* dstLine = targetBuffer + (startY * targetPitchPixels) + startX;

            for (TInt y = 0; y < copyHeight; ++y) {
                const TUint8* srcPtr = srcLine;
                TUint16* dstPtr = dstLine;
                for (TInt x = 0; x < copyWidth; ++x) {
                    *dstPtr++ = EPOC_HWPalette_256_to_DisplayMode[*srcPtr++];
                }
                srcLine += _this->screen->pitch;
                dstLine += targetPitchPixels;
            }
        }

        /* Push sub-region directly to window graphics context */
        TRect realRect(startX, startY, endX, endY);
        Private->iWindowCreator->PutBitmap(Private->EPOC_Bitmap, realRect.iTl, realRect);
    }

    /* 5. Cleanup & Flush */
    if (lockedHeap) {
        lock.End();
        gHeapIsLocked = EFalse;
    }

    Private->iWindowCreator->UpdateScreen();
#if defined (__WINS__)
    Private->iEikEnv->WsSession().Flush();
#endif
}

#endif


inline void EPOC_UpdateNative(_THIS, TInt numrects, SDL_Rect *rects)
{
    if (!gViewVisible || !_this->screen || !_this->screen->pixels)
        return;

    /* 1. Fast Thread Check (No object construction) */
    /*if (RThread().Id() != Private->iThreadId) {
        if (numrects <= 20 && !Private->iNeedUpdate) {
            Private->iNumRects = numrects;
            for (TInt loop = 0; loop < numrects; loop++) {
                Private->iDirtyRects[loop] = rects[loop];
            }
        } else {
            SDL_Rect fullScreen = {0, 0, (Uint16)_this->screen->w, (Uint16)_this->screen->h};
            Private->iNumRects = 1;
            Private->iDirtyRects[0] = fullScreen;
        }
        Private->iNeedUpdate = ETrue;
        return;
    }*/


    /* 2. Lock screen bitmap heap once */
    TBool lockedHeap = EFalse;
    TBitmapUtil lock(Private->EPOC_Bitmap);	
    if (!gHeapIsLocked) {
        lock.Begin(TPoint(0,0));
        lockedHeap = ETrue;
    }

    TUint16* targetBuffer = (TUint16*)Private->EPOC_Bitmap->DataAddress();
    const TInt srcPitchPixels = _this->screen->pitch / 2;
    const TInt targetPitchPixels = Private->EPOC_Bitmap->DataStride() / 2;
    const TInt screenW = _this->screen->w;
    const TInt screenH = _this->screen->h;
    const TBool is8Bpp = (_this->screen->format->BitsPerPixel == 8);

    /* Track overall bounding box for a single WSERV blit */
    TInt minX = screenW, minY = screenH, maxX = 0, maxY = 0;
    TBool hasValidRect = EFalse;

    /* 3. Batch copy all dirty regions into bitmap RAM */
    for (TInt i = 0; i < numrects; ++i) {
        const SDL_Rect& rect = rects[i];
        if (rect.w <= 0 || rect.h <= 0) continue;

        TInt startX = Max(0, (TInt)rect.x);
        TInt startY = Max(0, (TInt)rect.y);
        TInt endX = Min(screenW, (TInt)(rect.x + rect.w));
        TInt endY = Min(screenH, (TInt)(rect.y + rect.h));

        TInt copyWidth = endX - startX;
        TInt copyHeight = endY - startY;
        if (copyWidth <= 0 || copyHeight <= 0) continue;

        /* Expand overall dirty bounding rectangle */
        if (startX < minX) minX = startX;
        if (startY < minY) minY = startY;
        if (endX > maxX)   maxX = endX;
        if (endY > maxY)   maxY = endY;
        hasValidRect = ETrue;

        /* PATH A: 16 BPP Direct Copy */
        if (!is8Bpp) {
            const TUint16* srcLine = (const TUint16*)_this->screen->pixels + (startY * srcPitchPixels) + startX;
            TUint16* dstLine = targetBuffer + (startY * targetPitchPixels) + startX;
            const TInt lineByteSize = copyWidth * sizeof(TUint16);

            for (TInt y = 0; y < copyHeight; ++y) {
                Mem::Copy(dstLine, srcLine, lineByteSize);
                srcLine += srcPitchPixels;
                dstLine += targetPitchPixels;
            }
        }
        /* PATH B: 8 BPP Indexed Palette (Unrolled x2 for ARM pipeline) */
        else {
            const TUint8* srcLine = (const TUint8*)_this->screen->pixels + (startY * _this->screen->pitch) + startX;
            TUint16* dstLine = targetBuffer + (startY * targetPitchPixels) + startX;

            for (TInt y = 0; y < copyHeight; ++y) {
                const TUint8* srcPtr = srcLine;
                TUint16* dstPtr = dstLine;
                TInt width = copyWidth;

                while (width >= 2) {
                    dstPtr[0] = EPOC_HWPalette_256_to_DisplayMode[srcPtr[0]];
                    dstPtr[1] = EPOC_HWPalette_256_to_DisplayMode[srcPtr[1]];
                    srcPtr += 2;
                    dstPtr += 2;
                    width -= 2;
                }
                if (width > 0) {
                    *dstPtr = EPOC_HWPalette_256_to_DisplayMode[*srcPtr];
                }

                srcLine += _this->screen->pitch;
                dstLine += targetPitchPixels;
            }
        }
    }

    /* 4. Unlock heap before IPC call */
    if (lockedHeap) {
        lock.End();
        gHeapIsLocked = EFalse;
    }

    /* 5. Issue SINGLE PutBitmap & Screen Update for the bounding region */
    if (hasValidRect) {
        TRect dirtyBound(minX, minY, maxX, maxY);
        Private->iWindowCreator->PutBitmap(Private->EPOC_Bitmap, dirtyBound.iTl, dirtyBound);
        Private->iWindowCreator->UpdateScreen();
    }

#if defined (__WINS__)
    Private->iEikEnv->WsSession().Flush();
#endif
}
