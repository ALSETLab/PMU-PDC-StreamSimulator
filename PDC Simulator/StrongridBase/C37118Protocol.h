#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <cfloat>
#include <math.h>

namespace strongridbase
{
	enum C37118HdrFrameType
	{
		DATA_FRAME = 0,
		HEADER_FRAME = 1,
		CONFIGURATION_FRAME_1 = 2,
		CONFIGURATION_FRAME_2 = 3,
		CONFIGURATION_FRAME_3 = 5,
		COMMAND_FRAME = 4
	};
	
	struct C37118SyncField
	{
		char LeadIn; // Should be 0xAA
		C37118HdrFrameType FrameType;
		char Version;
	};

	struct C37118FracSec
	{
		uint32_t FractionOfSecond;
		uint8_t TimeQuality;
		
		static C37118FracSec Create( uint32_t fracSec, int leapSecOffset, bool leapSecPending, int cloclErrorLevel );
		void GetParsedQuality( int* outLeapSecOffset, bool* outLeapSecPending, float* outTimeClockMaxErrorSec, bool* outIsRealiable ) const;
	};

	struct C37118NomFreq
	{
		bool Bit0_1xFreqIs50_0xFreqIs60;

		int GetAsFrequency() const { return Bit0_1xFreqIs50_0xFreqIs60 == 1 ? 50 : 60; }
	};

	struct C37118TimeBase
	{
		uint8_t Flags;
		uint32_t TimeBase;
	};

	struct C37118PmuFormat
	{
		bool Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle;
		bool Bit1_0xPhasorsIsInt_1xPhasorFloat;
		bool Bit2_0xAnalogIsInt_1xAnalogIsFloat;
		bool Bit3_0xFreqIsInt_1xFreqIsFloat;
	};

	struct C37118PhasorUnit
	{
		C37118PhasorUnit() {}
		C37118PhasorUnit( uint8_t type_, uint32_t scalar_ )
		{
			Type = type_;
			PhasorScalar = scalar_;
		}

		uint8_t Type; // 0=Volt, 1=Current;
		uint32_t PhasorScalar;
	};

	struct C37118AnalogUnit
	{
		C37118AnalogUnit() {}
		C37118AnalogUnit(int type, int32_t scalar )
		{
			Type_X = type;
			AnalogScalar = scalar;
		}

		// 0 = Single point on wave
		// 1 = RMS of analog input
		// 2 = peak of analog input
		// 5-64 = reserved
		int Type_X; 
		int32_t AnalogScalar;
	};

	struct C37118DigitalUnit
	{
		C37118DigitalUnit() {}
		C37118DigitalUnit( uint16_t normSts, uint16_t validbits )
		{
			DigNormalStatus = normSts;
			DigValidInputs = validbits;
		}

		void BitAtIdx( int idx, bool* outNormalBit, bool* outValidBit ) const
		{
			uint16_t mask = (0x1 << idx);
			*outNormalBit = (DigNormalStatus & mask) != 0;
			*outValidBit = (DigValidInputs & mask) != 0;
		}

		void SetValidBit( int idx, bool val )
		{
			uint16_t mask = (0x1 << idx);
			DigValidInputs &= ~mask;
			if( val ) DigValidInputs |= mask;
		}

		void SetNormalStsBit( int idx, bool val )
		{
			uint16_t mask = (0x1 << idx);
			DigNormalStatus &= ~mask;
			if( val ) DigNormalStatus |= mask;
		}



		uint16_t DigNormalStatus;
		uint16_t DigValidInputs;
	};

	enum PhasorComponentCodeEnum
	{
		PHC0_ZERO_SEQUENCE = 0,
		PHC1_POSITIVE_SEQUENCE = 1,
		PHC2_NEGATIVE_SEQUENCE= 2,
		PHC3_RESERVED = 3,
		PHC4_PHASE_A = 4,
		PHC5_PHASE_B = 5,
		PHC6_PHASE_C = 6,
		PHC7_RESERVED = 7
	};

	struct C37118PhasorScale_Ver3
	{
		C37118PhasorScale_Ver3() {}
		C37118PhasorScale_Ver3( uint8_t voltOrCurrent_,PhasorComponentCodeEnum pcc_, float scaleFactorY_, float scaleFactorAngle_ )
		{
			PhasorBits = 0;
			VoltOrCurrent = voltOrCurrent_;
			PhasorComponentCode = pcc_;
			ScaleFactorOne_Y = scaleFactorY_;
			ScaleFactorTwo_Angle = scaleFactorAngle_;
		}

		// Group 1
		uint16_t PhasorBits; // Skip impl for now..
		uint8_t VoltOrCurrent; // 0 = voltage, 1 = current
		PhasorComponentCodeEnum PhasorComponentCode; 

		// Group 2+3
		float ScaleFactorOne_Y; // Scales primary volt/amperes
		float ScaleFactorTwo_Angle; // Scales phasor angle adjustment
	};

	struct C37118AnalogScale_Ver3
	{
		C37118AnalogScale_Ver3() { }
		C37118AnalogScale_Ver3(  float scale_, float offset_ )
		{
			Scale = scale_;
			Offset = offset_;
		}

		float Scale;
		float Offset;
	};

	struct C37118PmuConfiguration
	{
		std::string StationName;
		uint16_t IdCode;
		C37118PmuFormat DataFormat; 
		std::vector<std::string> phasorChnNames;
		std::vector<std::string> analogChnNames;
		std::vector<std::string> digitalChnNames;
		std::vector<C37118PhasorUnit> PhasorUnit;
		std::vector<C37118AnalogUnit> AnalogUnit;
		std::vector<C37118DigitalUnit> DigitalUnit; // 16 chn names per unit
		C37118NomFreq NomFreqCode;
		uint16_t ConfChangeCnt;
	};

	
	struct C37118FrameHeader
	{
		C37118SyncField Sync;
		uint16_t FrameSize;
		uint16_t IdCode;
		uint32_t SOC;
		C37118FracSec FracSec;
	};

	struct C37118DataRate
	{
		static C37118DataRate CreateByRawC37118Format( int16_t datarateRaw );
		static C37118DataRate CreateByFramesPerSecond( float datarateRaw );

		int16_t RawDataRate() const;
		float FramesPerSecond() const;

	private:
		int16_t m_datarateRaw;
	};


	struct C37118PdcConfiguration
	{
		C37118FrameHeader HeaderCommon;
		C37118TimeBase TimeBase;
		std::vector<C37118PmuConfiguration> PMUs;
		C37118DataRate DataRate;
		uint16_t FooterCrc16;
	};

	struct C37118PmuConfiguration_Ver3
	{
		std::string StationName;
		uint16_t IdCode;
		char GlobalPmuId[16];
		C37118PmuFormat DataFormat; 
		std::vector<std::string> phasorChnNames;
		std::vector<std::string> analogChnNames;
		std::vector<std::string> digitalChnNames;
		std::vector<C37118PhasorScale_Ver3> PhasorScales;
		std::vector<C37118AnalogScale_Ver3> AnalogScales;
		std::vector<C37118DigitalUnit> DigitalUnits;
		float POS_LAT;
		float POS_LON;
		float POS_ELEV;
		unsigned char ServiceClass;
		int32_t PhasorMeasurementWindow;
		int32_t PhasorMeasurementGroupDelayMs;
		C37118NomFreq NomFreqCode;
		uint16_t ConfChangeCnt;
	};

	struct C37118ContIdx
	{
		C37118ContIdx() { m_raw = 0; }
		static C37118ContIdx CreateByC37118Raw( uint16_t rawValue );
		static C37118ContIdx CreateAsFrameInSequence( int frameIdx, int numFrames );

		int GetCurrentFrameIndex() const;
		bool IsLastFrame() const;
		uint16_t GetRawC37118Value() const;

	private:
		C37118ContIdx(uint16_t rawval);
		uint16_t m_raw;
	};
	
	struct C37118PdcConfiguration_Ver3
	{
		C37118FrameHeader HeaderCommon;
		C37118ContIdx ContinuationIndex;
		C37118TimeBase TimeBase;
		std::vector<C37118PmuConfiguration_Ver3> PMUs;
		C37118DataRate DataRate;
		uint16_t FooterCrc16;
	};

	struct C37118PmuDataFrameStat
	{
	public:
		C37118PmuDataFrameStat() {m_raw = 0; }
		 C37118PmuDataFrameStat(uint16_t raw) { m_raw = raw; }

		 uint16_t ToRaw() const;

		 // Getters..
		 uint8_t getDataError() const;
		 bool getPmuSyncFlag() const;
		 bool getDataSortingFlag() const;
		 bool getPmuTriggerFlag() const;
		 bool getConfigChangeFlag() const;
		 bool getDataModifiedFlag() const;
		 uint8_t getTimeQualityCode() const;
		 uint8_t getUnlockTimeCode() const;
		 uint8_t getTriggerReasonCode() const;

		 // Setters
		 void setDataErrorCode(uint8_t errCode);
		 void setPmuSyncFlag(bool isset);
		 void setDataSortingFlag(bool isset);
		 void setPmuTriggerFlag(bool isset);
		 void setConfigChangeFlag(bool isset);
		 void setDataModifiedFlag(bool isset);
		 void setTimeQualityCode(uint8_t code);
		 void setUnlockTimeCode(uint8_t code);
		 void setTriggerReasonCode(uint8_t code);

	private:
		uint16_t m_raw;
	};

	struct C37118PmuDataFramePhasorRealImag
	{
	public:
		static C37118PmuDataFramePhasorRealImag CreateByRealImag(int16_t real, int16_t imag);
		static C37118PmuDataFramePhasorRealImag CreateByRealImag(float real, float imag);		
		static C37118PmuDataFramePhasorRealImag CreateByPolarMag(uint16_t magnitude, int16_t angleRad); // angle assumed to be in "radians * 10^4"
		static C37118PmuDataFramePhasorRealImag CreateByPolarMag(float magnitude, float angleRad);

		void getRealImagAsFloat(float* refReal, float* refImag) const;
		void getRealImagAsInt16(int16_t* refReal, int16_t* refImag) const;
		void getMagAngleAsFloat(float* mag, float* angle) const;
		void getMagAngleAsInt16(uint16_t* mag, int16_t* angle) const;

		float Real;
		float Imag;
	};

	struct C37118PmuDataFrameAnalog
	{
	public:
		static C37118PmuDataFrameAnalog CreateByInt16(int16_t value);
		static C37118PmuDataFrameAnalog CreateByFloat(float value);		

		float getValueAsFloat() const;
		int16_t getValueAsInt16() const;

	private:
		float Value;
	};

	struct C37118PmuDataFrameDigitalHelper
	{
	public:
		static C37118PmuDataFrameDigitalHelper CreateByBoolArray(std::vector<bool> digBits);
		static C37118PmuDataFrameDigitalHelper CreateByUint16Arr(std::vector<uint16_t> digWordArray);		
		void PushDigWord( uint16_t word);
		void PushDigValue( bool bit );

		std::vector<bool> ToBoolArray() const;
		std::vector<uint16_t> ToDigWord() const;

	private:
		std::vector<bool> m_digValues;
	};


	struct C37118PmuDataFrame
	{
		C37118PmuDataFrameStat Stat;
		std::vector<C37118PmuDataFramePhasorRealImag> PhasorValues;
		float Frequency;
		float DeltaFrequency;
		std::vector<C37118PmuDataFrameAnalog> AnalogValues;
		std::vector<bool> DigitalValues;
	};

	struct C37118PdcDataFrame
	{
		C37118FrameHeader HeaderCommon;
		std::vector<C37118PmuDataFrame> pmuDataFrame;
		uint16_t CRC16;
	};


	enum C37118CmdType
	{
		KILL_RTD = 1,
		START_RTD = 2,
		SEND_HDR_FRAME = 3,
		SEND_CFG1_FRAME = 4,
		SEND_CFG2_FRAME = 5,
		SEND_CFG3_FRAME = 6
	};

	struct C37118CommandFrame
	{
		C37118FrameHeader Header;
		C37118CmdType CmdType;
		uint16_t CRC16;
	};


	struct C37118PdcHeaderFrame
	{
		C37118FrameHeader Header;
		std::string HeaderMessage;		
		uint16_t FooterCrc16;
	};


	struct C37118PmuDataDecodeInfo
	{
		int numPhasors;
		int numAnalogs;
		int numDigitals;
		C37118PmuFormat DataFormat; 
	};

	struct C37118PdcDataDecodeInfo
	{
		C37118TimeBase timebase;
		std::vector<C37118PmuDataDecodeInfo> PMUs;
	};


	class C37118Protocol
	{
	public:
		static void WriteConfigurationFrame(char* data, const C37118PdcConfiguration* pdcconfig, int* offset);
		static void WriteConfigurationFrame_Ver3( char* data, const C37118PdcConfiguration_Ver3* pdcConfg, int* offset);
		static void WriteFrameHeader(char* data, const C37118FrameHeader* frameHeader, int* offset);
		static void WriteDataFrame(char* data, const C37118PdcDataDecodeInfo* config, const C37118PdcDataFrame* dataFrame, int* offset);
		static void WriteHeaderFrame(char* data, const C37118PdcHeaderFrame* headerFrame, int* offset);
		static void WriteCommandFrame(char* data, const C37118CommandFrame* cmdFrame, int* offset);

		static void WriteSyncField(char* data,const C37118SyncField* syncField, int* offset);
		static void WriteFracSecField(char* data, const C37118FracSec* fracSecField, int* offset);
		static void WriteNomFreqField(char* data, const C37118NomFreq* nomFreqField, int* offset);
		static void WriteTimeBaseField(char* data,  const C37118TimeBase* timeBaseField , int* offset);
		static void WriteC37118PmuFormat(char* data, const C37118PmuFormat* pmuFormat, int* offset );
		static void WriteC37118PhasorUnit(char* data,const C37118PhasorUnit* pmuFormat , int* offset);
		static void WriteC37118AnalogUnit(char* data,  const C37118AnalogUnit* pmuFormat, int* offset );
		static void WriteC37118DigitalUnit(char* data, const C37118DigitalUnit* pmuFormat, int* offset );	
		static void WriteC37118PhasorScale_Ver3(char* data, const C37118PhasorScale_Ver3* phScale, int* offset);
		static void WriteC37118AnalogScale_Ver3(char* data, const C37118AnalogScale_Ver3* phScale, int* offset);


		static C37118PdcConfiguration ReadConfigurationFrame(char* data, int length);
		static C37118PdcConfiguration_Ver3 ReadConfigurationFrame_Ver3(char* data, int length);
		static C37118FrameHeader ReadFrameHeader(char* data, int length, int* offset);
		static C37118PdcDataFrame ReadDataFrame(char* data, int length, const C37118PdcDataDecodeInfo* config, int* offset);
		static C37118PdcHeaderFrame ReadHeaderFrame(char* data, int length, int* offset);
		static C37118CommandFrame ReadCommandFrame(char* data, int bufferSize, int* offset);

		static C37118SyncField ReadSyncField(char* data, int* offset);
		static C37118FracSec ReadFracSecField(char* data,int* offset);
		static C37118NomFreq ReadNomFreqField(char* data, int* offset);
		static C37118TimeBase ReadTimeBaseField(char* data,int* offset);
		static C37118PmuFormat ReadC37118PmuFormat(char* data, int* offset );
		static C37118PhasorUnit ReadC37118PhasorUnit(char* data,int* offset);
		static C37118AnalogUnit ReadC37118AnalogUnit(char* data, int* offset );
		static C37118DigitalUnit ReadC37118DigitalUnit(char* data,int* offset );
		static C37118PhasorScale_Ver3 ReadC37118PhasorScale_Ver3(char* data, int* offset);
		static C37118AnalogScale_Ver3 ReadC37118AnalogScale_Ver3(char* data, int* offset);

		// Helper functions
		static C37118PdcDataDecodeInfo CreateDecodeInfoByPdcConfig(const C37118PdcConfiguration& pdccfg) ;
		static C37118PdcDataDecodeInfo CreateDecodeInfoByPdcConfig(const C37118PdcConfiguration_Ver3& pdccfg);
		static C37118PdcConfiguration DowngradePdcConfig(const C37118PdcConfiguration_Ver3* pdccfg);
		static C37118PmuConfiguration DowngradePmuConfig(const C37118PmuConfiguration_Ver3* pdccfg);

		static uint16_t CalcCrc16(char* data, int length);

	private:
		static void WriteFooter(char* data, int offsetAtStart, int* offset);
	};
}