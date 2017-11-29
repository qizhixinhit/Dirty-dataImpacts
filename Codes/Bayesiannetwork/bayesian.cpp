#include "bayesian.h"
#include <iostream>
#define  LEI 2

namespace baysian{

//calculate the accuracy
void bayesian::accuracy(int *outcome , int * result)
{
	//double correct=0;// store the number of correct predictions
    int lei[LEI]={0};
    int tp[LEI]={0};
    int tt[LEI]={0};

	for( int i=0 ; i<testinstances; i++)//count the number of correct predictions
	{
		//std::cout<<"outcome"<<testinstances<<std::endl;
		if(outcome[i] == 1)
        {
            tt[0]++;
        }

        else if(outcome[i] == 2)
            tt[1]++;
//        else if(outcome[i] == 3)
//            tt[2]++;
//        else if(outcome[i] == 4)
//            tt[3]++;
//        else if(outcome[i] == 5)
//            tt[4]++;
//        else if(outcome[i] == 6)
//            tt[5]++;
//        else if(outcome[i] == 7)
//            tt[6]++;
//        else if(outcome[i] == 8)
//            tt[7]++;


        if(result[i] == 1)
            lei[0]++;
        else if(result[i] == 2)
            lei[1]++;
//        else if(result[i] == 3)
//            lei[2]++;
//        else if(result[i] == 4)
//            lei[3]++;
//        else if(result[i] == 5)
//            lei[4]++;
//        else if(result[i] == 6)
//            lei[5]++;
//        else if(result[i] == 7)
//            lei[6]++;
//        else if(result[i] == 8)
//            lei[7]++;

		if (outcome[i]==result[i]&&outcome[i]==1)
			tp[0]++;
        else if(outcome[i]==result[i]&&outcome[i]==2)
            tp[1]++;
//        else if (outcome[i]==result[i]&&outcome[i]==3)
//            tp[2]++;
//        else if (outcome[i]==result[i]&&outcome[i]==4)
//            tp[3]++;
//        else if (outcome[i]==result[i]&&outcome[i]==5)
//            tp[4]++;
//        else if (outcome[i]==result[i]&&outcome[i]==6)
//            tp[5]++;
//        else if (outcome[i]==result[i]&&outcome[i]==7)
//            tp[6]++;
//        else if (outcome[i]==result[i]&&outcome[i]==8)
//            tp[7]++;



		//std::cout<<"predict to be "<<outcome[i]<<" is actually "<<result[i]<<std::endl;
	}

	//std::cout<<"Total "<<testinstances<<" data have "<<correct<<" correct predictions"<< std::endl;
	//double percentage=correct/testinstances; // calculate the accuracy
	//std::cout<<"Accuracy is "<<percentage*100<<"%"<<std::endl;
	double p[LEI]={0},r[LEI]={0},fm[LEI]={0};
        double avr=0.0,avp=0.0,avf=0.0;
        int j;
        for(j=0;j<LEI;j++)
        {

                if(tt[j]==0)
                    p[j]=0;
                else
                {
                   p[j]=double(tp[j])/tt[j];
                }
                if(lei[j]==0)
                    r[j]=0;
                else
                {
                   r[j]=double(tp[j])/lei[j];
                }
                avp = avp+p[j];
    //            cout<<"p"<<p[i]<<endl;
    //            cout<<"avp"<<avp<<endl;
                avr = avr+r[j];
    //            cout<<"r"<<r[i]<<endl;
    //            cout<<"avr"<<avr<<endl;
                if(r[j]==0 &&p[j]==0)
                {
                    fm[j]=0;
                }
                else
                fm[j]=(2*p[j]*r[j])/(r[j]+p[j]);

                std::cout<<lei[j]<<" "<<tt[j]<<" "<<tp[j]<<std::endl;

        }
        for(j=0;j<LEI;j++)
        {
            avf=avf+fm[j];
           // cout<<"avf"<<avf<<endl;
        }
        avr = avr/LEI;
        avp = avp/LEI;
        avf = avf/LEI;
        std::cout<<avp<<" "<<avr<<" "<<avf<<" ";
}


}// end of namespace bayesian
