#pragma once
#include "stdafx.h"
#include "ChannelDataFeed.h"

namespace strongridgui 
{	
	class CsvFileStreamDataFeed
	{
	public:
		CsvFileStreamDataFeed();
		~CsvFileStreamDataFeed();

		void SetInputFile(CString filename);
		CString GetInputFile() const;

		// Reads the current entry
		std::vector<float> ReadEntry();

		// Advance pointer
		void AdvanceCursor();

	private:
		CString m_fileName;
		std::vector<std::vector<float>> m_dataset;
		int m_cursor;
	};

	class FilestreamPhasorDataFeed : public IPhasorDataFeed
	{
	public:
		FilestreamPhasorDataFeed();
		void SetInputFile( CString inputFile );
		CString GetInputFile() const;

		virtual void GetPhasorValue( float* part1, float* part2 );
		virtual void SignalNextFrame();

	private:
		CsvFileStreamDataFeed m_dataFeed;
	};

	class FilestreamAnalogDataFeed : public IAnalogDataFeed
	{
	public:
		FilestreamAnalogDataFeed();
		void SetInputFile( CString inputFile );
		CString GetInputFile() const;

		virtual void GetAnalogValue( float* val );
		virtual void SignalNextFrame();

	private:
		CsvFileStreamDataFeed m_dataFeed;
	};

	class FilestreamDigitalDataFeed : public IDigitalDataFeed
	{
	public:
		FilestreamDigitalDataFeed();
		void SetInputFile( CString inputFile );
		CString GetInputFile() const;

		virtual bool GetDigitalStatus();
		virtual void SignalNextFrame();

	private:
		CsvFileStreamDataFeed m_dataFeed;
	};
}