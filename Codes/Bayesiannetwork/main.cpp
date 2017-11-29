#include <iostream>
#include <stdlib.h>
#include<sys/time.h>
#include "bayesiannetwork.h"
#include "naivebayesian.h"
using namespace std;

int main(){

int method=0;
//char* train;
//char* input;
//char* cfg;

if( method == 0 ){
    struct timeval tpstart,tpend;//tpstart1,tpend1,tpstart2,tpend2;
    float timeuse;//timeuse1,timeuse2;
    gettimeofday(&tpstart,NULL);
	baysian::naivebayesian naive(0);
	gettimeofday(&tpend,NULL);
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
    tpend.tv_usec-tpstart.tv_usec;
    timeuse/=1000000;
	std::cout<<timeuse<<" "<<endl;
//	gettimeofday(&tpstart1,NULL);
//    baysian::naivebayesian naive1(0);
//    gettimeofday(&tpend1,NULL);
//    timeuse1=1000000*(tpend.tv_sec-tpstart.tv_sec)+
//    tpend.tv_usec-tpstart.tv_usec;
//    timeuse1/=1000000;
//    std::cout<<timeuse1<<" "<<endl;
//
//    gettimeofday(&tpstart2,NULL);
//    baysian::naivebayesian naive2(0);
//	gettimeofday(&tpend2,NULL);
//	timeuse2=1000000*(tpend.tv_sec-tpstart.tv_sec)+
//    tpend.tv_usec-tpstart.tv_usec;
//    timeuse2/=1000000;
//    std::cout<<timeuse2<<" "<<endl;

}else if( method == 1 ){
    struct timeval tpstart,tpend;
    float timeuse;

    gettimeofday(&tpstart,NULL);
	baysian::bayesiannetwork bnetwork(1);
	gettimeofday(&tpend,NULL);
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
    tpend.tv_usec-tpstart.tv_usec;
    timeuse/=1000000;
    std::cout<<timeuse<<" ";
}

return 0;
}
