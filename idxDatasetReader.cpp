#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <opencv/highgui.h>
#include "idxDatasetReader.h"

using namespace std;


// I suspect this is not portable :(
// http://stackoverflow.com/questions/280162/is-there-a-way-to-do-a-c-style-compile-time-assertion-to-determine-machines-e
bool IdxDatasetReader::isLittleEndian()
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

// returns the extracted number of dimensions from the magic number
unsigned char IdxDatasetReader::getNumberOfDimensions()
{
	return (magicNumber & 0xFF);
}


// returns number of datsets
unsigned int IdxDatasetReader::getNumberOfDatasets()
{
	unsigned int tempNumberOfDatasets;
	file >> tempNumberOfDatasets;
	
	return tempNumberOfDatasets;
}


// returns size of each dimension
unsigned int* IdxDatasetReader::getSizeOfDimension()
{	
	unsigned int *size = new unsigned int[nDimensions];
	for (unsigned int i = 0; i < nDimensions; i++)
		file >> size[i];
	return size;
}

// returns acutal filesize 
long int getFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


// extracts magic number from the idx file
unsigned int getMagicNumber(string fileName)
{
        unsigned int tempMagicNumber;

        ifstream file(fileName, ios::read|ios::binary);
        
        file >> tempMagicNumber;

        file.close();

        return tempMagicNumber;
}

// extracts dataset from the input file
void IdxDatasetReader::getDataset()
{
	
	this->file.open(fileName.c_str(), ios::in);
	
	//extract magic number
	magicNumber = getMagicNumber();
	
	bool isLittleEndianTrue;
		
	if (isLittleEndian() == true)
		{
			isLittleEndianTrue = true;
			magicNumber = __builtin_bswap32(magicNumber);
		}
	

	// get number of dimensions
	nDimensions = getNumberOfDimensions();
		
	// get size of dimensions
	sizeOfDimension = getSizeOfDimension();	
	
	// size of dataset as implied by file header:
	nDatasets = getNumberOfDatasets();
	
	unsigned int predictedDatasetSize = nDatasets;
	
	for (unsigned int i = 0; i < nDimensions; i++)
		predictedDatasetSize *= sizeOfDimension[i];
		
	unsigned int actualDatasetSize;

	long fileSize = getFileSize(fileName);
	
	if (nDimensions > 1)
		actualDatasetSize = fileSize - sizeof(unsigned int) * (nDimensions + 2); // 1: magic number + 1: number of datasets	
	else
		actualDatasetSize = fileSize - sizeof(unsigned int) * (nDimensions + 1); // 1:magic number
		
	bool hasValidFileSize = (predictedDatasetSize == actualDatasetSize) ? true : false;
			
	// check if file contains number of bytes as implied by file header
	if (hasValidFileSize == false)
	{
		cout << "Actual data < data size implied by the header :(";
		exit(0);			
	}
		
	// read dataset
	switch(datasetType)
	{
		case 0x08:		

			data = new unsigned char*[nDatasets];
			for (unsigned int d = 0; d < nDatasets; d++)
				data[d] = new unsigned char[actualDatasetSize];		
			for(unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
				for (unsigned int byteId = 0; byteId < actualDatasetSize; byteId++)
					data[datasetId][byteId] = file.get(); 
		break;
		
		case 0x09: 
		
			data = new char*[nDatasets];
			for (unsigned int d = 0; d < nDatasets; d++)
				data[d] = new unsigned char[actualDatasetSize];
			for(unsigned int datasetId = 0; datasetId < nDatasets; datasetId++)
				for (unsigned int byteId = 0; byteId < totalDatasetSize; byteId++)
					data = new unsigned char*[nDatasets]; 
		break;
		
		case 0x0B: 
		
			data = new short[nDatasets][totalDatasetSize]; 
			for (unsigned int datasetId = 0; datasetId < nDatasets; datasetId++)
				for (unsigned int byteId = 0; byteId < totalDatasetSize; byteId++)
					{
						data[datasetId][byteId] = file.get(); 
						data[datasetId][byteId] = __builtin_bswap16(magicNumber);
					}
		break;
		
		case 0x0C: 
		
			data = new int[nDatasets][totalDatasetSize];
			for (unsigned int datasetId = 0; datasetId < nDatasets; datasetId++)
				for (unsigned int byteId = 0; byteId < totalDatasetSize; byteId++)
				{
					data[datasetId][byteId] = file.get(); 
					data[datasetId][byteId] = __builtin_bswap32(magicNumber);
				}
		break;
		
		case 0x0D: 

			data = new float[nDatasets][totalDatasetSize];
			for (unsigned int datasetId = 0; datasetId < nDatasets; datasetId++)
				for (unsigned int byteId = 0; byteId < totalDatasetSize; byteId++)
					{
						data[datasetId][byteId] = file.get(); 
						data[datasetId][byteId] = __builtin_bswap32(magicNumber);
					}
		break;
		
		case 0x0E: 

			data = new double[nDatasets][totalDatasetSize];
			for (unsigned int datasetId = 0; datasetId < nDatasets; datasetId++)			
				for (unsigned int byteId = 0; byteId < totalDatasetSize; byteId++)
				{
					data[datasetId][byteId] = file.get(); 
					data[datasetId][byteId] = __builtin_bswap64(magicNumber);
				}
		break;
		
		defualt:
			cout << "Invalid dataset type specified";
			exit(0);
	}	
	
	file.close();
}


// saves dataset in JPEG form(Debugging purpose)
void IdxDatsetReader::saveJPEG(bool datasetType)
{
	// save training images
	if(nDimensions == 2)
	{
		unsigned int nPixels = sizeOfDimension[0] * sizeOfDimension[1];		
		IplImage opencvImage = cvCreateImage(cvSize(sizeOfDimension[0], sizeOfDimension[1]), IPL_DEPTH_8U, 1);		
		
		for (unsigned int imageId = 0; imageId < nDatasets; imageId++)
			{
				if (datasetType == 0)
					sprintf(fileName, "MNISTDataset/trainImages/%i.jpg", imageId);
				else
					sprintf(fileName, "MNISTDataset/testImages/%i.jpg", imageId);
				
				for (unsigned int i = 0; i < nPixels; i++)
					opencvImage->ImageData[i] = data[imageId][i];	
					
				cvSaveImage(this->fileName, opencvImage);
			}
	}
	else
	cout << "Sorry, not a image dataset :(";
}

