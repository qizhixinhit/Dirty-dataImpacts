#include<algorithm>
#include<cmath>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<ctime>
#include<map>
#include <stdlib.h>
#include<set>
#include <windows.h> 

using namespace std;

#define AttributeKinds 20//属性个数上限
#define max_container 40//分类器最大个数
#define upper_limit 5000//数据上限

struct Node {
	string attribute;
	string class_label;
	string subsidary_calss[AttributeKinds];
	Node *child[AttributeKinds];
	bool is_c_data;
	float c_data;
};

struct continuous_data {
	float Data;
	int subscript;
};
vector<continuous_data> continuous_container;
#define e 2.71828
#define infinite 10000
#define Train_Time 100

#define FileName_all "iris.txt"
//#define FileName "abalone7.txt"//待读文本
//#define AttributeFile "abalone_attributes.txt"//属性文本
//#define TestFile "abalone3.txt"//待读文本
//#define FileName "car7.txt"//待读文本
//#define AttributeFile "car_attributes.txt"//属性文本
//#define TestFile "car3.txt"//待读文本
//#define FileName "ecoli7.txt"//待读文本
//#define TestFile "ecoli3.txt"//待读文本
//#define AttributeFile "ecoli_attributes.txt"//属性文本
#define FileName "iris7.txt"//待读文本
//#define FileName "test.txt"//待读文本
#define TestFile "iris3.txt"//待读文本
#define AttributeFile "iris_attributes.txt"//属性文本
#define Attr_upper_limit 30//属性上限
#define subsidary_limit 10//属性分类上限
#define Class_upper_limit 30

vector<vector<string>> sample;//定义二维向量保存数据
vector<vector<string>> attributes;
vector<vector<string>> test_data;
vector<vector<string>> all_data;

map<string, map<string, int>> attribute_to_subscript;
map<string, int> factor_map;
map<string, int>::iterator factor_pointer;
map<string, int>::iterator class_pointer;
map < string, map<string, int>>::iterator attr_pointer;
map<string, int> class_map;

int N = 0;//数据个数
int child_amount;
int attribute_factor[Attr_upper_limit];
int best_classify[Attr_upper_limit][upper_limit];
int Max_Height = 3;
int sample_in_common[Attr_upper_limit][upper_limit];
int error_subscript[upper_limit];
string majority_class;
int attr_count = 0;//属性个数
int class_count = 0;//类个数
char attr_type[subsidary_limit];
float divide_point;

string ClassName[Class_upper_limit];//车类型

ifstream fin_all(FileName_all, ios_base::in);
ifstream fin(FileName, ios_base::in);
ifstream fin_test(TestFile, ios_base::in);
ifstream fin_attr(AttributeFile, ios_base::in);

struct c_data {
	float Data;
	int subscript;
};

void initialWeight(float *weight_array, int array_length);
//初始化权值

void scanFile();
//扫描目标文件

bool stopCond(int * train_sample, int * attribute_array, int height);
//判断判定树终止

void treeGrowth(int *E, int* F, Node *Root, int height);
//生成决策树

string classify(int *sample_subscript);
//根据训练集分类

int findBestSplit(int *test_array, int *attributes_array, int &class_in_details);
//寻求最佳分类属性，返回属性向量下标

float calculateGain(int* test_array);
//计算增益率

void reset();
//重置数组用于findBestSplit函数中

int judge(Node *classifier, int test_sam,int opt);
//判断样本所属类

void mapAttributeFactor();
//映射属性具体分类到其下标

float calculateErrorRate(Node * classifier,int *&error_sub,int opt);
//计算错误率 opt == 0时计算样本，1测试

float Jtree_process(int *test_data,int *&error_sub, Node* container);
//决策树封装

float calculateSeqAttr(int attr_sub,int *test_array);
//计算连续型数据增益

bool sortC_data(const c_data &v1, const c_data &v2);
//对连续型数据进行排序（升序），vector

void sort_attr_factor(int attr_sub,int* test_array,vector<c_data>&sort_data);

void scanFile() {
	char chr;
	string temp;
	if (!fin_attr) {
		cerr << "属性文件不存在，请检查文件路径是否设置正确！" << endl;
		return;
	}
	int count = 0;
	fin_attr >> class_count;
	for (int i = 0; i < class_count; i++) {
		fin_attr >> ClassName[i];
		class_map.insert(map<string, int> ::value_type(ClassName[i], i));
	}
	int temp_attr_number;
	fin_attr >> temp_attr_number;
	for (int i = 0; i < temp_attr_number; i++)
		fin_attr >>attr_type[i];
	fin_attr.read(&chr, 1);
	while (!fin_attr.eof()) {
		vector<string> node;
		do {
			temp = "";
			fin_attr.read(&chr, 1);
			while (chr != 32 && chr != 10 && !fin_attr.eof()) {
				temp += chr;
				fin_attr.read(&chr, 1);
			}
			node.push_back(temp);
			count++;
		} while (chr != 10 && !fin_attr.eof());
		attributes.push_back(node);
		attribute_factor[attr_count++] = count - 1;
		count = 0;
	}
	fin_attr.close();
	if (!fin) {
		cout << "训练文件不存在，请检查文件路径是否设置正确！" << endl;
		return;
	}
	while (!fin.eof()) {
		vector<string> node;
		for (int j = 0; j < attr_count + 1; j++) {
			fin >> temp;
			node.push_back(temp);
		}
		N++;
		sample.push_back(node);
	}
	fin.close();
	if (!fin_test) {
		cout << "测试文件不存在，请检查文件路径是否设置正确！" << endl;
		return;
	}
	while (!fin_test.eof()) {
		vector<string> node;
		for (int j = 0; j < attr_count + 1; j++) {
			fin_test >> temp;
			node.push_back(temp);
		}
		test_data.push_back(node);
	}
	fin_test.close();
	if (!fin_all)
		cerr << "数据集打开失败！" << endl;
	else {
		while (!fin_all.eof()) {
			vector<string> node;
			for (int j = 0; j < attr_count + 1; j++) {
				fin_all >> temp;
				node.push_back(temp);
			}
			all_data.push_back(node);
		}
		fin_all.close();
	}
	fin_all.close();
}

bool stopCond(int *train_sample, int *attribute_array, int height) {
	int flag = 0;
	bool stop = true;
	if (train_sample[0] == 0)
		flag = 1;
	if (height <= Max_Height && attribute_array[0] != 0 && !flag) {//---------------------change
		string lab;
		lab = sample[train_sample[1]][attr_count];
		for (int i = 2; i <= train_sample[0]; i++) {
			//test_node.subscript = train_sample[i];
			if (lab != sample[train_sample[i]][attr_count]) {
				stop = false;
				break;
			}
		}
		if (stop == false) {
			int temp = 0;
			for (int i = 0; i<attr_count; i++) {
				if (attribute_array[i + 1] == 0)
					continue;
				lab = sample[train_sample[1]][i];
				for (int k = 2; k < train_sample[0]; k++) {
					if (lab != sample[train_sample[k]][i]) {
						stop = false;
						break;
					}
				}
			}
		}
	}
	return stop;

}

void treeGrowth(int *test_array, int *attributes_array, Node *root, int height) {
	height++;
	if (stopCond(test_array, attributes_array, height)) {
		if (test_array[0] == 0)
			root->class_label = majority_class;
		else
			root->class_label = classify(test_array);
	}
	else {
		int class_in_details;
		//xianghufuzhi
		int attribute_subscript = findBestSplit(test_array, attributes_array, class_in_details);
		if (attribute_subscript < 0)
			return;
		root->attribute = attributes[attribute_subscript][0];
		if (attr_type[attribute_subscript] == 'c')
			root->is_c_data = true;
		else
			root->is_c_data = false;
		int new_test_array[subsidary_limit][upper_limit];
		for (int j = 0; j < attr_count; j++) {
			for (int k = 0; k <= sample_in_common[j][0]; k++) {
				new_test_array[j][k] = sample_in_common[j][k];
			}
		}
		root->c_data = divide_point;
		for (int i = 0; i < attribute_factor[attribute_subscript]; i++) {
			Node* subsidiary = new Node;
			root->child[i] = subsidiary;
			if (attr_type[class_in_details] == 'd')
				root->subsidary_calss[i] = attributes[class_in_details][i + 1];
			if (new_test_array[i][0] == 0) {
				majority_class = classify(test_array);
			}
			if (new_test_array[0] != 0)
				treeGrowth(&new_test_array[i][0], attributes_array, subsidiary, height);
		}
	}
	return;
}

string classify(int * sample_subscript)
{
	int vote[Class_upper_limit];
	for (int i = 0; i < class_count; i++)
		vote[i] = 0;
	//data_tuple test_sample;
	for (int i = 1; i <= sample_subscript[0]; i++) {
		//test_sample.subscript = sample_subscript[i];
		for (int j = 0; j < class_count; j++) {
			if (sample[sample_subscript[i]][attr_count] == ClassName[j]) {
				vote[j]++;
				break;
			}
		}
	}
	int majority = 0, max = vote[0];
	for (int i = 1; i < class_count; i++) {
		if (max < vote[i]) {
			max = vote[i];
			majority = i;
		}
	}
	return ClassName[majority];
}

int findBestSplit(int * test_array, int * attributes_array, int &class_in_details) {
	//vector<vector <int>>().swap(sample_in_common); 
	//data_tuple test_sample;
	int attribute_subscript = -1, best_attr_sub = -1;
	float gain_ratio = 0, temp,best_split_point;
	int class_capacity;
	string attr_name, factor_name;
	child_amount = 0;
	//int class_subscript;
	for (int i = 0; i < attr_count; i++) {
		if (attributes_array[i + 1] == 0)
			continue;//属性在之前用来分类时不必考虑
		reset();//初始化数组（存放按该属性分类的子类）
		if (attr_type[i] == 'd') {
			for (int j = 1; j <= test_array[0]; j++) {
				//test_sample.subscript = test_array[j];
				factor_name = sample[test_array[j]][i];
				attr_name = attributes[i][0];
				attr_pointer = attribute_to_subscript.find(attr_name);
				factor_map = attr_pointer->second;
				factor_pointer = factor_map.find(factor_name);
				attribute_subscript = factor_pointer->second;
				class_capacity = ++sample_in_common[attribute_subscript][0];
				sample_in_common[attribute_subscript][class_capacity] = test_array[j];
			}
			temp = calculateGain(test_array);
		}
		else {
			temp = calculateSeqAttr(i,test_array);
		}
		if (temp > gain_ratio) {
			best_split_point = divide_point;
			gain_ratio = temp;
			best_attr_sub = i + 1;//属性下标注意0号位被占
			for (int m = 0; m < class_count; m++)
				for (int n = 0; n <= sample_in_common[m][0]; n++)
					best_classify[m][n] = sample_in_common[m][n];
		}
	}
	if (attr_type[best_attr_sub - 1] == 'c'&&gain_ratio == 0)
		return -1;
	for (int m = 0; m < attribute_factor[best_attr_sub - 1]; m++)
		for (int n = 0; n <= best_classify[m][0]; n++)
			sample_in_common[m][n] = best_classify[m][n];
	attributes_array[0]--;
	attributes_array[best_attr_sub] = 0;
	class_in_details = best_attr_sub - 1;
	divide_point = best_split_point;
	return best_attr_sub - 1;
}

float calculateGain(int* test_array)
{
	//data_tuple test_sample;
	float gini[Class_upper_limit], Gini = 0, Split = 0;
	for (int x = 0; x < class_count; x++) {
		gini[x] = 0;
		//split_info[x] = 0;
	}
	int i, j, k, t, portion_size, portion_amount = 0, total = 0;
	//for (int i = 0; i < class_count; i++)
	total += test_array[0];
	//int portion_amount = sample_in_common.size();
	float parent_Gini = 0, pk;
	int same_class[Class_upper_limit];//用于存放同分支的支内数目
	for (int s = 0; s < class_count; s++)
		same_class[s] = 0;//初始化数组
	for (int j = 1; j <= total; j++) {
		class_pointer = class_map.find(sample[test_array[j]][attr_count]);
		same_class[class_pointer->second]++;
	}//分类
	for (t = 0; t < class_count; t++) {
		float temp = (float)same_class[t] / total;
		parent_Gini += pow(temp, 2);
	}
	parent_Gini = 1 - parent_Gini;
	for (i = 0; i < class_count; i++) {
		//portion_size = sample_in_common[i].size();
		portion_size = sample_in_common[i][0];
		if (portion_size == 0)
			continue;
		portion_amount++;
		for (int s = 0; s < class_count; s++)
			same_class[s] = 0;//初始化数组
		for (j = 1; j <= portion_size; j++) {
			class_pointer = class_map.find(sample[sample_in_common[i][j]][attr_count]);
			same_class[class_pointer->second]++;
		}
		for (t = 0; t < class_count; t++) {
			float temp = (float)same_class[t] / portion_size;
			gini[i] += pow(temp, 2);
		}
		if (portion_size == 0)
			return -1;
		pk = (float)portion_size / total;
		Split -= pk * (log(pk) / log(2));
		gini[i] = 1 - gini[i];
		Gini += gini[i] * pk;
		//Split += split_info[k] * sample_in_common[i][0];
	}
	Gini = parent_Gini - Gini;
	if (Split == 0)
		return -1;
	float gain_ratio = Gini / Split;
	//gain_ratio越大越准确
	return gain_ratio;
}

void reset() {
	for (int z = 0; z < class_count; z++)
		sample_in_common[z][0] = 0;
}

int judge(Node * classifier,int test_sam, int opt)
{
	int i, j, k;
	Node *temp = classifier;
	Node* test1;
	while (temp->class_label.empty()) {
		for (j = 0; j < attr_count; j++) {
			if (attributes[j][0] == temp->attribute)
				break;
		}
		if (attr_type[j] == 'c') {
			if (opt == 0) 
				k = (stof(sample[test_sam][j]) < temp->c_data) ? 0 : 1;
			else if(opt == 1)
				k = (stof(test_data[test_sam][j]) < temp->c_data) ? 0 : 1;
			else
				k = (stof(all_data[test_sam][j]) < temp->c_data) ? 0 : 1;
		}
		else {
			string temp_class;
			for (k = 0; k < attribute_factor[j]; k++) {
				if (opt == 0)
					temp_class = sample[test_sam][j];
				else if(opt == 1)
					temp_class = test_data[test_sam][j];
				else
					temp_class = all_data[test_sam][j];
				if (temp_class == temp->subsidary_calss[k])
					break;
			}
		}
		test1 = temp;
		if (temp->child[k] == NULL)
			cout << endl;
		temp = temp->child[k];
	}
	class_pointer = class_map.find(temp->class_label);
	i = class_pointer->second;
	return i;
}

void mapAttributeFactor()
{
	string attribute_name, class_name;
	int subscript;
	for (int i = 0; i < attr_count; i++) {
		map<string, int> attr_factor;
		for (int j = 1; j <= attribute_factor[i]; j++) {
			attribute_name = attributes[i][j];
			subscript = j - 1;
			attr_factor.insert(map<string, int> ::value_type(attribute_name, subscript));
		}
		class_name = attributes[i][0];
		attribute_to_subscript.insert(map<string, map<string, int>> ::value_type(class_name, attr_factor));
	}
	return;
}

void initialWeight(float * weight_array, int array_length) {
	if (array_length == 0)
		return;
	for (int i = 0; i <= array_length; i++)
		weight_array[i] = 1 / (float)array_length;
}

float calculateErrorRate(Node * classifier,int *&error_sub, int opt)
{
	Node *temp = classifier;
	//data_tuple test_sample;
	int i, j, k, count_error = 0;
	float error_rate = 0;
	string temp_class_name;
	if (opt == 0) {
		for (i = 0; i < sample.size(); i++) {
			temp = classifier;
			while (temp->class_label.empty()) {
				for (j = 0; j < attr_count; j++) {
					if (attributes[j][0] == temp->attribute)
						break;
				}
				if (attr_type[j] == 'c') {
					k = (stof(sample[i][j]) < temp->c_data) ? 0 : 1;
				}
				else {
					for (k = 0; k < attribute_factor[j]; k++) {
						if (sample[i][j] == temp->subsidary_calss[k])
							break;
					}
				}
				temp = temp->child[k];
			}
			if (temp->class_label != sample[i][attr_count]) {
				count_error++;
				error_subscript[count_error] = i;
				error_subscript[0]++;
			}
		}
		error_rate = (float)count_error / sample.size();
		error_sub = error_subscript;
	}
	else if(opt == 1) {
		for (i = 0; i < test_data.size(); i++) {
			temp = classifier;
			while (temp->class_label.empty()) {
				for (j = 0; j < attr_count; j++) {
					if (attributes[j][0] == temp->attribute)
						break;
				}
				if (attr_type[j] == 'c') {
					k = (stof(test_data[i][j]) < temp->c_data) ? 0 : 1;
				}
				else {
					for (k = 0; k < attribute_factor[j]; k++) {
						if (test_data[i][j] == temp->subsidary_calss[k])
							break;
					}
				}
				temp = temp->child[k];
			}
			if (temp->class_label != test_data[i][attr_count]) {
				count_error++;
				error_subscript[count_error] = i;
				error_subscript[0]++;
			}
		}
		error_rate = (float)count_error / test_data.size();
	}
	else {
		for (i = 0; i < all_data.size(); i++) {
			temp = classifier;
			while (temp->class_label.empty()) {
				for (j = 0; j < attr_count; j++) {
					if (attributes[j][0] == temp->attribute)
						break;
				}
				if (attr_type[j] == 'c') {
					k = (stof(all_data[i][j]) < temp->c_data) ? 0 : 1;
				}
				else {
					for (k = 0; k < attribute_factor[j]; k++) {
						if (all_data[i][j] == temp->subsidary_calss[k])
							break;
					}
				}
				temp = temp->child[k];
			}
			if (temp->class_label != all_data[i][attr_count]) {
				count_error++;
				error_subscript[count_error] = i;
				error_subscript[0]++;
			}
		}
		error_rate = (float)count_error / all_data.size();
	}
	return error_rate;
}

float Jtree_process(int *test_data, int *&error_sub, Node* container) {
	int attribute_array[Attr_upper_limit];
	for (int l = 1; l <= attr_count; l++)
		attribute_array[l] = 1;
	attribute_array[0] = attr_count;
	for (int i = 0; i < attr_count; i++) {
		if (attr_type[i] == 'd')
			continue;
		else
			attribute_factor[i] = 2;
	}
	treeGrowth(test_data, attribute_array, container, 0);
	int test_array[upper_limit];
	for (int i = 0; i < N; i++)
		test_array[i + 1] = i;
	test_array[0] = N;
	error_subscript[0] = 0;
	//classify(test_array);
	return calculateErrorRate(container,error_sub, 0);
}

float calculateSeqAttr(int attr_sub,int *test_array) {
	vector<c_data> sort_data;
	sort_attr_factor(attr_sub, test_array, sort_data);
	int class_num, data_sub, class_amount[Class_upper_limit],subsidary_total1, subsidary_total2,divide_line;
	float gini,Gini_1,Gini_2,Gini,percent1,percent2,Gini_parent,split_info;
	float best_split_point, final_Gini = infinite,final_percent1,final_percent2;
	for (int j = 0; j < test_array[0]; j++) {
		data_sub = sort_data[j].subscript;
		class_pointer = class_map.find(sample[data_sub][attr_count]);
		class_num = class_pointer->second;
		sample_in_common[class_num][0]++;
	}//将节点数据分类
	gini = 0;
	for (int k = 0; k < class_count; k++) {
		if (test_array[0] == 0)
			break;
		float temp_p;
		temp_p = (float)sample_in_common[k][0] / test_array[0];
		gini += pow(temp_p, 2);
	}
	Gini_parent = 1 - gini;
	//divide_point = sort_data[0].Data - 0.01;
	Gini = Gini_parent;
	for (int i = 1; i < test_array[0]; i++) {
		while (i<test_array[0]&&sort_data[i].Data == sort_data[i - 1].Data)
			i++;
		if (i >= test_array[0])
			break;
		divide_point = (sort_data[i].Data + sort_data[i-1].Data) / 2;
		gini = Gini_1 = Gini_2 = Gini = 0;
		subsidary_total1 = i;
		subsidary_total2 = test_array[0] - i;
		reset();
		for (int i = 0; i < class_count; i++)
			class_amount[i] = 0;//重置数据
		for (int j = 0; j < i; j++) {
			data_sub = sort_data[j].subscript;
			class_pointer = class_map.find(sample[data_sub][attr_count]);
			class_num = class_pointer->second;
			sample_in_common[class_num][++class_amount[class_num]] = data_sub;
			sample_in_common[class_num][0]++;
		}//将节点数据分类
		for (int k = 0; k < class_count; k++) {
			if (subsidary_total1 == 0) {
				gini = 1;
				break;
			}
			float temp_p;
			temp_p = (float)sample_in_common[k][0] / subsidary_total1;
			gini += pow(temp_p, 2);
		}
		Gini_1 = 1 - gini;//获取分支一的gini
		reset();
		for (int i = 0; i < class_count; i++)
			class_amount[i] = 0;
		for (int j = i; j < test_array[0]; j++) {
			data_sub = sort_data[j].subscript;
			class_pointer = class_map.find(sample[data_sub][attr_count]);
			class_num = class_pointer->second;
			sample_in_common[class_num][++class_amount[class_num]] = data_sub;
			sample_in_common[class_num][0]++;
		}//将节点数据分类
		gini = 0;
		for (int k = 0; k < class_count; k++) {
			if (subsidary_total2 == 0)
				continue;
			float temp_p;
			temp_p = (float)sample_in_common[k][0] / subsidary_total2;
			gini += pow(temp_p, 2);
		}
		Gini_2 = 1 - gini;
		percent1 = (float)subsidary_total1 / test_array[0];
		percent2 = 1 - percent1;
		Gini = Gini_1*percent1 + Gini_2*percent2;
		if (Gini < final_Gini) {
			final_Gini = Gini;
			best_split_point = divide_point;
			divide_line = i;
			final_percent1 = percent1;
			final_percent2 = percent2;
		}
	}
	if (Gini == Gini_parent)
		return -1;
	Gini = Gini_parent - Gini;
	float modify_Gain = Gini - (log((float)(test_array[0] - 1)/ sample.size()) / log(2));
	split_info = final_percent1 * (log(final_percent1) / log(2)) + final_percent2 * (log(final_percent2) / log(2));
	split_info = -split_info;
	modify_Gain = modify_Gain / split_info;
	int count1 = 0, count2 = 0;
	for (int k = 0; k < class_count; k++)
		sample_in_common[k][0] = 0;
	for (int i = 0; i < divide_line; i++)
		sample_in_common[0][++count1] = sort_data[i].subscript;
	sample_in_common[0][0] = count1;
	for (int i = divide_line; i < test_array[0]; i++)
		sample_in_common[1][++count2] = sort_data[i].subscript;
	sample_in_common[1][0] = count2;
	divide_point = best_split_point;
	return modify_Gain;
}

void sort_attr_factor(int attr_sub,int* test_array,vector<c_data>&sort_data) {
	int subscript;
	c_data temp;
	for (int i = 1; i <= test_array[0]; i++) {
		subscript = test_array[i];
		temp.Data = stof(sample[test_array[i]][attr_sub]);
		temp.subscript = test_array[i];
		sort_data.push_back(temp);
	}
	std::sort(sort_data.begin(), sort_data.end(), sortC_data);
}

bool sortC_data(const c_data &v1, const c_data &v2)
{
	return v1.Data < v2.Data;//升序排列  
}