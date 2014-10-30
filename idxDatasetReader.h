/*
Specializes DatasetReader class for Idx files defined here(see at the bottom):
http://yann.lecun.com/exdb/mnist/
*/

#include "datasetReader.h"
#include <string>

using namespace std;

template <class DataType>
class IdxDatasetReader : public DatasetReader
{	
	public:
		
		unsigned int magicNumber;
		unsigned int *sizeOfDimension; // variable sizes array containing size for each of nDimensions
		DataType **data; // n X 1 vector where 'n' = nDatasets*nDimensions*PROD(i){sizeOfDimension_{i}}, stored in row-major order		
		void saveJPEG(bool); // 0: training dataset, 1: test dataset
		void getDataset();					
		
		IdxDatasetReader(string fileName, unsigned char datasetType)
		{
			fileName = fileName;
			datasetType = datasetType;
		}

	protected:
	
		unsigned int nDatasets;
		unsigned char nDimensions;	
		unsigned char datasetType;

		unsigned int getMagicNumber();		
		unsigned char getNumberOfDimensions();
		unsigned int getNumberOfDatasets();
		unsigned int* getSizeOfDimension();		
		bool isLittleEndian();
};
