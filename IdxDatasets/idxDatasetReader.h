/*
Specializes DatasetReader class for Idx files defined here(see at the bottom):
http://yann.lecun.com/exdb/mnist/
*/

#include "../datasetReader.h"
#include <string>

using namespace std;

template <class DataType>
class IdxDatasetReader : public DatasetReader
{	
	public:
		
		unsigned int magicNumber;
		unsigned int *sizeOfDimension; // variable sizes array containing size for each of nDimensions
	
	 	void saveJPEG(bool); // 0: training dataset, 1: test dataset
		
		IdxDatasetReader(string inputFileName, unsigned char inputDatasetType)
		{
			fileName = inputFileName;
			datasetType = inputDatasetType;			
		}
			
		void getDataset();
	protected:
	
		unsigned int nDatasets;
		unsigned char nDimensions;	
		unsigned char datasetType;
		DataType **data;

		unsigned int getMagicNumber();		
		long int getFileSize();
		unsigned char getNumberOfDimensions();
		unsigned int getNumberOfDatasets();
		unsigned int* getSizeOfDimension();		
		bool isLittleEndian();
};
