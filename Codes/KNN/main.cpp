/*abalone*/
#include<map>
#include<vector>
#include<stdio.h>
#include<cmath>
#include<stdlib.h>
#include<time.h>
#include<algorithm>
#include<fstream>
#include<iostream>
#include<string>
#define LINE_NUM 4177//数据集总数
#define LINE_TEST 1253//测试数据
using namespace std;

typedef int tLabel;
typedef double tData;
typedef pair<int,double> PAIR;
const int collen = 8;
const int rowlen = LINE_NUM-LINE_TEST;

class KNN
{
private:
    double dataSet[LINE_NUM][collen];
    int labels[LINE_NUM];
    //char douhao[LINE_NUM*collen];
    double testdata[LINE_TEST][collen];
    int k;
    map<int,map<int,double> >map_index_dis;
    map<int,int>map_label_freq;
    double get_distance(tData *d1,tData *d2);
public:
    KNN(int k);
    void get_all_distance();
    void get_max_freq_label();
    struct CmpbyValue
    {
        bool operator()(const PAIR& lhs,const PAIR& rhs)
        {
            return lhs.second < rhs.second;
        }
    };
};
KNN::KNN(int k)
{
    this->k = k;
    fstream fin;
    fin.open("data.txt",ios::in);
    if(!fin)
    {
        cout<<"can not open the file data.txt"<<endl;
        exit(1);
    }
    //input the random dataset
    int a[LINE_NUM]={0};
    int i,m,j,num;
    srand((unsigned)time(NULL));
    for(i = 0;i<LINE_NUM;i++)
    {
        while(a[m=rand()%LINE_NUM]);
        a[m] = i;
    }
//    for(int i=0;i<LINE_NUM;i++)
//    {
//        cout<<a[i]<<" ";
//    }
 //存入70%数据
    for(num=0;num<LINE_NUM;num++)
    {
        for(j=0;j<collen;j++)
        {
            fin>>dataSet[a[num]][j];
            //fin>>douhao[num*collen+j];
           // cout<<dataSet[a[num]][j]<<" ";
            //cout<<douhao[num*collen+j]<<" ";

        }
        fin>>labels[a[num]];
//        cout<<"label:"<<labels[a[num]]<<endl;
//        cout<<endl;

    }
//随机存入30%待测数据
    for(i = LINE_NUM-LINE_TEST;i < LINE_NUM;i++)
    {
        for(int j = 0;j < collen;j++)
        {
            testdata[i-(LINE_NUM-LINE_TEST)][j]=dataSet[i][j];
        }
    }

}
double KNN::get_distance(tData *d1,tData *d2)
{
    double sum = 0;
    for(int i =0;i<collen;i++)
    {
        sum += pow((d1[i] - d2[i]),2);
    }
    return sqrt(sum);
}

void KNN::get_all_distance()
{
    double distance[LINE_NUM];
    int i,num;
    for(num = 0;num<LINE_TEST;num++)
    {
            for(i = 0;i<rowlen;i++)
        {
            distance[i] = get_distance(dataSet[i],testdata[num]);
            map_index_dis[num].insert( map<int,double>::value_type(i,distance[i]));
        }
    }

}
void KNN::get_max_freq_label()
{
    int tp=0;
    double p=0;
    for(int i=0;i<LINE_TEST;i++)
    {
        vector<PAIR>vec_index_dis(map_index_dis[i].begin(),map_index_dis[i].end());
        sort(vec_index_dis.begin(),vec_index_dis.end(),CmpbyValue());//按照第二个值从小到大的顺序排列
//        cout<<"================================"<<endl;
//        cout<<"数据"<<i<<endl;
        map_label_freq.clear();
        for(int j = 0;j<k;j++)
        {

//           cout<<"the index = "<<vec_index_dis[j].first<<" the distance = "<<vec_index_dis[j].second <<" the label = "<< labels[vec_index_dis[j].first]
//           <<"  the coordinate( "<<dataSet[vec_index_dis[j].first][0]<<","<<dataSet[vec_index_dis[j].first][1]<<","<<dataSet[vec_index_dis[j].first][2]<<","<<dataSet[vec_index_dis[j].first][3]<<")"<<endl;//vec_index_dis[i].first里装的是距离最小的
            map_label_freq[labels[vec_index_dis[j].first]]++;
        }
        map<tLabel,int>::const_iterator map_it = map_label_freq.begin();
        tLabel label;
        int max_freq = 0;
        while(map_it != map_label_freq.end())
        {
            if(map_it->second >= max_freq)
            {
                max_freq = map_it->second;
                label = map_it->first;
            }
            map_it++;
        }
 //       cout<<endl<<endl;
//        cout<<"测试数据"<<"("<<testdata[i][0]<<"  "<<testdata[i][1]<<"  "<<testdata[i][2]<<"  "<<testdata[i][3]<<")"<<endl;
//        cout<<"实际属于："<<labels[i+105]<<endl;
//        cout<<"测试属于："<<label<<endl;
        if(labels[i+LINE_NUM-LINE_TEST] == label)
        {
             tp++;
        }
//        cout<<"================================"<<endl;
//        cout<<endl<<endl;
    }
    p=(double)tp/LINE_TEST;
    cout<<"准确率"<<p<<endl;
}
int main()
{
//    int temp=0;
//    int time,k;
//    for(int k=1;k<1253;k=k+200)
//    {
//        for(time=0;time<10;time++)
//        {
//
//            pvalue[temp][10]+=pvalue[temp][time];
//        }
//        temp++;
//    }
//    cout<<"please input the k value :"<<endl;
//    cin >> k;
//    for(int i=0;i<temp;i++)
//    {
//        cout<<i<<" k="<<1+200*i<<pvalue[i][10]<<endl;
//    }

    int k=600;
    KNN knn(k);
    knn.get_all_distance();
    knn.get_max_freq_label();
   // system("pause");
    return 0;
}
