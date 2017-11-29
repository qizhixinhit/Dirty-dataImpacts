#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <float.h>
#include <list>
#include <vector>
#include <string.h>
#include <math.h>
#include <stack>
#include <sys/time.h>
#include <fstream>
using namespace std;
typedef struct cf {
	int n;
	vector<double> LS;
	vector<double> SS;
	struct CFnode* child;
}CF;
typedef struct CFnode{
	list<CF> CFlist;
	int length;
	struct CFnode* fatherNode;
	CF* fatherCF;
}Node;
class CFTree{
public:
	CFTree(int L,double T,int attributeNum,char* file);
	list<CF> getResultCFlist();
	int getAttributeNum();
	void judgeItem(char* file);
	void getClassList(char* classs);
private:
	list<CF> leafList;
	vector<string> classVector;
	int counting;
	int L;//每个节点的最大CF数L
	double T;//最大样本半径阈值T
	ifstream input;
	int attributeNum;//defaulted as constant attribute，维度
	void createCFTree(char* file);
	void addnewCF(vector<double> tmp);
	/*
	1.从根节点向下寻找和新样本距离最近的叶子节点和叶子节点里最近的CF节点
    2.如果新样本加入后，这个CF节点对应的超球体半径仍然满足小于阈值T，
	则更新路径上所有的CF三元组，插入结束。否则转入3.
    3. 如果当前叶子节点的CF节点个数小于阈值L，则创建一个新的CF节点，放入新样本，
	将新的CF节点放入这个叶子节点，更新路径上所有的CF三元组，插入结束。否则转入4。
    4.将当前叶子节点划分为两个新叶子节点，选择旧叶子节点中所有CF元组里超球体距离最远的两个CF元组，
	分布作为两个新叶子节点的第一个CF节点。将其他元组和新样本元组按照距离远近原则放入对应的叶子节点。
	依次向上检查父节点是否也要分裂，如果需要按和叶子节点分裂方式相同。
	*/
	Node* root;
	double* expectation;
	double calculateEDistance(vector<double> v1, vector<double> v2);//计算两个向量之间的欧几里得距离
	//簇内tightness
	vector<double> calculateCoreVector(const CF cf);//计算当前CF的中心点（向量）
	double calculateR(const CF cf);//计算当前聚类特征中点到中心点的平均距离。
	double calculateD(const CF cf);//计算当前聚类特征CF中各点对之间的平均距离。
	//簇间closeness，注意在D的计算中，自身到自身的不算入平均值中，故分子式N*（N-1），所以D2两个参数都为cf时值与D不同
	double calculateD2(const CF cf1, const CF cf2);//就算两个簇间距离，采用inter-cluster距离，具体见参考资料，公式和LS,SS之间关系很容易即可推导	
	void AddorMinusVector(CF* cf,vector<double> v,bool operation);//若为真，则添加向量，若为假，则删除向量
	void spilt(Node* node,Node* node1,Node* node2);
	void update(Node* probeNode,vector<double> tmp);//从该节点往上跟新CF树。若需分裂则分裂，相应的CF值需要跟新
	void createResultCFList();
};

CFTree::CFTree(int L, double T,int attributeNum,char* file)
{
	CFTree::L = L;
	CFTree::T = T;
	CFTree::attributeNum = attributeNum;
	expectation = new double[attributeNum];
	for (int i = 0; i < attributeNum; i++) {
		expectation[i] = 0;
	}
	/*
	double tes1[] = { 1, 1, 0 };
	double tes2[] = { 0, 0, 1 };
	vector<double> t1(tes1, tes1 + 5);
	vector<double> t2(tes2, tes2 + 5);
	cout << calculateEDistance(t1, t2);
	*/
	/*CF* cf = new CF();
	cf->n = 8;
	cf->LS.resize(attributeNum);
	cf->SS.resize(attributeNum);
	for (int i = 0; i < attributeNum; i++)
	{
		cf->LS[i] = 4;
		cf->SS[i] = 4;
	}
	cout << calculateD(*cf) << endl;*/
	input.open(file);
	if (!input) {
		cout << "open failed!" << endl;
	}
	root = new Node();
	root->length = 0;
	root->fatherCF = NULL;
	root->fatherNode = NULL;
	createCFTree(file);
	createResultCFList();
}
list<CF> CFTree::getResultCFlist()
{
	return leafList;
}
int CFTree::getAttributeNum()
{
	return attributeNum;
}
void CFTree::judgeItem(char* file)
{
	ifstream input(file);
	vector<vector<double> > cores;
	for (list<CF>::iterator i = leafList.begin(); i != leafList.end(); i++) {
		vector<double> tmp = calculateCoreVector(*i);
		cores.push_back(tmp);
	}

	vector<vector<int> > result(classVector.size());
	for (int i = 0; i < classVector.size(); i++) {
		for (int j = 0; j < leafList.size(); j++) {
			result[i].push_back(0);
		}
	}
	vector<vector<double> > p(classVector.size());//precision
	for (int i = 0; i < classVector.size(); i++) {
		for (int j = 0; j < leafList.size(); j++) {
			p[i].push_back(0);
		}
	}
	vector<vector<double> > r(classVector.size());//recall
	for (int i = 0; i < classVector.size(); i++) {
		for (int j = 0; j < leafList.size(); j++) {
			r[i].push_back(0);
		}
	}
	vector<vector<double> > f(classVector.size());//recall
	for (int i = 0; i < classVector.size(); i++) {
		for (int j = 0; j < leafList.size(); j++) {
			f[i].push_back(0);
		}
	}
	char class1[20] = { 0 };
	input>>class1;
	while (!input.eof()) {
		vector<double> tmp(attributeNum);
		char tempp[20] = { 0 };
		char* endptr;
		for (int i = 0; i < attributeNum; i++) {
			input >> tempp;
			if (strcmp(tempp, "?") != 0) {
				tmp[i] = strtod(tempp,&endptr);
			}
			else tmp[i] = expectation[i];
		}
		input >> class1;
		int belongto = 0;
		double distance = DBL_MAX;
		int count = 0;
		for (vector<vector<double> >::iterator i = cores.begin(); i != cores.end(); i++,count++) {
			double tmep = calculateEDistance(tmp,*i);
			if (tmep < distance) {
				distance = tmep;
				belongto = count;
			}
		}
		for(int i=0;i<classVector.size();i++)
			if (classVector[i].compare(class1)==0) {
				result[i][belongto]++;
				break;
			}
	}
	cout<<"共有"<<leafList.size()<<"个簇"<<endl;
	/*for (int i = 0; i < leafList.size(); i++) {
		cout << "第" << i + 1 << "个CF簇的结果为:" << endl;
		for (int j = 0; j < classVector.size(); j++) {
			//if (result[j][i] < 20) continue;
			cout << classVector[j] << "类别个数为:" << result[j][i] << endl;
		}
		cout << endl;
	}*/
	vector<double> sumaik(classVector.size());
	vector<double> sumakj(leafList.size());
	for (int i = 0; i < classVector.size(); i++) {
		sumaik[i] = 0;
		for (int j = 0; j < leafList.size(); j++) sumaik[i] += result[i][j];
	}
	for (int j = 0; j < leafList.size(); j++) {
		sumakj[j] = 0;
		for (int i = 0; i < classVector.size(); i++) sumakj[j] += result[i][j];
	}
	for (int i = 0; i < classVector.size(); i++) {
		for (int j = 0; j < leafList.size(); j++) {
			//cout << sumaik[i] << " " << sumakj[j] << endl;
			r[i][j] = result[i][j] / sumaik[i];
			p[i][j] = result[i][j] / sumakj[j];
			//cout << r[i][j] << " " << p[i][j] << endl;
			if (result[i][j] == 0) f[i][j] = -1;
			else f[i][j] = 2 * r[i][j] * p[i][j] / (r[i][j] + p[i][j]);
		}
	}
	/*for (int i = 0; i < classVector.size(); i++) {
		for (int j = 0; j < leafList.size(); j++) {
			printf("%10.6f", f[i][j]);
		}
		cout << endl;
	}*/
	double sumf = 0; 
	double sump = 0;
	double sumr = 0;
	for (int i = 0; i < classVector.size(); i++) {
		int k = 0;
		for (int j = 0; j < leafList.size(); j++) {
			if (f[i][k] < f[i][j]) k = j;
		}
		if (f[i][k]!=-1)//若全是-1 ，就不要这个class的fmeasure了
		{
			sumf += f[i][k]*(sumaik[i]/counting);
			sump += p[i][k]*(sumaik[i]/counting);
			sumr += r[i][k]*(sumaik[i]/counting);
		}
	}
	cout<<"p值为:"<<sump<<endl;
	cout<<"r值为:"<<sumr<<endl;
	cout << "f-measure值为:" << sumf << endl;
	input.close();
}
void CFTree::getClassList(char* classs)
{
	ifstream input(classs);
	char tmp[20] = { 0 };
	while (!input.eof()) {
		input >> tmp;
		//cout << tmp << endl;
		classVector.push_back(tmp);
	}
	input.close();
}
void CFTree::createCFTree(char* file)
{
	int counter = 0;
	input >> counter;
	char* endptr;
	while (!input.eof()) {
		char temp[20] = { 0 };
		for (int i = 0; i < attributeNum; i++) {
			input >> temp;
			if(strcmp(temp,"?")!=0){
			//	cout << temp << endl;
				//cout << atof(temp);
				expectation[i] += strtod(temp,&endptr);
			}
		}
		//counter++;
		input >> temp;
	}
	for (int i = 0; i < attributeNum; i++) expectation[i] /= counter;
	input.close();
	input.open(file);
	input>>counting;
	while (!input.eof()) {
		vector<double> tmp(attributeNum);
		//cout <<":"<< counter << endl;
		char temp[20] = { 0 };
		for (int i = 0; i < attributeNum; i++) {
			input >> temp;
			if (strcmp(temp, "?") != 0) {
				tmp[i] =  strtod(temp,&endptr);
			}
			else tmp[i] = expectation[i];
		}
		addnewCF(tmp);
		input >> temp;
	}
}
void CFTree::addnewCF(vector<double> tmp)
{
	//第一步：从根节点开始找最近的CF点
	Node* probeNode = root;
	if (root->CFlist.empty()) {//若根节点为空，则新建一个CF点
		CF* newCF = new CF();
		newCF->child = NULL;
		newCF->n = 1;
		for (int i = 0; i <attributeNum; i++){
			newCF->LS.push_back(tmp[i]);
			newCF->SS.push_back(tmp[i] * tmp[i]);
		}
		root->length++;
		root->CFlist.push_back(*newCF);
	}
	else {//否则从根节点开始往下找到与tmp最近的叶节点中的CF
		while (true) {
			Node* closetNodeChild = NULL; 
			double distance = DBL_MAX;
			CF* probe=NULL;
			//int count = 0;
			for (list<CF>::iterator probeCF = probeNode->CFlist.begin(); probeCF != probeNode->CFlist.end(); probeCF++) {
				vector<double> core = calculateCoreVector(*probeCF); //count++;
				double tmpCoreED = calculateEDistance(tmp, core);//计算该CF的中心点和当前样本点的距离
				if (tmpCoreED < distance) {
					closetNodeChild = probeCF->child;
					distance = tmpCoreED;
					probe = &(*probeCF);
				}
			}//循环结束时我们得到的是该层中与当前样本点最近的CF
			//cout << "count=" << count << endl;
			if(closetNodeChild!=NULL) probeNode = closetNodeChild;//若还没到叶节点，则循环继续，此时的探针指向之前所找到的最近的CF所对应的儿子节点
			else {//否则的话，已经找到了叶节点，且找到了该叶节点中最近的CF。
				//1.将其加入当前CF中，并判断其是否超过了阈值T
				AddorMinusVector(probe,tmp,true);
				double CFD = calculateD(*probe);//计算当前探针所指CF加入了新样本点后是否使其直径超过了阈值。
				if (CFD > T) { //若超过，2.则需要新建CF
					AddorMinusVector(probe, tmp, false);
					CF* newCF = new CF();
					newCF->child = NULL;
					newCF->n = 1;
					for (int i = 0; i <attributeNum; i++){
						newCF->LS.push_back(tmp[i]);
						newCF->SS.push_back(tmp[i] * tmp[i]);
					}
					probeNode->CFlist.push_back(*newCF);//先加入到该叶节点的CF链中，若length因此超过了设定值，则在update中分裂
					probeNode->length++;
				}
				update(probeNode, tmp);//4.跟新每个非叶节点的CF信息，如果分裂节点在父节点中插入新的元组，检查分裂，直到root
				break;//退出循环
			}
		}
	}
}
double CFTree::calculateEDistance(vector<double> v1, vector<double> v2)
{
	double result = 0.0;
	for (int i = 0; i < attributeNum; i++) result += (v1[i] - v2[i])*(v1[i] - v2[i]);
	/*for (vector<double>::iterator j = v1.begin(); j != v1.end(); j++)
		cout << *j << " ";
	cout << endl;
	for (vector<double>::iterator j = v2.begin(); j != v2.end(); j++)
		cout << *j << " ";
	cout << endl;
	cout << result << endl;*/
	return sqrt(result);
}
vector<double> CFTree::calculateCoreVector(const CF cf)
{
	vector<double> result(cf.LS);
	for (vector<double>::iterator i = result.begin(); i != result.end(); i++) *i /= cf.n;
	return result;
}
double CFTree::calculateR(const CF cf)
{
	double EuclidianSumDis = 0.0;
	vector<double> core = calculateCoreVector(cf);
	for (int i = 0; i < attributeNum; i++) EuclidianSumDis += (cf.SS[i]+core[i]*core[i]*cf.n-2*cf.LS[i]*core[i]);
	return sqrt(EuclidianSumDis/cf.n);
}
double CFTree::calculateD(const CF cf)
{
	double result = 0.0;
	for (int i = 0; i < attributeNum; i++) result += 2 * cf.n*cf.SS[i] - 2 * cf.LS[i] * cf.LS[i];
	return sqrt(result/(cf.n*(cf.n-1)));
}
double CFTree::calculateD2(const CF cf1, const CF cf2)
{
	double result = 0.0;
	for (int i = 0; i < attributeNum; i++) result += cf1.n*cf2.SS[i] + cf2.n*cf1.SS[i] - 2 * cf1.LS[i] * cf2.LS[i];
	return sqrt(result/(cf1.n*cf2.n));
}
void CFTree::AddorMinusVector(CF * cf, vector<double> v, bool operation)
{
	if (operation == true) {
		cf->n++;
		for (int i = 0; i < attributeNum; i++){
			cf->LS[i] += v[i];
			cf->SS[i] += v[i] * v[i];
		}
	}
	else {
		cf->n--;
		for (int i = 0; i < attributeNum; i++) {
			cf->LS[i] -= v[i];
			cf->SS[i] -= v[i] * v[i];
		}
	}
}
void CFTree::spilt(Node* node,Node* node1,Node* node2)
{
	//1.找到node中距离最远的两个CF，以其为种子新建两个node，并将剩余CF分配到这两个node中
	vector<vector<double> > cfCorelist;
	for (list<CF>::iterator i = node->CFlist.begin(); i != node->CFlist.end(); i++) cfCorelist.push_back(calculateCoreVector(*i));
	/*for (vector<vector<double> >::iterator i = cfCorelist.begin(); i != cfCorelist.end(); i++) {
		for (vector<double>::iterator j = i->begin(); j != i->end(); j++)
			cout << *j << " ";
		cout << endl;
	}*/
	//至此算出了每个CF的中心点位置。
	double* EDistance = new double[node->length*node->length];
	int maxi=0, maxj=0;//两个相距最远的中心点在cfCorelist中的位置
	for (int i = 0; i < node->length; i++)
		for (int j = 0; j < node->length; j++)
		{
			EDistance[i*node->length + j] = calculateEDistance(cfCorelist[i], cfCorelist[j]);
			//cout << EDistance[i*node->length + j] << endl;
		}
	//至此算出各中心两两之间的距离,事实上，这是一个对称矩阵
	for (int i = 0; i < node->length; i++)
		for (int j = 0; j < node->length; j++)
			if (EDistance[i*node->length + j] > EDistance[maxi*node->length + maxj]) maxi = i, maxj = j;
	//至此找到了距离最大的两个中心点及其对应的CF。
	/*cout << maxi << " " << maxj << endl;
	for (int i = 0; i < node->length; i++)
	{
		for (int j = 0; j < node->length; j++)
			cout << EDistance[i*node->length + j] << "  ";
		cout << endl;
	}*/
	node1->length = 0;
	node2->length = 0;
	int count = 0;
	//cout << node->length << "?" << node->CFlist.size() << endl;	
	for (list<CF>::iterator i = node->CFlist.begin(); i != node->CFlist.end(); i++,count++) {
		if (EDistance[count*node->length + maxi] > EDistance[count*node->length + maxj]) node2->CFlist.push_back(*i), node2->length++;
		else node1->CFlist.push_back(*i), node1->length++;
	}
	delete[] EDistance;
	//delete node;
	//至此原节点已分裂为了两个节点，此时还需修改原节点的父亲节点的信息。//但不在该函数中修改，先返回两个node
}
void CFTree::update(Node * probeNode, vector<double> tmp)
{
	while (true) {
		Node* fatherNode = probeNode->fatherNode;
		CF* fatherCF = probeNode->fatherCF;
		if (probeNode->length > L) {//3.若满足分裂条件。
			Node* node1=new Node() ; Node* node2=new Node() ;//之前一直发现返回后的内容丢了，因为这里没有new，局部变量直接释放了，，，好蠢啊我，这么低级的错误
			spilt(probeNode, node1,node2);//在spilt中当获得node1和node2后，probeNode就被释放了，故要在之前就保存其父亲的信息。
			node1->fatherNode = fatherNode; node2->fatherNode = fatherNode;
			//
			CF* cf1 = new CF(); CF* cf2 = new CF();
			cf1->child = node1; cf2->child = node2;
			//node1->fatherCF = cf1; node2->fatherCF = cf2;这里有问题，因为后面的fatherNode添加cf1和cf2的时候是通过复制实现的，这里的指针仍然指着原cf1，cf2，这样这里的
			//fatherCF都不是真正的fatherCF。java写多了，指针都不会了。。。。
			cf1->n = 0; cf2->n = 0;
			//
			for (int i = 0; i < attributeNum; i++) {
				cf1->LS.push_back(0); cf1->SS.push_back(0);
				cf2->LS.push_back(0); cf2->SS.push_back(0);
			}
			for (list<CF>::iterator i = node1->CFlist.begin(); i != node1->CFlist.end(); i++) {
				cf1->n += i->n;
				for (int j = 0; j < attributeNum; j++) {
					cf1->LS[j] += i->LS[j];
					cf1->SS[j] += i->SS[j];
				}
			}
			for (list<CF>::iterator i = node2->CFlist.begin(); i != node2->CFlist.end(); i++) {
				cf2->n += i->n;
				for (int j = 0; j < attributeNum; j++) {
					cf2->LS[j] += i->LS[j];
					cf2->SS[j] += i->SS[j];
				}
			}
			//
			if (fatherNode != NULL&&fatherCF != NULL) {//若当前结点不是根节点，跟新其父节点信息
				/*cout << fatherNode->CFlist.size() << endl;
				cout << fatherCF->n << endl;
				for (int j = 0; j < attributeNum; j++) {
					cout << fatherCF->LS[j] << ",";
				}
				cout << endl;
				for (int j = 0; j < attributeNum; j++) {
					cout << fatherCF->SS[j] << ",";
				}
				cout << endl << endl;*/
				for (list<CF>::iterator i = fatherNode->CFlist.begin(); i != fatherNode->CFlist.end(); i++) { //删去原CF
					/*cout << i->n << endl;
					for (int j = 0; j < attributeNum; j++) {
						cout << i->LS[j] << ",";
					}
					cout << endl;
					for (int j = 0; j < attributeNum; j++) {
						cout << i->SS[j] << ",";
					}
					cout << endl << endl;*/
					if (&(*i) == fatherCF) {
						fatherNode->CFlist.erase(i);
						//cout << "?" << endl;
						break;
					}
				}
				//cout << fatherNode->CFlist.size() << endl;
				fatherNode->CFlist.push_back(*cf1);//添加新CF1
				fatherNode->CFlist.push_back(*cf2);//添加新CF2
				list<CF>::iterator i = fatherNode->CFlist.end();
				i--; node2->fatherCF = &(*i);
				i--; node1->fatherCF = &(*i);
				//cout << fatherNode->CFlist.size() << endl;
				fatherNode->length++;//长度+2-1
				probeNode = fatherNode;
				delete cf1, cf2;
				continue;
			}
			else {//若当前分裂了的原节点是根节点，此时高度加一，返回
				Node* newRoot = new Node();
				newRoot->fatherCF = NULL; newRoot->fatherNode = NULL;
				newRoot->length = 2;
				node1->fatherNode = newRoot;
				node2->fatherNode = newRoot;
				newRoot->CFlist.push_back(*cf1);
				newRoot->CFlist.push_back(*cf2);
				list<CF>::iterator i = newRoot->CFlist.end();
				i--; node2->fatherCF = &(*i);
				i--; node1->fatherCF = &(*i);
				root = newRoot;
				/*for (list<CF>::iterator i = newRoot->CFlist.begin(); i != newRoot->CFlist.end(); i++) {
					for (int j = 0; j < attributeNum; j++)
					{
						cout << i->LS[j] << endl;
						cout << i->SS[j] << endl;
					}
				}*/
				return;
			}
		}
		else if(fatherNode!=NULL&&fatherCF!=NULL){
			AddorMinusVector(fatherCF, tmp, true);
			probeNode = fatherNode;
		}
		else break;//此时仅剩的情况是当前probeNode为根,且probeNode不分裂
	}
}
void CFTree::createResultCFList()
{
	stack<Node*> stk;
	stk.push(root);
	while (!stk.empty()) {
		Node* tmp=NULL;
		tmp = stk.top();
		stk.pop();
		if (tmp->CFlist.empty() || tmp->CFlist.begin()->child == NULL) 
			for (list<CF>::iterator i = tmp->CFlist.begin(); i != tmp->CFlist.end(); i++) leafList.push_back(*i);
		for (list<CF>::iterator i = tmp->CFlist.begin(); i != tmp->CFlist.end(); i++) {
			if(i->child!=NULL) stk.push(i->child);
		}
	}
}
int main() {
	struct timeval tpstart,tpend; 
	float timeuse; 
	gettimeofday(&tpstart,NULL);
	CFTree cft(20,30000,3, "/home/qizhixin/dbscan/Clustering_data/activity_recognition/normal_distribution/30%missing_activity_recognition.csv");
	cft.getClassList("/home/qizhixin/dbscan/Clustering_data/activity_recognition/class.txt");
	list<CF> result = cft.getResultCFlist();
	int count = 1;
	/*for (list<CF>::iterator i = result.begin(); i != result.end(); i++,count++) {
		if (i->n < 100) continue;
		cout << "第" << count << "个CF：\n其n为" << i->n << ";"<<endl;
		cout << "LS为：";
		for (int j = 0; j < cft.getAttributeNum(); j++) {
			cout << i->LS[j] << ",";
		}
		cout << endl << "SS为：";
		for (int j = 0; j < cft.getAttributeNum(); j++) {
			cout << i->SS[j] << ",";
		}
		cout << endl << endl;
	}*/
	cft.judgeItem("/home/qizhixin/dbscan/Clustering_data/activity_recognition/normal_distribution/30%missing_activity_recognition.csv");
	gettimeofday(&tpend,NULL); 
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ 
	tpend.tv_usec-tpstart.tv_usec; 
	timeuse/=1000000; 
	printf("Used Time:%f\n",timeuse*1000); 
	exit(0);
}
