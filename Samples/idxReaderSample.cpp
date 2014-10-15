#include <iostream>
#include <string>
#include "idxDatasetReader.h"


using namespace std;


int main()
{
	IdxDatasetReader idxReader;
	
	cout << "Please enter the filename";
	cin >> idxReader.fileName;

	idxReader.getDataset();
	idxReader.saveJPEG(0);

	return 0;
}
