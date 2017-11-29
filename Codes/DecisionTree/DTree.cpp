#include "structureDefine.h"
ofstream output("DecisionTree//carResult.txt");
class DecisionTree {
private :
	ifstream fileData;
	ifstream fileAttributes;
	ifstream fileInput;//读取目的划分的标签名称
	Instance* testList;
	DTree* root;//得到决策树的树头
	attribute* spiltAttributes;
	attribute aim;
	double correctRate;
	double timeCost;
	int n;
	Instance* inputData(attribute* spiltAttributes, attribute* aim, Instance* head);
	attribute * inputSpiltAttributes(); //得到属性数组
	int chooseSpiltAttributes(attribute* spiltAttribute, attribute aim);
	void printMatrix(attribute* spiltAttributes, attribute aim);
	bool getAttributeMatrix(attribute* spiltAttributes, attribute* aim, Instance* head);//若当前没有属性可选了，返回true
	DTree* createDecisionTree(attribute* spiltAttributes, attribute* aim, Instance* head, double purity);//生成决策树
	int purityJudge(Instance * head, attribute aim, double purity);//纯度判断，若当前节点某标签数目占比到一定程度，就将其设为终节点停止分裂
	char* classJudge(Instance ins, DTree* root, attribute* spiltAttributes);//根据建立的决策树判断当前元组属于哪个标签
public :
	DecisionTree(string data, string attribute, string classValue);
	double getCorrectRate();
	attribute* getAttributes();
	double getTimeCost();
};

Instance * DecisionTree::inputData(attribute * spiltAttributes, attribute * aim, Instance * head)
{
	Instance* testData = createNewInstanceList();
	while (!fileData.eof())//读取数据，每读取一行数据，建立一个实例
	{
		Instance * ins = new Instance();
		int random = rand() % 10;//若小于三则将该实例划分入测试集，若大于三则划分入训练集
		char temp[N][N] = { 0 };
		for (int i = 0; i < n; i++)
		{
			if (spiltAttributes[i].partition != 0)
			{
				fileData >> temp[i];
				//cout << temp[i]<< " ";////////////////
				strcpy(ins->spiltAttr[i], temp[i]);//获得属性值
			}
			else {
				fileData >> (ins->spiltAttrNum[i]);//若为连续型属性，获取数值型属性
			}
			strcpy(ins->spiltAttrName[i], spiltAttributes[i].chr);//获得相应的属性名
		}
		fileData >> temp[n];
		strcpy(ins->categorization, temp[n]);
		//cout << ins->categorization << " ";/////////////
		//cout << endl;
		if (random >= 3) addNewInstance(head, ins);
		else addNewInstance(testData, ins);
	}
	return testData;
}

attribute * DecisionTree::inputSpiltAttributes()
{
	fileAttributes >> n;
	attribute* spiltAttributes = new attribute[n];
	for (int i = 0; i < n; i++)
	{
		fileAttributes >> spiltAttributes[i].chr;
		int partition;
		fileAttributes >> partition;
		spiltAttributes[i].partition = partition;
		for (int j = 0; j < spiltAttributes[i].partition; j++)
			fileAttributes >> spiltAttributes[i].partitionName[j];
	}
	return spiltAttributes;
}

int DecisionTree::chooseSpiltAttributes(attribute * spiltAttribute, attribute aim)
{
	double aimEntropy = Entropy(aim.partition, aim.list[0]);
	double infoExpection[N] = { 0 };
	double gain[N] = { 0 };//信息增益
	double spilt_info[N] = { 0 };//分裂信息
	double gain_ratio[N] = { 0 };//增益率
	for (int i = 0; i < n; i++)
	{
		if (isvisited[i]) continue;
		if (spiltAttribute[i].partition != 0)//离散型变量
		{
			infoExpection[i] = info(spiltAttribute[i], aim.partition);
			gain[i] = aimEntropy - infoExpection[i];
			spilt_info[i] = Entropy(aim.partition, spiltAttribute[i].list[0]);
			gain_ratio[i] = gain[i] / spilt_info[i];
		}
		else {//连续型变量

		}
	}
	int p = searchMax(gain_ratio, n);
	//printf("Spilt:%s\n", spiltAttribute[p].chr);
	return p;
}

void DecisionTree::printMatrix(attribute * spiltAttributes, attribute aim)
{
	for (int i = 0; i < n; i++)
	{
		std::cout << spiltAttributes[i].chr << "的数值矩阵为:" << endl;
		if (spiltAttributes[i].partition != 0) {
			for (int j = 0; j <= spiltAttributes[i].partition; j++)//纵轴
			{
				for (int k = 0; k <= aim.partition; k++)
				{
					std::cout << spiltAttributes[i].list[j][k] << "\t";
				}
				std::cout << endl;
			}
		}
		else {
			for (int j = 0; j <= 2; j++)//纵轴
			{
				for (int k = 0; k <= aim.partition; k++)
				{
					std::cout << spiltAttributes[i].list[j][k] << "\t";
				}
				std::cout << endl;
			}
		}
	}
}

bool DecisionTree::getAttributeMatrix(attribute * spiltAttributes, attribute * aim, Instance * head)
{
	for (int i = 0; i < n; i++)//初始化每一个分裂属性的存储表
		if (spiltAttributes[i].partition != 0)
		{
			for (int j = 0; j <= spiltAttributes[i].partition; j++)
				for (int k = 0; k <= N; k++)
					spiltAttributes[i].list[j][k] = 0;
		}
		else {
			for (int j = 0; j <= 2; j++)
				for (int k = 0; k <= N; k++)
					spiltAttributes[i].list[j][k] = 0;
		}
	for (int i = 0; i <= n; i++) aim->list[0][i] = 0;//初始化目的属性数值矩阵，该矩阵只需用到第一行
	int count = 0;//用来记录之前已访问了的属性个数
	//将训练集降序排序k次，k为未访问的连续型属性的个数，并计算出连续型属性的分裂点
	for (int i = 0; i < n; i++) {
		if (isvisited[i]||spiltAttributes[i].partition!=0) continue;
		else {
			int *wholenum = insertSort(head, i,*aim);
			double spiltPoints = head->next->spiltAttrNum[i];//先指定为第一个
			double max = -100000;
			Instance* p = head->next; int countInstance = 1;
			int* counts = new int[aim->partition];
			for (int j = 0; j < aim->partition; j++) counts[j] = 0;
			while (p != NULL) {
				for(int j=0;j<aim->partition;j++) 
					if (strcmp(aim->partitionName[j], p->categorization) == 0) {
						counts[j]++;
						break;
					}
				int list[3];
				list[0] = wholenum[0];
				list[1] = countInstance;
				list[2] = wholenum[0] - countInstance;
				double entropy = Entropy(2, list);
				//std::cout << entropy << " ";
				//计算期望信息
				double info = 0;
				int *listtemp = new int[aim->partition + 1];
				listtemp[0] = countInstance;
				for (int k = 1; k <= aim->partition; k++) listtemp[k] = counts[k - 1];
				info += (((double)countInstance) / wholenum[0])*Entropy(aim->partition, listtemp);
				listtemp[0] = wholenum[0] - countInstance;
				for (int k = 1; k <= aim->partition; k++) listtemp[k] = wholenum[k] - counts[k - 1];
				info += (((double)(wholenum[0]-countInstance)) / wholenum[0])*Entropy(aim->partition, listtemp);
				//std::cout << info << " ";
				double info_gain = entropy - info;
				//std::cout << info_gain << " ";
				//std::cout << p->spiltAttrNum[i] << endl;
				if (info_gain > max)
				{
					max = info_gain;
					spiltPoints = p->spiltAttrNum[i];
				}
				p = p->next;
				countInstance++;
			}
			spiltAttributes[i].spiltPoint = spiltPoints;
			//std::cout << spiltPoints << endl;
			delete[] counts;
		}
	}
	/*std::cout << endl;
	for (int i = 0; i < aim->partition; i++) std::cout << spiltAttributes[i].spiltPoint << "-";
	std::cout << spiltAttributes[3].spiltPoint;
	std::cout << endl;*/
	//此时每个连续型属性的值以被计算出了
	while (head->next != NULL)
	{
		count = 0;
		for (int i = 0; i < n; i++)
		{
			if (isvisited[i]) {
				count++;
				continue;//若该属性在之前已经分裂过了，就不再重复计算了
			}
			if (spiltAttributes[i].partition != 0)
			{
				for (int j = 0; j < spiltAttributes[i].partition; j++)
				{
					if (strcmp(head->next->spiltAttr[i], spiltAttributes[i].partitionName[j]) == 0)
					{
						spiltAttributes[i].list[j + 1][0]++;
						spiltAttributes[i].list[0][j + 1]++;
						for (int k = 0; k < aim->partition; k++)
							if (strcmp(head->next->categorization, aim->partitionName[k]) == 0)
							{
								spiltAttributes[i].list[j + 1][k + 1]++;
								aim->list[0][k + 1]++;
								break;
							}
						break;
					}
				}
			}
			else {
					if (head->next->spiltAttrNum[i] < spiltAttributes[i].spiltPoint) {
						spiltAttributes[i].list[1][0]++;
						spiltAttributes[i].list[0][1]++;
						for (int k = 0; k < aim->partition; k++)
							if (strcmp(head->next->categorization, aim->partitionName[k]) == 0)
							{
								spiltAttributes[i].list[1][k + 1]++;
								aim->list[0][k + 1]++;
								break;
							}
					}
					else {
						spiltAttributes[i].list[2][0]++;
						spiltAttributes[i].list[0][2]++;
						for (int k = 0; k < aim->partition; k++)
							if (strcmp(head->next->categorization, aim->partitionName[k]) == 0)
							{
								spiltAttributes[i].list[2][k + 1]++;
								aim->list[0][k + 1]++;
								break;
							}
					}
			}
			spiltAttributes[i].list[0][0]++;
			aim->list[0][0]++;
		}
		if (n == count) return true;
		head = head->next;
	}
	for (int i = 0; i <= aim->partition; i++) aim->list[0][i] /= (n - count);
	return false;
}//**

DTree * DecisionTree::createDecisionTree(attribute * spiltAttributes, attribute * aim, Instance * head, double purity)
{
	if (head->next == NULL) return NULL;
	Instance* p = head;
	DTree* node = new DTree();
	int flag = purityJudge(head, *aim, purity);
	if (flag != -1)//说明是终节点
	{
		node->brands = 0;
		strcpy(node->endName, aim->partitionName[flag]);
	}
	else {//否则，递归分裂
		bool isAllVisited = getAttributeMatrix(spiltAttributes, aim, head);
		if (isAllVisited == true) {
			node->brands = 0;
			int* classNum = new int[aim->partition];
			for (int i = 0; i < aim->partition; i++) classNum[i] = 0;
			Instance* p = head;
			while (p->next != NULL)
			{
				for (int i = 0; i < aim->partition; i++) {
					if (strcmp(aim->partitionName[i], p->next->categorization) == 0) {
						classNum[i]++;
						break;
					}
				}
				p = p->next;
			}
			int k = 0;
			for (int i = 0; i < aim->partition; i++)
			{
				if (classNum[k] < classNum[i]) k = i;
			}
			strcpy(node->endName, aim->partitionName[k]);//若在所有属性都建完都还没有到达纯度要求，就把当前结点设置终结点，标签为当前的训练表中最多的标签
			return node;
		}
		int maxGain_info = chooseSpiltAttributes(spiltAttributes, *aim);
		Instance* headList = NULL;
		if(spiltAttributes[maxGain_info].partition!=0)//若离散型属性，子集个数为划分个数
			headList = createNewInstanceLists(spiltAttributes[maxGain_info].partition);
		else headList = createNewInstanceLists(2);//连续型属性，子集个数为2
		strcpy(node->decisionName, spiltAttributes[maxGain_info].chr);
		node->decisionSubscript = maxGain_info;

		if (spiltAttributes[maxGain_info].partition != 0)
		node->brands = spiltAttributes[maxGain_info].partition;
		else node->brands = 2;

		isvisited[maxGain_info] = true;
		p = head;
		while (p->next != NULL)
		{
			Instance* ins = new Instance();
			strcpy(ins->categorization, p->next->categorization);
			for (int i = 0; i < n; i++)
			{
				strcpy(ins->spiltAttrName[i], p->next->spiltAttrName[i]);
				strcpy(ins->spiltAttr[i], p->next->spiltAttr[i]);
				ins->spiltAttrNum[i] = p->next->spiltAttrNum[i];
			}
			if (spiltAttributes[maxGain_info].partition != 0) {//离散型
				for (int i = 0; i < spiltAttributes[maxGain_info].partition; i++)
				{
					if (strcmp(ins->spiltAttr[maxGain_info], spiltAttributes[maxGain_info].partitionName[i]) == 0)
					{
						addNewInstance(&headList[i], ins);
						break;
					}
				}
			}
			else {//连续型
				if (ins->spiltAttrNum[maxGain_info] < spiltAttributes[maxGain_info].spiltPoint) {
					addNewInstance(&headList[0], ins);
				}
				else addNewInstance(&headList[1], ins);
			}
			p = p->next;
		}
		freeList(head);
		for (int i = 0; i < node->brands; i++)
			node->child[i] = createDecisionTree(spiltAttributes, aim, &headList[i], purity);
		isvisited[maxGain_info] = false;
	}
	return node;
}

int DecisionTree::purityJudge(Instance * head, attribute aim, double purity)
{
	int* department = new int[aim.partition];
	for (int i = 0; i < aim.partition; i++) department[i] = 0;
	while (head->next != NULL)
	{
		for (int i = 0; i < aim.partition; i++)
			if (strcmp(head->next->categorization, aim.partitionName[i]) == 0) { department[i]++; break; }
		head = head->next;
	}
	int sum = 0;
	for (int i = 0; i < aim.partition; i++) sum += department[i];
	for (int i = 0; i < aim.partition; i++)
		if (((double)department[i] / sum)>purity) return i;
	delete[] department;
	return -1;
}

char * DecisionTree::classJudge(Instance ins, DTree * root, attribute * spiltAttributes)
{
	DTree*  p = root;
	if (p == NULL) return NULL;
	if (root->brands == 0) return root->endName;
	do {
		int flag = 0;
		if (spiltAttributes[p->decisionSubscript].partition != 0) {
			for (int i = 0; i < spiltAttributes[p->decisionSubscript].partition; i++)
				if (strcmp(ins.spiltAttr[p->decisionSubscript], spiltAttributes[p->decisionSubscript].partitionName[i]) == 0)
				{
					flag = 1;
					p = p->child[i];
					break;
				}
		}
		else {
			flag = 1;
			if (ins.spiltAttrNum[p->decisionSubscript] < spiltAttributes[p->decisionSubscript].spiltPoint) p = p->child[0];
			else p = p->child[1];
		}
		if (p == NULL) return NULL;
		else if (flag == 0) {//若当前的值与分裂节点属性中任一值都不匹配的话，进入元素个数最多的节点处
			return NULL;//暂且先不处理
		}
	} while (p->brands != 0);
	return p->endName;
}

DecisionTree::DecisionTree(string data, string attribute, string classValue) {
	srand((int)time(0));
	fileData.open(data,ios::in);
	if (!fileData) {
		std::cout << "打开失败！" << endl;
		return;
	}
	fileAttributes.open(attribute, ios::in);
	if (!fileAttributes) {
		std::cout << "打开失败！" << endl;
		return;
	}
	fileInput.open(classValue, ios::in);
	if (!fileInput) {
		std::cout << "打开失败！" << endl;
		return;
	}
	spiltAttributes = inputSpiltAttributes();	
	//读取属性向量
	/*std::cout << "分裂属性如下所示（每一行为一种分裂属性，第一个字段为属性名，第二个为该属性划分个数，后面依次为各划分的名称）" << endl;
	for (int i = 0; i < n; i++)
	{
		std::cout << spiltAttributes[i].chr << " " << spiltAttributes[i].partition << " ";
		for (int j = 0; j < spiltAttributes[i].partition; j++)
		{
			std::cout << spiltAttributes[i].partitionName[j] << " ";
		}
		std::cout << endl;
	}*/
	//
	//读取标签向量
	char c[N];
	fileInput >> aim.partition;
	for (int i = 0; !fileInput.eof(); i++)
	{
		fileInput >> c;
		strcpy(aim.partitionName[i], c);
	}
	//
	LARGE_INTEGER litmp;
	long long QPstart, QPend;
	double dfMinus, dfFreq;
	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;//获得CPU工作频率
	QueryPerformanceCounter(&litmp);//计数
	QPstart = litmp.QuadPart;
	//建立决策树
	Instance* head = createNewInstanceList();
	Instance* testList = inputData(spiltAttributes, &aim, head);
	DTree* root = createDecisionTree(spiltAttributes, &aim, head, 0.9);
	//至此建立了决策树
	//
	//计算当前决策树在测试集下的正确率
	int denaminator = 0;//记录总测试元素个数
	int molecule = 0;//记录正确测试元素个数
	correctRate = 0;//在当前测试集和训练集下的正确率
	Instance* p = testList;
	while (p->next != NULL)
	{
		char* judge = classJudge(*(p->next), root, spiltAttributes);
		if (judge != NULL&&strcmp(judge, p->next->categorization) == 0) molecule++;
		denaminator++;
		p = p->next;
	}
	correctRate = (double)molecule / denaminator;//得到正确率
	output << "读取到的正确信息的条数：" << molecule << endl;
	output << "读取到的总信息的条数：" << denaminator << endl;
	QueryPerformanceCounter(&litmp);//计数
	QPend = litmp.QuadPart;
	dfMinus = (double)(QPend - QPstart);
	timeCost = dfMinus / dfFreq;//工作次数/工作频率
	fileAttributes.close();
	fileData.close();
	fileInput.close();
}

double DecisionTree::getCorrectRate(void)
{
	return correctRate;
}

attribute * DecisionTree::getAttributes(void)
{
	return spiltAttributes;
}

double DecisionTree::getTimeCost(void)
{
	return timeCost;
}

int main()
{
	srand((int)time(0));
	int count = 10;
	double times[10] = { 0 };
	for (int i = 0; i < count; i++) {
		output << "第" << i + 1 << "次：" << endl;
		DecisionTree tree("DecisionTree//car//car_data.txt", "DecisionTree//car//car_attributes.txt", "DecisionTree//car//class.txt");
		double correctRate = tree.getCorrectRate();
		output << "当前决策树的正确率为(测试集与训练集元素个数比例为3:7):" << correctRate << endl;
		output << "召回率为1（对验证集中所有元素都进行了测试）" << endl;
		output << "故得F1-measure值为:" << (2 * correctRate / (correctRate + 1)) << endl;
		times[i] = tree.getTimeCost();
		output <<"第"<<i+1<< "次构建决策树与测试集测试的总时间花销为:" << times[i] << "秒" << endl<<endl;
	}
	double average = 0;
	for (int i = 0; i < 10; i++) {
		average += times[i];
	}
	average /= 10;
	output << "十次运算的平均时间为:" << average << "秒" << endl;
	output.close();
	return 0;
}