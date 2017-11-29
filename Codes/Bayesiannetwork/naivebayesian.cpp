#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <math.h>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "naivebayesian.h"


namespace baysian{

//initialize all the information we need from training data
naivebayesian::naivebayesian(int a)
{
	a=a+1;
	std::cout<<"Run NaiveBayesian"<<std::endl;
	std::ifstream configure;
        configure.open("adult_cfg.txt",std::ios::in);
        if(!configure){std::cout<<"Can't open configuration file!"<<std::endl;return;}

	configure>>traininstances>>testinstances>>attributes; // read the number of training instances and attributes

	int *discrete = new int[attributes];
	//this array store the information about each attribute is continuous or not
	for(int z=0; z<attributes ; z++)     //  read the information about continuous or not
		configure>>discrete[z];


	int *numclass= new int[attributes+1];
	//this array store the number of classes of each attribute
	for(int b=0; b<=attributes; b++){     // read the number of classes
		configure>>numclass[b];
		//if(discrete[b])// set numclass as 2 for continuous data
			//numclass[b]=2;
	}

	double *count = new double[numclass[attributes]];
	//this array store the total number of each decision's class in training data
	for(int c=0; c<numclass[attributes]; c++)
		count[c]=0;

	configure.close();


	std::ifstream trainingDataFile;
	std::string Buf;
        trainingDataFile.open("adult_train.txt",std::ios::in);
        if(!trainingDataFile){std::cout<<"Can't open training data file!"<<std::endl;return;}

	//this "probabilityTable" store the count of every possible combination
	//and divide each of them by the total occurences
	long double** probabilityTable = new long double*[(attributes*numclass[attributes])];
	for(int j=0; j<attributes; j++)
	{
		if (discrete[j]==1)// if this attribute is discrete
		{
			for(int i=(j*numclass[attributes]) ; i<(j*numclass[attributes]+numclass[attributes]); i++)
				probabilityTable[i]=new long double[numclass[j]];
		}
		else if (discrete[j]==0)//if this attribute is continuous
		{
			for(int i=(j*numclass[attributes]);i<(j*numclass[attributes]+numclass[attributes]);i++)
				probabilityTable[i]=new long double[2];
			//the first one store mean , the second store the standard deviation
		}
	}


	//initialize the probabilityTable to be 0
	for(int r=0 ; r<attributes; r++)
	{
		if(discrete[r]==1)
		{
			for( int g=(r*numclass[attributes]);g< (r*numclass[attributes]+numclass[attributes]) ;g++)
			{
				for(int e=0; e <numclass[r]; e++)
					probabilityTable[g][e]=0;
			}
		}
		else if (discrete[r]==0)
		{
			for( int gg=(r*numclass[attributes]);gg<(r*numclass[attributes]+numclass[attributes]);gg++)
			{
				for(int e=0; e < 2; e++)
					probabilityTable[gg][e]=0;
			}
		}
	}

	//use a array to store each instance for further processing
	double *oneLine = new double [attributes+1];

	//store the information of each instance into probabilityTable
	for( int i=1 ; i<=traininstances; i++)
	{
		//getline( trainingDataFile, Buf );
		std::string Buf;
        //std::cout<<i<<std::endl;
		for (int y=0 ; y<=attributes ; y++){//read one instance for processing
			//getline( lineStream, Buf);
			//oneLine[y]=stod(Buf);
			trainingDataFile>>Buf;
			const char *p = Buf.data();
            oneLine[y]=atof(p);

		}

		count[static_cast<int>(oneLine[attributes]) -1 ]++;//count the result


		for( int jj=0 ; jj<attributes;jj++)
		{
			if(discrete[jj]==1)// if this attribute is discrete
			{
				probabilityTable[jj*numclass[attributes]+static_cast<int>(oneLine[attributes])-1]
				[static_cast<int>(oneLine[jj])-1]++;
			}
			else if (discrete[jj]==0)//if this attribute is continuous
			{
                //std::cout<<i<<" "<<"jj"<<jj<<" "<<jj*numclass[attributes]+static_cast<int>(oneLine[attributes])-1<<std::endl;
				probabilityTable[ jj*numclass[attributes]+static_cast<int>(oneLine[attributes])-1 ]
				[0]+=oneLine[jj];
				probabilityTable[ jj*numclass[attributes]+static_cast<int>(oneLine[attributes])-1 ]
				[1]+=pow( oneLine[jj] , 2 ) ;
			}
		}

	}
	//for( int m=0 ;m<attributes*numclass[attributes];m++)
      //  {
            //if(probabilityTable[m][0]==0)
//            {
//                 probabilityTable[m][0]=-1;
//                 std::cout<<m<<"===="<<std::endl;
//            }
//
//            if(probabilityTable[m][1]==0)
//            {
//                probabilityTable[m][1]=-1;
//                 std::cout<<m<<"===="<<std::endl;
//            }

            //std::cout<<m<<" "<<probabilityTable[m][0]<<" "<<probabilityTable[m][1]<<std::endl;
       // }

	//delete [] oneLine;
        trainingDataFile.close();

	//processing the information in the protalbe to get the proabability
	for( int t=0 ; t< attributes ; t++)
	{
		if (discrete[t]==1)// if this attribute is discrete
		{
			for ( int d=0 ; d<numclass[attributes] ; d++)
			{
				int correction=0;
				for (int o=0 ; o < numclass[t] ; o++)
				//this loop judge weather there is zero occurence of some conjuction
				//if it dose, then do Laplacian correction
				{
					if (probabilityTable[(t*numclass[attributes]+d)][o]==0 )
					{
						correction=numclass[t];
						for(int p=0 ; p <numclass[t] ; p++)
						{
							probabilityTable[(t*numclass[attributes]+d)][p]++;
						}
						break;
					}
				}
				for ( int w=0 ; w<numclass[t] ; w++)
				//claculate every conjuction's contribution of probability
				{
					probabilityTable[(t*numclass[attributes]+d)][w]/=(count[d]+correction);
				}
			}
		}
		else if (discrete[t]==0)
		//if this attribute is continuous,we assume it's Gaussian distribution
		//claculate the mean and standard deviation of each continuous attribute
		{
			for (int h=0 ; h < numclass[attributes] ; h++)
			{

				long double a0=pow( probabilityTable[(t*numclass[attributes]+h)][0]/count[h] , 2 );
				//std::cout<<t*numclass[attributes]+h<<" "<<"a0 "<<a0<<std::endl;
				long double a1=probabilityTable[(t*numclass[attributes]+h)][1]/count[h]-a0;
				//std::cout<<t*numclass[attributes]+h<<" "<<"a1 "<<a1<<std::endl;
				//long double a2=a1/count[h];
				//std::cout<<t*numclass[attributes]+h<<" "<<"a2 "<<a2<<std::endl;
				long double a3=sqrt(a1);
				//std::cout<<t*numclass[attributes]+h<<" "<<"a3 "<<a3<<std::endl;
				//std::cout<<t*numclass[attributes]+h<<std::endl;
				//std::cout<<"                         "<<count[6]<<std::endl;
				probabilityTable[(t*numclass[attributes]+h)][1]=a3;
				//std::cout<<"               "<<probabilityTable[(t*numclass[attributes]+6)][0]<<std::endl;
				probabilityTable[(t*numclass[attributes]+h)][0]/=count[h];
				//std::cout<<t*numclass[attributes]+h<<" "<<probabilityTable[(t*numclass[attributes]+h)][0]<<" "<<probabilityTable[(t*numclass[attributes]+h)][1]<<" "<<count[h]<<std::endl;
			}

		}

		//std::cout<<t<<" "<<probabilityTable[t][0]<<" "<<probabilityTable[t][1]<<std::endl;
	}

     std::cout<<"======================================"<<std::endl;
	//calculate the probability of each resulting class
	for ( int ppp=0 ; ppp<numclass[attributes] ; ppp++)
		count[ppp]=count[ppp]/traininstances;

	classifier(probabilityTable , numclass ,  count , discrete);
	//call function for classification

	//release the memory
//	for( int x=0; x<(attributes*numclass[attributes]) ; x++)
//		delete [] probabilityTable[x];
//
//	delete [] probabilityTable;
//	delete [] discrete;
//	delete [] numclass;
//	delete [] count;


}


//calculate the probability of each choice and choose the greatest one as our prediction
void naivebayesian::classifier(long double** probabilityTable,int*numclass ,double* count ,int *discrete)
{
	std::ifstream testInputFile;
	testInputFile.open("adult_test.txt",std::ios::in);
	if(!testInputFile){std::cout<<"Can't open test data file!"<<std::endl;return;}

	std::string Buf;


	int *result= new int[testinstances]; //this array store the real result for comparison
	for(int w=0; w<testinstances; w++)
	{
		result[w]=0;
	}

	int *outcome=new int[testinstances]; //this array store our prediciton
	for(int f=0; f<testinstances; f++)
	{
		outcome[f]=0;
	}

	double *oneLine=new double [attributes+1]; //store each instance for processing

	long double *decision=new long double[numclass[attributes]];
	// store the probability of each choice

	for( int a=0 ; a<testinstances ; a++)
	{
		for ( int m=0 ; m<numclass[attributes]; m++)
			decision[m]=1;
		//set the array's entries as 1 for each testing instance

		//getline( testInputFile , Buf );
		//std::stringstream  lineStream(Buf);

		for (int u=0 ; u< attributes; u++){
			//getline( lineStream, Buf , ' ' );
			//oneLine[u]=stod(Buf);
			testInputFile>>Buf;
			const char *p = Buf.data();
            oneLine[u]=atof(p);
		}
		// read one instance for prediction

		//getline( lineStream, Buf , ' ' );
		//result[a]=stod(Buf);
		testInputFile>>Buf;
		const char *p = Buf.data();
            result[a]=atof(p);
		// store the result
        //std::cout<<numclass[attributes]<<std::endl;
		for( int x=0 ; x<numclass[attributes] ; x++)
		{

			for( int j=0 ; j<attributes ; j++)
			{
				if(discrete[j]==1)// if this attribute is discrete
				{

					decision[x] *= probabilityTable[(j*numclass[attributes])+x][static_cast<int>(oneLine[j])-1];
				}
				else if (discrete[j]==0)
				// if this attribute is continuous , then use the Gaussian distribution formular to calculate it's contribution of probability
				{
                    //if(probabilityTable[(j*numclass[attributes])+x][1] == 0)
                      //  probabilityTable[(j*numclass[attributes])+x][1]=1;
					//std::cout<<"!!"<<(j*numclass[attributes])+x<<" "<<probabilityTable[(j*numclass[attributes])+x][0]<<" "<<probabilityTable[(j*numclass[attributes])+x][1]<<std::endl;
					long double a0=-pow( ( oneLine[j] - probabilityTable[(j*numclass[attributes])+x][0] ) , 2 );
					//std::cout<<j*numclass[attributes]+x<<" "<<"a0 "<<a0<<std::endl;
					long double a1=2 * pow( probabilityTable[(j*numclass[attributes])+x][1] , 2 );
					//std::cout<<j*numclass[attributes]+x<<" "<<"a1 "<<a1<<std::endl;
//					if(a1 == 0)
//                        a1=1;
					long double a2=a0/a1;
					//std::cout<<j*numclass[attributes]+x<<" "<<"a2 "<<a2<<std::endl;
					long double a3=exp(a2);
					//std::cout<<j*numclass[attributes]+x<<" "<<"a3 "<<a3<<std::endl;
					long double a4 =( 0.39894228/probabilityTable[(j*numclass[attributes])+x][1] )*a3;
					//std::cout<<j*numclass[attributes]+x<<" "<<"a4 "<<a4<<std::endl;
					//int isnan(probabilityTable[(j*numclass[attributes])+x][1]);
					if(probabilityTable[(j*numclass[attributes])+x][1]== 0 || probabilityTable[(j*numclass[attributes])+x][0] == 0   )
                        a4 = 0.00001;
					decision[x] *= a4;
                    //std::cout<<"decision"<<" "<<a0<<std::endl<<a1<<std::endl<<a2<<std::endl<<a3<<std::endl<<a4<<std::endl<<decision[x]<<std::endl<<std::endl;
				}

			}
			decision[x]*=count[x];
		}


		//decide which choice has the highest probability
		int big=0;
		long double hug=decision[0];
		for ( int v=1 ; v<numclass[attributes] ; v++)
		{
			if ( decision[v]>hug)
			{
				big=v;
				hug=decision[v];
			}
		}
		outcome[a]=(big+1);

	}

	accuracy ( outcome , result );
	//call function "caauracy" to calculate the accuracy

	//release memory
//	delete [] result;
//	delete [] decision;
//	delete [] oneLine;
//	delete [] outcome;
}

}// end of namespace bayesian
