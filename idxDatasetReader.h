/*
Specializes DatasetReader class for Idx files defined here(see at the bottom):
http://yann.lecun.com/exdb/mnist/
*/

#include "datasetReader.h"
#include <string>

using namespace std;

class IdxDatasetReader : public DatasetReader
{	
	public:
		
		unsigned magicNumber;
		unsigned int *sizeOfDimension; // variable sizes array containing size for each of nDimensions
		unsigned int *data; // n X 1 vector where 'n' = nDatasets*nDimensions*PROD(i){sizeOfDimension_{i}}, stored in row-major order		
		void saveJPEG(bool); // 0: training dataset, 1: test dataset		
					
	protected:
	
		unsigned int nDatasets;
		unsigned int getMagicNumber();
		char getDatatype();		
		char getNumberOfDimensions();
		unsigned int getNumberOfDatasets();
		unsigned int* getSizeOfDimension();		
	
};
