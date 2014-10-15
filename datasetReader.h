// defines generic class for reading machine learning datasets

#include <string>
#include <fstream>

using namespace std;


class DatasetReader
{
	public:
		
		string fileName;	    
		ifstream file;		
	
		void getDataset();		
};
