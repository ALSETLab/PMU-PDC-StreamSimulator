#pragma once

#include "stdafx.h"
#include <vector>

namespace strongridgui 
{	
	class SerializableCStringList
	{
	public:
		std::vector<CString> items;

		void Serialize(CArchive& ar) {
			if(ar.IsStoring()) {
				ar << (int)items.size();
				for( int i = 0; i < items.size(); ++i )
				{
					ar << items[i];
				}
			}
			else {
				// be sure to read in the order in which they were stored
				int numItems;
				ar >> (int)numItems;
            
				// Read each item in sequence
				for(int i = 0; i < numItems; i++) {
					CString tmp;
					ar >> tmp;
					items.push_back(tmp);
				}
			}
		}
	};

	template<class T>
	class SerializableListPtr
	{
	public:
		std::vector<T*> items;

		void Serialize(CArchive& ar) {
			if(ar.IsStoring()) {
				ar << (int)items.size();
				for( int i = 0; i < items.size(); ++i )
				{
					items[i]->Serialize(ar);
				}
			}
			else {
				// be sure to read in the order in which they were stored
				int numItems;
				ar >> (int)numItems;
            
				// Read each item in sequence
				for(int i = 0; i < numItems; i++) {
					T* t = new T();
					t->Serialize(ar);
					items.push_back(t);
				}
			}
		}
	};

	class CPhasorUnit : public CObject
	{
	public:
		DECLARE_SERIAL( CPhasorUnit )	
		CPhasorUnit() {}
		CPhasorUnit( int type, int scalar ) { SG_Type = type; SG_Scalar=scalar; }
		virtual void Serialize( CArchive& ar );

		int SG_Type;
		int SG_Scalar;
	};

	class CAnalogUnit : public CObject
	{
	public:
		DECLARE_SERIAL( CAnalogUnit )	
		CAnalogUnit() {}
		CAnalogUnit( int type, int scalar ) { SG_Type = type; SG_Scalar = scalar; }
		virtual void Serialize( CArchive& ar );

		int SG_Type; 
		int SG_Scalar;
	};

	class CDigitalUnitBit : public CObject
	{	
	public:
		DECLARE_SERIAL( CDigitalUnitBit )	
		CDigitalUnitBit() {}
		CDigitalUnitBit( bool validin, bool normalst ) { SG_ValidInput = validin; SG_NormalState = normalst; }
		virtual void Serialize( CArchive& ar );

		bool SG_ValidInput;
		bool SG_NormalState;
	};

	class CPmuFormat : public CObject
	{
	public:
		DECLARE_SERIAL( CPmuFormat )	
		virtual void Serialize( CArchive& ar );

		bool Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle;
		bool Bit1_0xPhasorsIsInt_1xPhasorFloat;
		bool Bit2_0xAnalogIsInt_1xAnalogIsFloat;
		bool Bit3_0xFreqIsInt_1xFreqIsFloat;
	};

	class CPmuConfig : public CObject
	{
	public:
		DECLARE_SERIAL( CPmuConfig )	
		virtual void Serialize( CArchive& ar );

		CString SG_PmuStationName;
		int SG_PmuIdCode;
		CPmuFormat SG_PmuFormat;
		int SG_ChangeCnt;

		SerializableCStringList SG_PhasorNames;
		SerializableCStringList SG_AnalogNames;
		SerializableCStringList SG_DigitalNames;

		SerializableListPtr<CPhasorUnit> SG_PhasorUnitArray;
		SerializableListPtr<CAnalogUnit> SG_AnalogUnitArray;
		SerializableListPtr<CDigitalUnitBit> SG_DigitalUnitArray;
	
		bool SG_FreqCode; // 0=50 hz, 1=60 hz;
	};

	class CPdcConfig : public CObject
	{
	public:
		DECLARE_SERIAL( CPdcConfig )	
		virtual void Serialize( CArchive& ar );

		int SG_PdcId;
		int SG_ListenPort;
		int SG_TimeBase;
		float SG_DataRate_FramesPerSecond;

		SerializableListPtr<CPmuConfig> SG_PmuConfigArray;
	};
}