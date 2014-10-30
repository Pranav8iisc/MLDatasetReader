#include <iostream>
#include <string>
#include "idxDatasetReader.h"


using namespace std;



// extracts magic number from the idx file
unsigned int getMagicNumber(string fileName)
{
	unsigned int tempMagicNumber;	
	
	ifstream file(fileName, ios::read|ios::binary);
	
	file >> tempMagicNumber;

	return tempMagicNumber;
}

// returns extracted datatype id from the magic number
unsigned char getDataType(std::string fileName)
{
	unsigned char magicNumber = getMagicNumber(fileName);
	return (magicNumber >> 8) & 0xFF; // the second MSB
}

// -------------main()-----------------------------//

int main()
{	
	string fileName;

	cout << "Please enter the filename";

	cin >> fileName;

	unsigned char datasetType = getDatasetType(fileName);

	switch(datasetType)	
	{
	
		case 0x08:
			IdxDatasetReader<unsigned char> idxReader;
			break;
		case 0x09:
			IdxDatasetReader<char> idxReader;
			break;
		case 0x0B:
			IdxDatasetReader<short> idxReader;
			break;
		case 0x0C:
			IdxDatasetReader<int> idxReader;
			break;
		case 0x0D:
			IdxDatasetReader<float> idxReader;
			break;
		case 0x0E:
			IdxDatasetReader<double> idxReader;
			break;
		default:
			cout << "Invalid dataset type specified :(";
		
	}

	idxReader.fileName = fileName;

	idxReader.getDataset();

	idxReader.saveJPEG(0);

	return 0;
}
