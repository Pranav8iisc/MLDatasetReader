#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <opencv/highgui.h>
#include "idxDatasetReader.h"

using namespace std;





unsigned int reverseInt (unsigned int i)
{
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((unsigned int)c1 << 24) + ((unsigned int)c2 << 16) + ((unsigned int)c3 << 8) + c4;
}

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
	cout << "\nMagic number is:" << magicNumber;
	unsigned char tempNDimensions = (magicNumber & 0xFF);
	cout << "\nNumber of dimensions are:" << (unsigned int)tempNDimensions;
	return tempNDimensions;
}


// returns number of datsets
template<class DatasetType>
unsigned int IdxDatasetReader<DatasetType>::getNumberOfDatasets()
{
	unsigned int tempNumberOfDatasets;
	file.read((char*)&tempNumberOfDatasets, sizeof(tempNumberOfDatasets));
	
//	bool isLittleEndianTrue = false;

//	if (isLittleEndian() == true)
//	{
//		isLittleEndianTrue = true;
		tempNumberOfDatasets = reverseInt(tempNumberOfDatasets);
///	}

	return tempNumberOfDatasets;
}


// returns size of each dimension
template<class DatasetType>
unsigned int* IdxDatasetReader<DatasetType>::getSizeOfDimension()
{	
  //     bool isLittleEndianTrue = false;

//	if (isLittleEndian() == true)
//		isLittleEndianTrue = true;		
	

	unsigned int *size = new unsigned int[nDimensions];
	
//	if (isLittleEndianTrue)
		for (unsigned int i = 0; i < nDimensions; i++)
		{
			file.read((char*)&size[i], sizeof(size[i]));
			size[i] = reverseInt(size[i]);
		}

//	else
//		for (unsigned int i = 0; i < nDimensions; i++)
//			file.read((char*)&size[i], sizeof(size[i]));
		
	return size;
}

// returns actual filesize 
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
	file.read((char*)&tempMagicNumber, sizeof(tempMagicNumber));

//	bool isLittleEndianTrue = false;

//	if (isLittleEndian() == true)
//	{
//		isLittleEndianTrue = true;
		tempMagicNumber = reverseInt(tempMagicNumber);
//	}

	return tempMagicNumber;
}

// extracts dataset from the input file
template<class DatasetType>
void IdxDatasetReader<DatasetType>::getDataset()
{
	
	file.open(fileName.c_str(), ios::in);
	
	//extract magic number
	magicNumber = getMagicNumber();
	
	// get number of dimensions
	nDimensions = getNumberOfDimensions();
	cout << "\nNumber of dimensions:" <<(unsigned int) nDimensions;	
	
	// get size of dimensions
	sizeOfDimension = getSizeOfDimension();	
	
	// size of dataset as implied by file header:
	//nDatasets = getNumberOfDatasets();
	nDatasets = sizeOfDimension[0];
       	cout << "\nNumber of datasets are:" << nDatasets;

	if (nDimensions == 3)
	{
		cout << "\nNumber of rows:" << sizeOfDimension[1];
		cout << "\nNumber of columns:" << sizeOfDimension[2];
	}

	unsigned int predictedDatasetSize = nDatasets;
	
	for (unsigned int i = 0; i < nDimensions; i++)
		predictedDatasetSize *= sizeOfDimension[i];
		
	unsigned int actualDatasetSize;

	long fileSize = getFileSize();

	data = new DatasetType*[nDatasets];
	
	if (nDimensions == 1)
		actualDatasetSize = sizeOfDimension[0];
	else
		for (unsigned int i = 1; i < nDimensions; i++)
			actualDatasetSize *= sizeOfDimension[i];

//	actualDatasetSize = sizeOfDimension[1]*sizeOfDimension[2];

	for (unsigned int d = 0; d < nDatasets; d++)
		data[d] = new DatasetType[actualDatasetSize];
		

	for (unsigned datasetId = 0; datasetId < nDatasets; datasetId++)
		for (unsigned int byteId = 0; byteId < actualDatasetSize; byteId++)
			{
				file.read((char*)&data[datasetId][byteId], sizeof(data[datasetId][byteId]));
				switch(datasetType)
				{
					case 0x08:
					case 0x09:
					break;

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

	char files[500];

	// save training images
	if(nDimensions == 3)
	{
		unsigned int nPixels = sizeOfDimension[1] * sizeOfDimension[2];		
		IplImage *opencvImage = cvCreateImage(cvSize(sizeOfDimension[1], sizeOfDimension[2]), IPL_DEPTH_8U, 1);		
		
		cout << "\nNumber of datasets:" << nDatasets;
		for (unsigned int imageId = 0; imageId < nDatasets; imageId++)
			{
				//if (datasetType == 0)
				//	sprintf((char*)fileName.c_str(), "MNIST/trainImages/%i.jpg", imageId);
				//else
					sprintf(files, "MNIST/testImages/%d.jpg", imageId);
				
				for (unsigned int i = 0; i < nPixels; i++)
					opencvImage->imageData[i] = data[imageId][i];	
					
				cvSaveImage(files, opencvImage);
			}
		cout << "\nDONE!!";
	}
	else
	cout << "\nSorry, not a image dataset :(";
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
