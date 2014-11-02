#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <opencv/highgui.h>
#include "idxDatasetReader.h"

using namespace std;


// I suspect this is not portable :(
// http://stackoverflow.com/questions/280162/is-there-a-way-to-do-a-c-style-compile-time-assertion-to-determine-machines-e
template<class DatasetType>
bool IdxDatasetReader<DatasetType>::isLittleEndian()
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
template<class DatasetType>
unsigned char IdxDatasetReader<DatasetType>::getNumberOfDimensions()
{
	return (magicNumber & 0xFF);
}


// returns number of datsets
template<class DatasetType>
unsigned int IdxDatasetReader<DatasetType>::getNumberOfDatasets()
{
	unsigned int tempNumberOfDatasets;
	file >> tempNumberOfDatasets;
	
	return tempNumberOfDatasets;
}


// returns size of each dimension
template<class DatasetType>
unsigned int* IdxDatasetReader<DatasetType>::getSizeOfDimension()
{	
	unsigned int *size = new unsigned int[nDimensions];
	for (unsigned int i = 0; i < nDimensions; i++)
		file >> size[i];
	return size;
}

// returns acutal filesize 
template<class DatasetType>
long int IdxDatasetReader<DatasetType>::getFileSize()
{
    struct stat stat_buf;
    int rc = stat(fileName.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


// extracts magic number from the idx file
template<class DatasetType>
unsigned int IdxDatasetReader<DatasetType>::getMagicNumber()
{
        unsigned int tempMagicNumber;
	file >> tempMagicNumber;
	
	return tempMagicNumber;
}

// extracts dataset from the input file
template<class DatasetType>
void IdxDatasetReader<DatasetType>::getDataset()
{
	
	file.open(fileName.c_str(), ios::in);
	
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

	long fileSize = getFileSize();
	
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


	data = new DatasetType*[nDatasets];
	for (unsigned int d = 0; d < nDatasets; d++)
		data[d] = new DatasetType[actualDatasetSize];
	

	for (unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
		for (unsigned int byteId = 0; byteId < actualDatasetSize; byteId++)
			{
				file >> data[datasetId][byteId];
				switch(datasetType)
				{
					case 0x0B:
					data[datasetId][byteId] = __builtin_bswap16(data[datasetId][byteId]);
					break;

					case 0x0c:
					data[datasetId][byteId] = __builtin_bswap32(data[datasetId][byteId]);
					break;

					case 0x0d:
					data[datasetId][byteId] = __builtin_bswap64(data[datasetId][byteId]);		
				
					default:
					cout << "\nDatatype not supported";
					exit(0);						
				}
			}	
	file.close();
}


// saves dataset in JPEG form(Debugging purpose)
template<class DatasetType>
void IdxDatasetReader<DatasetType>::saveJPEG(bool datasetType)
{
	// save training images
	if(nDimensions == 2)
	{
		unsigned int nPixels = sizeOfDimension[0] * sizeOfDimension[1];		
		IplImage *opencvImage = cvCreateImage(cvSize(sizeOfDimension[0], sizeOfDimension[1]), IPL_DEPTH_8U, 1);		
		
		for (unsigned int imageId = 0; imageId < nDatasets; imageId++)
			{
				if (datasetType == 0)
					sprintf((char*)fileName.c_str(), "MNISTDataset/trainImages/%i.jpg", imageId);
				else
					sprintf((char*)fileName.c_str(), "MNISTDataset/testImages/%i.jpg", imageId);
				
				for (unsigned int i = 0; i < nPixels; i++)
					opencvImage->imageData[i] = data[imageId][i];	
					
				cvSaveImage((char*)this->fileName.c_str(), opencvImage);
			}
	}
	else
	cout << "Sorry, not a image dataset :(";
}


template void IdxDatasetReader<unsigned char>::getDataset();
template void IdxDatasetReader<char>::getDataset();
template void IdxDatasetReader<short>::getDataset();
template void IdxDatasetReader<int>::getDataset();
template void IdxDatasetReader<float>::getDataset();
template void IdxDatasetReader<double>::getDataset();

template void IdxDatasetReader<unsigned char>::saveJPEG(bool);
template void IdxDatasetReader<char>::saveJPEG(bool);
template void IdxDatasetReader<short>::saveJPEG(bool);
template void IdxDatasetReader<int>::saveJPEG(bool);
template void IdxDatasetReader<float>::saveJPEG(bool);
template void IdxDatasetReader<double>::saveJPEG(bool);
