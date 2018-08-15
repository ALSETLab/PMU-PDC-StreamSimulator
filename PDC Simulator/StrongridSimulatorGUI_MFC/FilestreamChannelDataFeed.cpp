#include "stdafx.h"
#include <vector>
#include "FilestreamChannelDataFeed.h"

using namespace strongridgui;



// --------------------------------------------------------
// -------- CsvFileStreamDataFeed -----------------------
// --------------------------------------------------------

CsvFileStreamDataFeed::CsvFileStreamDataFeed()
{
	m_cursor = 0;
}

CsvFileStreamDataFeed::~CsvFileStreamDataFeed()
{
}

std::vector<std::vector<float>> SplitStringInto2dValueArray(CString str)
{	
	// Extract value-sets (separated by ',')
	std::vector<CString> valueSet;
	{
		int nTokenPos = 0;
		CString strToken = str.Tokenize(_T(","), nTokenPos);

		while (!strToken.IsEmpty())
		{
			valueSet.push_back(strToken);
			strToken = str.Tokenize(_T(","), nTokenPos);
		}
	}

	// Traverse the value-sets and extract individual values, as separated by ";"
	std::vector<std::vector<float>> output;
	for( std::vector<CString>::const_iterator iter = valueSet.begin(); iter != valueSet.end(); ++iter )
	{
		int nTokenPos = 0;
		CString strToken = iter->Tokenize(_T(";"), nTokenPos);

		// Extract values from valueset
		std::vector<float> valuesInSet;
		while (!strToken.IsEmpty())
		{
			valuesInSet.push_back(_tstof(strToken)); 
			strToken = iter->Tokenize(_T(";"), nTokenPos);
		} 
		output.push_back(valuesInSet);
	}

	return output;
}

void CsvFileStreamDataFeed::SetInputFile(CString filename)
{
	m_fileName = filename;

	// Read file into memory and disacard the file handle
	CStdioFile file(filename, CFile::OpenFlags::modeRead);
	CString csvString;
	int bytesRead = file.ReadString(csvString);
	file.Close();
	
	// Extract value-set	
	m_dataset = SplitStringInto2dValueArray(csvString);
}

CString CsvFileStreamDataFeed::GetInputFile() const
{
	return m_fileName;
}

std::vector<float> CsvFileStreamDataFeed::ReadEntry()
{
	if( m_dataset.size() == 0 ) return std::vector<float>(2, 0.0f); // default to vector of 2 elements, 0 in value
	return m_dataset[m_cursor];
}

void CsvFileStreamDataFeed::AdvanceCursor()
{
	m_cursor = ++m_cursor % m_dataset.size();
}

// --------------------------------------------------------
// -------- FilestreamPhasorDataFeed -----------------------
// --------------------------------------------------------

FilestreamPhasorDataFeed::FilestreamPhasorDataFeed()
{
}

void FilestreamPhasorDataFeed::SetInputFile( CString inputFile )
{
	m_dataFeed.SetInputFile(inputFile);
}

CString FilestreamPhasorDataFeed::GetInputFile() const
{
	return m_dataFeed.GetInputFile();
}

void FilestreamPhasorDataFeed::GetPhasorValue( float* part1, float* part2 )
{
	std::vector<float> tmp = m_dataFeed.ReadEntry();
	if( tmp.size() < 2 ) 
	{
		*part1 = 0;
		*part2 = 0;
	}
	else
	{
		*part1 = tmp[0];
		*part2 = tmp[1];
	}
}

void FilestreamPhasorDataFeed::SignalNextFrame()
{
	m_dataFeed.AdvanceCursor();
}


	
// --------------------------------------------------------
// -------- FilestreamAnalogDataFeed -----------------------
// --------------------------------------------------------


FilestreamAnalogDataFeed::FilestreamAnalogDataFeed()
{
}

void FilestreamAnalogDataFeed::SetInputFile( CString inputFile )
{
	m_dataFeed.SetInputFile(inputFile);
}

CString FilestreamAnalogDataFeed::GetInputFile() const
{
	return m_dataFeed.GetInputFile();
}

void FilestreamAnalogDataFeed::GetAnalogValue( float* val )
{
	std::vector<float> tmp = m_dataFeed.ReadEntry();
	*val = tmp[0];
}

void FilestreamAnalogDataFeed::SignalNextFrame()
{
	m_dataFeed.AdvanceCursor();
}


	
// --------------------------------------------------------
// -------- FilestreamDigitalDataFeed -----------------------
// --------------------------------------------------------


FilestreamDigitalDataFeed::FilestreamDigitalDataFeed()
{
}

void FilestreamDigitalDataFeed::SetInputFile( CString inputFile )
{
	m_dataFeed.SetInputFile(inputFile);
}

CString FilestreamDigitalDataFeed::GetInputFile() const
{
	return m_dataFeed.GetInputFile();
}

bool FilestreamDigitalDataFeed::GetDigitalStatus()
{
	std::vector<float> tmp = m_dataFeed.ReadEntry();
	return tmp[0] != 0;
}

void FilestreamDigitalDataFeed::SignalNextFrame()
{
	m_dataFeed.AdvanceCursor();
}
