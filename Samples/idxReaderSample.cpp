#include <iostream>
#include <string>
#include "idxDatasetReader.h"


using namespace std;


int main()
{
	IdxDatasetReader idxReader;
	string fileName;
	
	cout << "Please enter the filename";
	cin >> fileName;

	idxReader.getDataset(fileName);
	idxReader.saveJPEG(0);

	return 0;
}
