#include "naivebayes.h"
#include <fstream>
//#include<windows.h>
using namespace std;
void test() {
	ifstream ftrain("f:\\desktop\\car7.txt");
	ifstream ftest("f:\\desktop\\car3.txt");
	//ifstream fpredict("predict.txt");
	//ofstream fresult("result.txt");

	naivebayes::NaiveBayes nb;
	nb.train(ftrain);
	cout << "F1:\t" << nb.test(ftest);
	//nb.predict(fpredict, fresult);

	ftrain.close();
	ftest.close();

	//fpredict.close();
	//fresult.close();
}
void main() {
	clock_t start = clock();
	test();
	clock_t end = clock();
	cout << endl << (double)(end - start) * 1000 / CLOCKS_PER_SEC << "ms" << endl;
	system("pause");
}