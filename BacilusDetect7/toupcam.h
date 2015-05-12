#ifndef __toupcam_h__
#define __toupcam_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ITOUPCAM_DEFINED__
#define __ITOUPCAM_DEFINED__
	/* Both the DirectShow source filter and the output pin support this interface.
		That is to say, you can use QueryInterface on the filter or the pin to get the IToupcam interface. */
	// {66B8EAB5-B594-4141-95FC-691A81B445F6}
	DEFINE_GUID(IID_IToupcam, 0x66b8eab5, 0xb594, 0x4141, 0x95, 0xfc, 0x69, 0x1a, 0x81, 0xb4, 0x45, 0xf6);

	typedef void (CALLBACK* PITOUPCAM_EXPOSURE_CALLBACK)(void* pCtx);
	typedef void (CALLBACK* PITOUPCAM_WHITEBALANCE_CALLBACK)(const int aGain[3], void* pCtx);
	typedef void (CALLBACK* PITOUPCAM_HISTOGRAM_CALLBACK)(const double aHistY[256], const double aHistR[256], const double aHistG[256], const double aHistB[256], void* pCtx);
	typedef void (CALLBACK* PITOUPCAM_CHROME_CALLBACK)(void* pCtx);
    DECLARE_INTERFACE_(IToupcam, IUnknown)
    {
    	/*
    		put_Size, put_eSize, can be used to set the video output resolution BEFORE the DirectShow source filter is connected.
    		put_Size use width and height parameters, put_eSize use the index parameter.
    		for example, UCMOS03100KPA support the following resolutions:
    				index 0: 	2048,	1536
					index 1:	1024,	768
					index 2:	680,	510
    		so, we can use put_Size(1024, 768) or put_eSize(1). Both have the same effect.
    		
  			------------------------------------------------------------|
  			| Parameter			|	Range		|	Default				|
  			|-----------------------------------------------------------|
			| AutoExpoTarget	|	4~255		|	120					|
			| Temp				|	2000~15000	|	6503				|
			| Tint				|	200~2500	|	1000				|
			| LevelRange 		|	0~255		|	Low = 0, High = 255	|
			| Contrast			|	-100~100	|	0					|
			| Hue				|	-180~180	|	0					|
			| Saturation		|	0~255		|	128					|
			| Brightness 		|	-64~64		|	0					|
			| Gamma 			|	20~180		|	100					|
			| VidgetAmount		|	-100~100	|	0					|
			| VignetMidPoint	|	0~100		|	50					|
			-------------------------------------------------------------
    	*/
		STDMETHOD(put_Size) (THIS_
					LONG nWidth, LONG nHeight) PURE;

		STDMETHOD(get_Size) (THIS_
					LONG* pWidth, LONG* pHeight) PURE;

		STDMETHOD(put_eSize) (THIS_
					ULONG nIndex) PURE;

		STDMETHOD(get_eSize) (THIS_
					ULONG* pIndex) PURE;

		STDMETHOD(LevelRangeAuto) (THIS) PURE;

		STDMETHOD(GetHistogram) (THIS_
					PITOUPCAM_HISTOGRAM_CALLBACK fnHistogramProc, void* pHistogramCtx
					) PURE;
			
		STDMETHOD(put_ExpoCallback) (THIS_
					PITOUPCAM_EXPOSURE_CALLBACK fnExpoProc, void* pExpoCtx
					) PURE;

		STDMETHOD(get_AutoExpoEnable) (THIS_
					BOOL* bAutoExposure
					) PURE;
				
		STDMETHOD(put_AutoExpoEnable) (THIS_
					BOOL bAutoExposure
					) PURE;

		STDMETHOD(get_AutoExpoTarget) (THIS_
					BYTE* Target
					) PURE;
				
		STDMETHOD(put_AutoExpoTarget) (THIS_
					BYTE Target
					) PURE;
				
		STDMETHOD(put_AWBInit) (THIS_
					PITOUPCAM_WHITEBALANCE_CALLBACK fnWBProc, void* pWBCtx
					) PURE;

		STDMETHOD(put_AuxRect) (THIS_
					RECT* pAuxRect
					) PURE;
				
		STDMETHOD(get_AuxRect) (THIS_
					RECT* pAuxRect
					) PURE;
				
		STDMETHOD(put_AuxRectShow) (THIS_
					BOOL bShow
					) PURE;
			
		STDMETHOD(get_AuxRectShow) (THIS_
					BOOL* bShow
					) PURE;
					
		STDMETHOD(put_VignetEnable) (THIS_
					BOOL bEnable
					) PURE;
				
		STDMETHOD(get_VignetEnable) (THIS_
					BOOL* bEnable
					) PURE;
		
		/* reserved, please use put_VignetAmountInt */
		STDMETHOD(put_VignetAmount) (THIS_
					double dAmount
					) PURE;
		
		/* reserved, please use get_VignetAmountInt */
		STDMETHOD(get_VignetAmount) (THIS_
					double* dAmount
					) PURE;

		/* reserved, please use put_VignetMidPointInt */
		STDMETHOD(put_VignetMidPoint) (THIS_
					double dMidPoint
					) PURE;
				
		/* reserved, please use get_VignetMidPointInt */
		STDMETHOD(get_VignetMidPoint) (THIS_
					double* dMidPoint
					) PURE;
		
		/* reserved, please use put_TempTint */
		STDMETHOD(put_WhiteBalanceGain) (THIS_
					int aGain[3]
					) PURE;

		/* reserved, please use get_TempTint */
		STDMETHOD(get_WhiteBalanceGain) (THIS_
					int aGain[3]
					) PURE;
				
		STDMETHOD(put_Hue) (THIS_
					int Hue
					) PURE;
				
		STDMETHOD(get_Hue) (THIS_
					int* Hue
					) PURE;

		STDMETHOD(put_Saturation) (THIS_
					int Saturation
					) PURE;
				
		STDMETHOD(get_Saturation) (THIS_
					int* Saturation
					) PURE;

		STDMETHOD(put_Brightness) (THIS_
					int Brightness
					) PURE;
				
		STDMETHOD(get_Brightness) (THIS_
					int* Brightness
					) PURE;
		
		/* microsecond */
		STDMETHOD(get_ExpoTime) (THIS_
					ULONG* Time
					) PURE;
		
		/* microsecond */
		STDMETHOD(put_ExpoTime) (THIS_
					ULONG Time
					) PURE;
		
		/* percent */
		STDMETHOD(get_ExpoAGain) (THIS_
					USHORT* AGain
					) PURE;
	
		/* percent */
		STDMETHOD(put_ExpoAGain) (THIS_
					USHORT AGain
					) PURE;

		STDMETHOD(put_LevelRange) (THIS_
					BYTE aLow[4], BYTE aHigh[4]
					) PURE;
				
		STDMETHOD(get_LevelRange) (THIS_
					BYTE aLow[4], BYTE aHigh[4]
					) PURE;
				
		STDMETHOD(get_Contrast) (THIS_
					int* Contrast
					) PURE;
				
		STDMETHOD(put_Contrast) (THIS_
					int Contrast
					) PURE;
				
		STDMETHOD(get_Gamma) (THIS_
					int* Gamma
					) PURE;
				
		STDMETHOD(put_Gamma) (THIS_
					int Gamma
					) PURE;

		STDMETHOD(get_Chrome) (THIS_
					BOOL* bChrome
					) PURE;
				
		STDMETHOD(put_Chrome) (THIS_
					BOOL bChrome
					) PURE;
				
		STDMETHOD(get_VFlip) (THIS_
					BOOL* bVFlip
					) PURE;
				
		STDMETHOD(put_VFlip) (THIS_
					BOOL bVFlip
					) PURE;
				
		STDMETHOD(get_HFlip) (THIS_
					BOOL* bHFlip
					) PURE;
				
		STDMETHOD(put_HFlip) (THIS_
					BOOL bHFlip
					) PURE;

		STDMETHOD(put_Speed) (THIS_
					BYTE nSpeed
					) PURE;

		STDMETHOD(get_Speed) (THIS_
					BYTE* pSpeed
					) PURE;
				
		/* get the maximum speed, see Misc page, "Frame Speed Level" */
		STDMETHOD(get_MaxSpeed) (THIS_) PURE;

		/* power supply: 
				0 -> 60HZ AC
				1 -> 50Hz AC
				2 -> DC
		*/
		STDMETHOD(put_HZ) (THIS_
					int nHZ
					) PURE;

		STDMETHOD(get_HZ) (THIS_
					int* nHZ
					) PURE;
				
		/* skip or bin */
		STDMETHOD(put_Mode) (THIS_
					BOOL bSkip
					) PURE;

		STDMETHOD(get_Mode) (THIS_
					BOOL* bSkip
					) PURE;

		STDMETHOD(put_ChromeCallback) (THIS_
					PITOUPCAM_CHROME_CALLBACK fnChromeProc, void* pChromeCtx
					) PURE;
				
		STDMETHOD(get_ExpTimeRange) (THIS_
					ULONG* nMin, ULONG* nMax, ULONG* nDef
					) PURE;
				
		STDMETHOD(get_ExpoAGainRange) (THIS_
					USHORT* nMin, USHORT* nMax, USHORT* nDef
					) PURE;
				
		STDMETHOD(get_ResolutionNumber) (THIS_
					) PURE;
				
		STDMETHOD(get_Resolution) (THIS_
					ULONG nIndex, LONG* pWidth, LONG* pHeight
					) PURE;
					
		STDMETHOD(put_TempTint) (THIS_
					int nTemp, int nTint
					) PURE;

		STDMETHOD(get_TempTint) (THIS_
					int* nTemp, int* nTint
					) PURE;

		STDMETHOD(put_VignetAmountInt) (THIS_
					int nAmount
					) PURE;
				
		STDMETHOD(get_VignetAmountInt) (THIS_
					int* nAmount
					) PURE;

		STDMETHOD(put_VignetMidPointInt) (THIS_
					int nMidPoint
					) PURE;
				
		STDMETHOD(get_VignetMidPointInt) (THIS_
					int* nMidPoint
					) PURE;
    };
#endif

#ifndef __ITOUPCAMSTILLIMAGE_DEFINED__
#define __ITOUPCAMSTILLIMAGE_DEFINED__
	/* most Touptek Camera DirectShow source filters has two output pins, one is pin category PIN_CATEGORY_PREVIEW, and the other is pin category PIN_CATEGORY_STILL.
		Please refrence MSDN library "Capturing an Image From a Still Image Pin". http://msdn.microsoft.com/en-us/library/dd318622(VS.85).aspx.
		Please see the example toupcamdemo which is in the install directory.
		*/
	// {E978EEA0-A0FF-465f-AB35-65907B8C62AC}
	DEFINE_GUID(IID_IToupcamStillImage, 0xe978eea0, 0xa0ff, 0x465f, 0xab, 0x35, 0x65, 0x90, 0x7b, 0x8c, 0x62, 0xac);

    DECLARE_INTERFACE_(IToupcamStillImage, IUnknown)
	{
		/*
			similar to put_Size, put_eSize
		*/
		STDMETHOD(put_StillSize) (THIS_
					LONG nWidth, LONG nHeight
					) PURE;

		STDMETHOD(get_StillSize) (THIS_
					LONG* pWidth, LONG* pHeight
					) PURE;

		STDMETHOD(put_eStillSize) (THIS_
					ULONG nIndex
					) PURE;

		STDMETHOD(get_eStillSize) (THIS_
					ULONG* pIndex
					) PURE;
					
		STDMETHOD(get_StillResolutionNumber) (THIS_
					) PURE;
				
		STDMETHOD(get_StillResolution) (THIS_
					ULONG nIndex, LONG* pWidth, LONG* pHeight
					) PURE;					
    };
#endif

#ifndef __ITOUPCAMLONGEXPOSURE_DEFINED__
#define __ITOUPCAMLONGEXPOSURE_DEFINED__
	/* some high-end model (such as CCD) Camera support long exposure.
		You can use QueryInterface on the filter or the pin to get the IToupcamLongExposure interface.
		Please see the example toupcamdemo which is in the install directory.
		*/
	// {D60D24ED-2B9E-4d46-9944-E2FDB4507DFD}
	DEFINE_GUID(IID_IToupcamLongExposure, 0xd60d24ed, 0x2b9e, 0x4d46, 0x99, 0x44, 0xe2, 0xfd, 0xb4, 0x50, 0x7d, 0xfd);

    DECLARE_INTERFACE_(IToupcamLongExposure, IUnknown)
	{
		/* microsecond */
		STDMETHOD(get_LongExpTime) (THIS_
					ULONG* Time
					) PURE;
				
		/* microsecond */
		STDMETHOD(put_LongExpTime) (THIS_
					ULONG Time
					) PURE;
				
		STDMETHOD(get_LongExpStatus) (THIS_
					BOOL* bLongExping,
					ULONG* TotalTime,
					ULONG* LeftTime
					) PURE;
				
		STDMETHOD(put_LongExpCancel) (THIS_
					) PURE;
				
		STDMETHOD(get_LongExpTimeRange) (THIS_
					ULONG* nMin, ULONG* nMax, ULONG* nDef
					) PURE;
    };
#endif
	
#ifndef __ITOUPCAMSERIALNUMBER_DEFINED__
#define __ITOUPCAMSERIALNUMBER_DEFINED__
	/*
		serial number is always 32 chars which is zero-terminated such as "TP110826145730ABCD1234FEDC56787"
	*/
	// {E12D4B13-333F-4eae-BC89-0446D1FC634D}
	DEFINE_GUID(IID_IToupcamSerialNumber, 0xe12d4b13, 0x333f, 0x4eae, 0xbc, 0x89, 0x4, 0x46, 0xd1, 0xfc, 0x63, 0x4d);

    DECLARE_INTERFACE_(IToupcamSerialNumber, IUnknown)
	{
		STDMETHOD(get_SerialNumber) (THIS_
					char sn[32]
					) PURE;
    };
#endif
	
#ifdef __cplusplus
}
#endif

#endif