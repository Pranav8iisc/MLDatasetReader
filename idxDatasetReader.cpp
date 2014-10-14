#include <ifstream>
#include <opencv/highgui.h>
#include "idxDatasetReader.h"

using namespace std;

idxDatasetReader::idxDatasetReader()
{	
	
}

~idxDatasetReader::idxDatasetReader()
{
	
}

// I suspect this is not portable :(
// http://stackoverflow.com/questions/280162/is-there-a-way-to-do-a-c-style-compile-time-assertion-to-determine-machines-e
bool idxDatasetReader::isLittleEndian()
{
	union {
	char c[sizeof(int)];
	int n;
	} isLittle = 1;
	
	if (isLittle[0] == 0x00)
		return true;
	else
		return false;		
}

unsigned int idxDatasetReadr::getMagicNumber()
{
	unsigned int tempMagicNumber;	
	return (file >> tempMagicNumber);
}

// returns extracted datatype id from the magic number
char idxDatasetReader::getDatatype()
{
	return (magicNumber >> 8) & 0xFF; // the second MSB
}

// returns the extracted number of dimensions from the magic number
char idxDatasetReader::getNumberOfDimensions()
{
	return (magicNumber & 0xFF);
}

unsigned int idxDatasetReader::getNumberOfDatasets()
{
	unsigned int tempNumberOfDatasets;
	file >> tempNumberOfDatasets;
	
	return tempNumberOfDatasets;
}

unsigned int* idxDatasetReader::getSizeOfDimension()
{

}

void idxDatasetReader::getDataset(string inputFileName, unsigned nInputDatasets)
{
	filename = inputFileName;
		
	if(this->file.open(fileName) == NULL)
		{
			cout << "Invalid filename. Please ensure correct filename is specified";
			exit(0);
		}
	
	// extract magic number
	magicNumber = getMagicNumber();
	
	bool isLittleEndian;
		
	if (isLittleEndian() == true)
		{
			isLittleEndian = true;
			magicNumber = __builtin_bswap32(magicNumber);
		}
	
	// get datatype
	char datatypeId; 
	datatypeId = getDatatype();
	
	// get number of dimensions
	char nDimensions;
	nDimensions = getNumberOfDimension();
		
	// get size of dimensions
	sizeOfDimension = getSizeOfDimension();	
	
	// size of dataset as implied by file header:
	unsigned nDatasets = getNumberOfDatasets();
	
	unsigned predictedDatasetSize = nDatasets;
	
	for (unsigned i = 0; i < nDimensions; i++)
		predictedDatasetSize *= sizeOfDimension[i];
		
	unsigned actualDatasetSize;
	
	if (nDimensions > 1)
		actualDatasetSize = file.size() - sizeof(unsigned int) * (nDimensions + 2); // 1: magic number + 1: number of datasets	
	else
		actualDatasetSize = return (file.size() - sizeof(unsigned int) * (nDimensions + 1)); // 1:magic number
		
	bool hasValidFileSize = (predictedDatasetSize == actualDatasetSize) ? true : false;
			
	// check if file contains number of bytes as implied by file header
	if (hasValidFileSize == false)
		{
			cout << "Actual data < data size implied by the header :(";
			exit(0);			
		}
		
	// read dataset
	switch(datatypeId)
	{
		case 0x08:		
		data = new unsigned char[nDatasets][actualDatasetSize];
		for(unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
			for (unsigned byteId = 0; byteId < totalDatasetSize; byteId++)
				file.get(data[datasetId][byteId]); 
		break;
		
		case 0x09: 
		data = new char[nDatasets][actualDatasetSize];
		for(unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
			for (unsigned byteId = 0; byteId < totalDatasetSize; byteId++)
				file.get(data[datasetId][byteId]); 
		break;
		
		case 0x0B: 
		data = new short[nDatasets][totalDatasetSize]; 
		for (unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
			for (unsigned byteId = 0; byteId < totalDatasetSize; byteId++)
				{
					file.get(data[datasetId][byteId]); 
					data[datasetId][byteId] = __builtin_bswap16(magicNumber);
				}
		break;
		
		case 0x0C: 
		data = new int[nDatasets][totalDatasetSize];
		for (unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
			for (unsigned byteId = 0; byteId < totalDatasetSize; byteId++)
				{
					file.get(data[datasetId][byteId]); 
					data[datasetId][byteId] = __builtin_bswap32(magicNumber);
				}
		break;
		
		case 0x0D: 
		data = new float[nDatasets][totalDatasetSize];
		for (unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
			for (unsigned byteId = 0; byteId < totalDatasetSize; byteId++)
				{
					file.get(data[datasetId][byteId]); 
					data[datasetId][byteId] = __builtin_bswap32(magicNumber);
				}
		break;
		
		case 0x0E: 
		data = new double[nDatasets][totalDatasetSize];
		for (unsigned datasetId = 0; datasetId < nDatasets; datasetId++)			
			for (unsigned byteId = 0; byteId < totalDatasetSize; byteId++)
			{
				file.get(data[datasetId][byteId]); 
				data[datasetId][byteId] = __builtin_bswap64(magicNumber);
			}
		break;
		
		defualt:
		cout << "Invalid dataset type specified";
		exit(0);
	}	
	
	file.close();
}


// saves dataset in JPEG form
void idxDatsetReader::saveJPEG(bool datasetType)
{
	// save training images
	if(nDimensions == 2)
	{
		unsigned nDatasets = getNumberOfDatasets();
		char fileName[MAX_FILENAME_CHAR];	
		unsigned int nPixels = sizeOfDimension[0] * sizeOfDimension[1];		
		IplImage opencvImage = cvCreateImage(cvSize(sizeOfDimension[0], sizeOfDimension[1]), IPL_DEPTH_8U, 1);		
		
		for (unsigned int imageId = 0; imageId < nDatasets; imageId++)
			{
				if (datasetType == 0)
					sprintf(fileName, "MNISTDataset/trainImages/%i.jpg", imageId);
				else
					sprintf(fileName, "MNISTDataset/testImages/%i.jpg", imageId);
				
				for (unsigned i = 0; i < nPixels; i++)
					opencvImage->ImageData[i] = data[imageId][i];	
					
				cvSaveImage(fileName, opencvImage);
			}
	}
	else
	cout << "Sorry, not a image dataset :(";
}

