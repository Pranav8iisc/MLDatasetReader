#include <iostream>
#include <string>
#include <bitset>
#include "idxDatasetReader.h"


using namespace std;

int reverseInt (int i)
{
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}


bool isLittleEndian()
{
        union {
        char c[sizeof(int)];
        int n;
        } isLittle;

        isLittle.n = 1;

        if (isLittle.c[0] == 0x00)
                return true;
        else
                return false;
}



// extracts magic number from the idx file
unsigned int getMagicNumber(string fileName, bool isLittleEndianTrue)
{
	unsigned int tempMagicNumber;	
	
	ifstream file;	

	file.open(fileName.c_str(), ifstream::in);
	
	if (file.is_open())
		cout << "\nFile opened succesfully!!\n";

	file.read((char*)&tempMagicNumber, sizeof(tempMagicNumber));

	file.close();
        
	//if (isLittleEndianTrue)
	tempMagicNumber = reverseInt(tempMagicNumber);

        cout << "\nHELLP:\t" << tempMagicNumber;
	
	return tempMagicNumber;
}


// returns extracted datatype id from the magic number
unsigned char getDatasetType(std::string fileName)
{
	unsigned int magicNumber = getMagicNumber(fileName, isLittleEndian());
	cout << "\nMagic number:\t" << magicNumber;
	//magicNumber = __builtin_bswap32(magicNumber);
	return ((magicNumber >> 8) & 0xFF); // the second MSB
}



int main()
{	
	string fileName;

	cout << "\nPlease enter the filename:\t";

	cin >> fileName;

	unsigned char datasetType = getDatasetType(fileName);

	//bitset<8> x(datasetType);
	cout << "\nDataset type:\t" << (unsigned int)datasetType; 
		

	switch(datasetType)	
	{
	
		case 0x08:
			{
				IdxDatasetReader<unsigned char> idxReader(fileName, datasetType);
				idxReader.getDataset();
				idxReader.saveJPEG(0);
			}
			break;
		case 0x09:
			{
				IdxDatasetReader<char> idxReader(fileName, datasetType);
				idxReader.getDataset();
				idxReader.saveJPEG(0);
			}
			break;
		case 0x0B:
			{
				IdxDatasetReader<short> idxReader(fileName, datasetType);
				idxReader.getDataset();
				idxReader.saveJPEG(0);
			}
			break;
		case 0x0C:
			{
				IdxDatasetReader<int> idxReader(fileName, datasetType);
				idxReader.getDataset();
				idxReader.saveJPEG(0);
			}
			break;
		case 0x0D:
			{
				IdxDatasetReader<float> idxReader(fileName, datasetType);
				idxReader.getDataset();
				idxReader.saveJPEG(0);
			}
			break;
		case 0x0E:
			{
				IdxDatasetReader<double> idxReader(fileName, datasetType);
				idxReader.getDataset();
				idxReader.saveJPEG(0);
			}
			break;
		default:
			cout << "\nInvalid dataset type specified :(\n";		
	}


	return 0;
}
