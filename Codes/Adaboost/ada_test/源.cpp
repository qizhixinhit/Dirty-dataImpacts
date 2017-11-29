#include"ada.h"

using namespace std;

int main() {
	SYSTEMTIME  time1, time2;
	srand((int)time(NULL));
	scanFile();
	mapAttributeFactor();
	Node* judgeTree = new Node;
	float W[upper_limit];
	adaBoost_function aba(N);
	Node* containers[max_container];
	GetLocalTime(&time1);
	aba.adaBoost_process(containers);
	float accuracy1 = aba.jointClassifier(containers, 1);
	GetLocalTime(&time2);
	int time = time2.wMilliseconds - time1.wMilliseconds + (time2.wSecond - time1.wSecond) * 1000;
	cout <<"时间"<< time << "ms" << endl;
	int num1 = accuracy1*test_data.size();
	cout <<"测试数据正确率："<< accuracy1 << endl;
	cout << num1 << endl;
	float accuracy2 = aba.jointClassifier(containers, 2);
	int num2 = accuracy2*all_data.size();
	float recall_rate = (float)num1 / num2;
	cout << "召回率：" << recall_rate << endl;
	float f_measure = (2 * recall_rate*accuracy1) / (recall_rate + accuracy1);
	cout << "f_measure:"<<f_measure << endl;
	//aba.adaBoost_process(containers,Jtree_process);
	initialWeight(W, N);
	int attribute_array[Attr_upper_limit];
	for (int l = 1; l <= attr_count; l++)
		attribute_array[l] = 1;
	attribute_array[0] = attr_count;
	int test_array[upper_limit];
	for (int i = 0; i < N; i++)
		test_array[i + 1] = i;
	test_array[0] = N;
	treeGrowth(test_array, attribute_array, judgeTree, 0);
	int *s = NULL;
	cout <<"使用决策树正确率："<< 1-calculateErrorRate(judgeTree,s,1) << endl;
	system("pause");
}