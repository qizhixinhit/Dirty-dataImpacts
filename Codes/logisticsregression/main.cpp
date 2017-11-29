#include<stdio.h>
#include<cmath>
#include<stdlib.h>
#include<fstream>
#include<iostream>
#include<string>

#define LINE_NUM 100
#define LINE_TEST 30
#define NUM_ATT 4
#define rowlen 70

using namespace std;

//const int rowlen = LINE_NUM - LINE_TEST;
double tual[NUM_ATT+1];
double alpha = 0.001;

class logistic{

private:
	double deibaSet[LINE_NUM][NUM_ATT+1];
	double dataSet[LINE_NUM][NUM_ATT+1];
	string labels[LINE_NUM+1];
	char labelss[LINE_NUM];
	//int labels[LINE_NUM];
	string labelsT[LINE_TEST];
	char points[LINE_NUM*NUM_ATT];
	char pointsT[LINE_TEST*NUM_ATT];
	double testData[LINE_TEST][NUM_ATT];
	int hashe[2000];


public:
	void ReadTXT();
	void ReadAB();
	void ReadCar();
	void updateCeta();
	int judgeClass(string a);
	//int judgeClass(int a);
	int judgey(double k);
	//void test();

};

//void logistic::ReadCar(){
//	fstream fin;
//	fin.open("car.txt", ios::in);
//	if (!fin)
//	{
//		cout << "can not open the file car.txt" << endl;
//		exit(1);
//	}
//	for (int num = 1; num <= LINE_NUM-1; num++)
//	{
//		for (int j = 1; j <= NUM_ATT; j++)
//		{
//            fin>>dataSet[num][j];
//            fin >> points[num*NUM_ATT + j];
//			//cout << dataSet[num][j] << " ";
//		}
//		dataSet[num][0] = 1;
//		fin >> labels[num];
//        //cout << labels[num] << endl;
//	}
//}

//void logistic::ReadAB(){
//	fstream fin;
//	int r=0;
//	fin.open("abtraindata.txt", ios::in);
//	if (!fin)
//	{
//		cout << "can not open the file abtraindata.txt" << endl;
//		exit(1);
//	}
//	for (int num = 1; num <= LINE_NUM; num++)
//	{
//        fin >> labelss[num];
//		if(labelss[num]=='M')
//            deibaSet[num][1]=0;
//        else if(labelss[num]=='F')
//            deibaSet[num][1]=1;
//        else if(labelss[num]=='I')
//            deibaSet[num][1]=2;
//        //cout<<dataSet[num][1]<<" ";
//		for (int j = 2; j <= NUM_ATT+1; j++)
//		{
//			fin >> points[num*NUM_ATT + j];
//			fin >> deibaSet[num][j];
//			if(deibaSet[num][j]==10||deibaSet[num][j]==11)
//            {
//                hashe[++r]=num;
//            }
//            //cout << deibaSet[num][j] << " ";
//		}
//		//cout<<endl;
//
//	}
//	cout<<r<<endl;
//	for(int m=1;m<=r;m++)
//    {
//        for(int n=1;n<=NUM_ATT;n++)
//        {
//            dataSet[m][n]=deibaSet[hashe[m]][n];
//            //cout<<dataSet[m][n]<<" ";
//        }
//        labels[m]=deibaSet[hashe[m]][9];
//        //cout<<labels[m]<<endl;
//        dataSet[m][0]=1;
//    }
//}

void logistic::ReadTXT(){
	fstream fin;
	fin.open("traindata.txt", ios::in);
	if (!fin)
	{
		cout << "can not open the file traindata.txt" << endl;
		exit(1);
	}
	for (int num = 1; num <= rowlen; num++)
	{
		for (int j = 1; j <= NUM_ATT; j++)
		{
			fin >> dataSet[num][j];
			fin >> points[num*NUM_ATT + j];
			cout << dataSet[num][j] << " ";
		}
		dataSet[num][0] = 1;
		fin >> labels[num];
		cout << labels[num] << endl;
	}
}
void logistic::updateCeta(){
	int time = 0;
	double total[rowlen] = { 0 };
	double sum[NUM_ATT + 1] = { 0 };
	int s;
	double ratio=0.5;
	double tr,tt;
	int ss;
	int k;
	double sk=0;
	int even = 0;
	string st;
	double ceta[NUM_ATT+1]={0};
	do{
        tr=ceta[4];
        for (int i = 1; i <= rowlen; i++)
        {
            for (int j = 0; j <= NUM_ATT; j++)
                total[i] += ceta[j] * dataSet[i][j];
        }
        for (s = 0; s <= NUM_ATT; s++){
            for (int ii = 1; ii <= rowlen; ii++)
                sum[s] += (1.0 / (1 + exp(-total[ii])) - judgeClass(labels[ii]))*dataSet[ii][s];
            ceta[s] -= alpha*sum[s];
            cout << ceta[s] << " ";
        }
        for (int ie = 1; ie <= rowlen; ie++)
            total[ie] = 0;
        for (int sw = 0; sw <= NUM_ATT; sw++)
            sum[sw] = 0;
		cout << endl;
		for (k = 1; k<=rowlen; k++)
		{
			for (ss = 0; ss<=NUM_ATT; ss++)
			{
				sk += ceta[ss] * dataSet[k][ss];
			}
			if (judgeClass(labels[k]) == judgey(1.0 / (1 + exp(-sk))))
                even++;
            sk=0;
		}
		ratio = (double)even / rowlen;
		even = 0;
		tt=ceta[4];
		time++;
		cout << "第" << time << "次迭代，正确率为" << ratio << endl;
	//} while (!(fabs(tr-tt)>0&&fabs(tr-tt)<1e-5));
        }while(time!=7);
	for(int f=0;f<=NUM_ATT;f++)
        tual[f]=ceta[f];
    fstream fin;
	fin.open("testdata.txt", ios::in);
	if (!fin)
	{
		cout << "can not open the file testdata.txt" << endl;
		exit(1);
	}
	for (int num = 1; num <= LINE_TEST; num++)
	{
		for (int j = 1; j <= NUM_ATT; j++)
		{
			fin >> testData[num][j];
			fin >> pointsT[num*NUM_ATT + j];
			cout << testData[num][j] << " ";
		}
		testData[num][0] = 1;
		fin >> labelsT[num];
		cout << labelsT[num] << endl;
	}
	int timet=0;
	double uv=0;
	for(int u=1;u<=LINE_TEST;u++)
    {
        for(int v=0;v<=NUM_ATT;v++)
            uv+=tual[v]*testData[u][v];
        if(judgey(1.0/(1+exp(-uv)))==0)
            cout<<"测试分类为setosa"<<" ";
        else
            cout<<"测试分类为versicolor"<<" ";
        if(judgeClass(labelsT[u])==0)
            cout<<"实际分类为setosa"<<endl;
        else
            cout<<"实际分类为versicolor"<<endl;
        if(judgey(1.0/(1+exp(-uv)))==judgeClass(labelsT[u]))
            timet++;
        uv=0;
    }
    cout<<"正确率为"<<(double)timet/LINE_TEST<<endl;
//	int timet=0;
//	double uv=0;
//	for(int u=1116;u<=1593;u++)
//    {
//        for(int v=0;v<=NUM_ATT;v++)
//            uv+=tual[v]*dataSet[u][v];
////        if(judgey(1.0/(1+exp(-uv)))==0)
////            cout<<"测试分类为10"<<" ";
////        else
////            cout<<"测试分类为11"<<" ";
////        if(judgeClass(labels[u])==0)
////            cout<<"实际分类为10"<<endl;
////        else
////            cout<<"实际分类为11"<<endl;
//        if(judgey(1.0/(1+exp(-uv)))==judgeClass(labels[u]))
//            timet++;
//        uv=0;
//    }
//    cout<<"正确率为"<<(double)timet/479.0<<endl;
    exit(0);
}

int logistic::judgeClass(string a)
{
	if (a == "Iris-setosa")
		return 0;
	else
		return 1;
}
//int logistic::judgeClass(string a)
//{
//	if (a == "unacc")
//		return 0;
//	else
//		return 1;
//}
//int logistic::judgeClass(int a)
//{
//	if (a == 10)
//		return 0;
//	else
//		return 1;
//}
int logistic::judgey(double k)
{
	if (k <= 0.5)
		return 0;
	else
		return 1;
}

//void logistic::test()
//{
//    fstream fin;
//	fin.open("testdata.txt", ios::in);
//	if (!fin)
//	{
//		cout << "can not open the file testdata.txt" << endl;
//		exit(1);
//	}
//	for (int num = 1; num <= LINE_TEST; num++)
//	{
//		for (int j = 1; j <= NUM_ATT; j++)
//		{
//			fin >> testData[num][j];
//			fin >> pointsT[num*NUM_ATT + j];
//			cout << testData[num][j] << " ";
//		}
//		testData[num][0] = 1;
//		fin >> labelsT[num];
//		cout << labelsT[num] << endl;
//	}
//	int timet=0;
//	double uv=0;
//	for(int u=1;u<=LINE_TEST;u++)
//    {
//        for(int v=0;v<=NUM_ATT;v++)
//            uv+=tual[v]*testData[u][v];
//        if(judgey(1.0/(1+exp(-uv)))==0)
//            cout<<"测试分类为setosa"<<" ";
//        else
//            cout<<"测试分类为versicolor"<<" ";
//        if(judgeClass(labelsT[u])==0)
//            cout<<"实际分类为setosa"<<endl;
//        else
//            cout<<"实际分类为versicolor"<<endl;
//        if(judgey(1.0/(1+exp(-uv)))==judgeClass(labelsT[u]))
//            timet++;
//        uv=0;
//    }
//    cout<<"正确率为"<<(double)timet/LINE_TEST<<endl;
//}

int main(){
	cout << "当前数据集为：" << endl;
	logistic lr;
	lr.ReadTXT();
	//lr.ReadAB();
	//lr.ReadCar();
	lr.updateCeta();
	//lr.test();
	return 0;
}
