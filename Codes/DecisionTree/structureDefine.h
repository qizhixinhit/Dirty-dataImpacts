#ifndef STRUCTUREDEFINE_H
#define  STRUCTUREDEFINE_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <time.h>
#include <fstream>
#include <windows.h>
#include <winnt.h>
#define N 100
using namespace std;
typedef struct instance
{
	char spiltAttrName[N][N];
	char spiltAttr[N][N];
	double spiltAttrNum[N];
	char categorization[N];//目的属性分类
	struct instance* next;
	struct instance* pre;
}Instance;
typedef struct attr
{
	char chr[N];//属性名
	int partition;//该分裂属性下对训练元集划分的个数,若partition为0，则为连续型属性
	char partitionName[N][N];
	double spiltPoint;//连续型属性的分裂点
	int list[N][N];//除第0行外每行代表一个划分，每行第一个元素记录该划分中元素的总个数，其后的元素记录在目的划分D不同划分集下的元素个数,第0行，0号单元存储训练集总个数，后面依次存储该属性的不同划分集中元素个数。
}attribute;
ifstream fileData;
ifstream fileAttributes("DecisionTree//attribute.txt");
ifstream fileInput("DecisionTree//classValue.txt");//读取目的划分的类别名称
int n;
bool isvisited[N] = { 0 };//用来控制建树过程中某属性是否已访问
typedef struct node
{
	struct node* child[N];
	int brands;//说明分支的个数，若为0则为终节点
	Instance* head;
	char decisionName[N];
	int decisionSubscript;//该下标用来指定具体是哪个属性
	char endName[N];//终节点的标签名
}DTree; 

double Entropy(int partition, int list[])//计算属性的熵,num为集合的基数
{
	int n = partition;
	double entropy = 0;
	for (int i = 1; i <= n; i++)
	{
		double temp = ((double)(list[i]) / list[0]);
		if (temp == 0) continue;
		entropy += temp*(log10(temp) / log10(2));
	}
	if (entropy == 0) return 0;
	return -entropy;
}

double info(attribute spilt, int partition)//计算某分裂属性划分下的期望信息，spilt为分裂属性名，partition是目的划分的划分数目（即aim的划分数目）
{
	int spiltPartition = spilt.partition;
	double info = 0;
	if (spilt.partition != 0) {
		for (int i = 1; i <= spiltPartition; i++)
		{
			double temp = ((double)(spilt.list[0][i]) / spilt.list[0][0]);
			temp *= Entropy(partition, spilt.list[i]);
			info += temp;

		}
	}
	else {
		for (int i = 1; i <= 2; i++)
		{
			double temp = ((double)(spilt.list[0][i]) / spilt.list[0][0]);
			temp *= Entropy(partition, spilt.list[i]);
			info += temp;

		}
	}
	return info;
}

int searchMax(double array[], int n)
{
	int k = 0;
	for (int i = 0; i < n; i++)
		if (!isvisited[i]) {
			k = i; break;
		}
	for (int i = k; i < n; i++)
	{
		if (isvisited[i]) continue;
		if (array[i] > array[k]) k = i;
	}
	return k;
}

void freeList(Instance* head)
{
	Instance* p = head;
	if (p == NULL) return;
	else {
		while (p->next != NULL) {
			Instance* q = p->next;
			p->next = q->next;
			delete q;
		}
	}
}

int lengthOfList(Instance * head)
{
	int count = 0;
	while (head->next != NULL)
	{
		count++;
		head = head->next;
	}
	return count;
}

void addNewInstance(Instance * head, Instance * v)
{
	while (head->next != NULL) head = head->next;
	head->next = v;
	v->next = NULL;
}

Instance * createNewInstanceList()
{
	Instance* head = new Instance;
	head->next = NULL;
	return head;
}

Instance * createNewInstanceLists(int n)
{
	Instance* headList = new Instance[n];
	for (int i = 0; i < n; i++)
		headList[i].next = NULL;
	return headList;
}

int* insertSort(Instance* trainingList,int i,attribute aim) {//对训练集关于第i个属性进行插入排序,返回的是一个数组，0单元为总元素个数，后面n个单元为相应标签的个数
	Instance *p = trainingList;
	Instance*q = trainingList->next;
	p->pre = NULL;
	int* count = new int[aim.partition+1];
	for (int i = 0; i <=aim.partition; i++) count[i] = 0;
	while (q != NULL) {
		for (int i = 0; i < aim.partition; i++) {
			if (strcmp(aim.partitionName[i], q->categorization) == 0) {
				count[i + 1]++;
			}
		}
		q->pre = p;
		q = q->next;
		p = p->next;
		count[0]++;
	}
	p = trainingList->next;
	while (p != NULL) {
		Instance* temp = p->next;
		q = p->pre;
		while (q->pre != NULL) {
			if (q->spiltAttrNum[i] >= p->spiltAttrNum[i]) break;
			else q = q->pre;
		}//循环结束，p应该插入q的后面
		if(p->next!=NULL) p->next->pre = p->pre;
		p->pre->next = p->next;
		if(q->next!=NULL) q->next->pre = p;
		p->pre = q;
		p->next = q->next;
		q->next = p;
		p = temp;
	}
	/*Instance* pttt = trainingList;
	while (pttt->next != NULL) {
		for (int i = 0; i < 3; i++) cout << pttt->next->spiltAttrNum[i] << "-";
		cout << pttt->next->spiltAttrNum[3] << endl;
		pttt = pttt->next;
	}*/
	return count;
}
#endif 