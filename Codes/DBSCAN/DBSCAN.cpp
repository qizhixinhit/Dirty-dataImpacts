#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <Windows.h>

#define pointSet std::vector<point>
#define pointerSet std::vector<int>

//下面两个需要一起改
#define FILENAME "input.txt"
#define ATTRIBUTIONS 4

//#define AUTO

//如果定义了AUTO，运行后需要输入l和r两个参数，程序会运行11次，E的值为l和r之间
//否则程序只运行一次，聚类结果输出在output.txt，运行时间标准输出，E的值为下面的#define EP的值

#define WRITE_TO_FILE
#define LAMBDA 2
#define INFINITE 1000000
#define MAX_CLUSTER 100 //数据较大且参数差的太远的时候，需要把这个变大，否则会溢出
#define NOT_VISITED MAX_CLUSTER-1

#define minPts 4
#define EP 0.4

class point {
public:
	double a[ATTRIBUTIONS];
	int ddr;//amount of directly density-reachable points
	int flag;
	int flag_ans;
	pointerSet *ddrs;
	point(double *attributions, int flag_ans) {
		for (int i = 0; i < ATTRIBUTIONS; ++i) {
			a[i] = attributions[i];
		}
		ddr = 0;
		ddrs = NULL;
		flag = NOT_VISITED;
		this->flag_ans = flag_ans;
	}
	double distance(const point &p0) {
		double sum = 0;
		for (int i = 0; i < ATTRIBUTIONS; ++i) {
			sum += (a[i] - p0.a[i])*(a[i] - p0.a[i]);
			//sum += pow(fabs(a[i] - p0.a[i]), LAMBDA);
		}
		return sqrt(sum);
		//return pow(sum, 1.0/LAMBDA);
	}
	bool operator < (const point &p0) {
		for (int i = 0; i < ATTRIBUTIONS; ++i) {
			if (a[i] < p0.a[i]) return true;
			else if (a[i] > p0.a[i]) return false;
		}
		return false;
	}
};

class DBSCAN {
public:
	DBSCAN(std::string file) {
		//Prepare for normalization
		double s_max[ATTRIBUTIONS];
		double s_min[ATTRIBUTIONS];
		double avg[ATTRIBUTIONS] = { 0 };
		for (int i = 0; i < ATTRIBUTIONS; ++i) {
			s_max[i] = -INFINITE;
			s_min[i] = INFINITE;
		}

		//scan from input file
		std::ifstream fin(file);
		fin >> N;
		for (int i = 0; i < N; ++i) {
			double attributions[ATTRIBUTIONS];
			int flag;
			for (int i = 0; i < ATTRIBUTIONS; ++i) {
				fin >> attributions[i];
				if (s_max[i] < attributions[i]) s_max[i] = attributions[i];
				if (s_min[i] > attributions[i]) s_min[i] = attributions[i];
				avg[i] += attributions[i];
			}
			fin >> flag;
			a_num[flag]++;
			points.push_back(point(attributions, flag));
		}
		fin.close();

		//Normalization
		for (int i = 0; i < ATTRIBUTIONS; ++i) avg[i] /= N;
		for (pointSet::iterator p = points.begin(); p != points.end(); p++) {
			for (int i = 0; i < ATTRIBUTIONS; ++i)
				p->a[i] = (p->a[i] - avg[i]) / (s_max[i] - s_min[i]);
		}

	}
	//To calcute ddr(amount directly density-reachable points) of every point and save ddrs(directly density-reachable points) for each CORE points
	void calcDdr(void) {
		for (int i = 0; i < N; i++) {
			pointerSet *p = new pointerSet;
			points[i].ddr = 0;
			for (int j = 0; j < N; j++) {
				if (i == j) continue;
				if (points[i].distance(points[j]) < E) {
					points[i].ddr++;
					p->push_back(j);
				}
			}
			if (points[i].ddr >= minPts) {
				delete points[i].ddrs;
				points[i].ddrs = p;
			}
			else {
				delete p;//if point i is not a core point, we need not to know its directly density-reachable points
			}
		}
	}
	//原来的DFS会栈溢出，改用BFS
	int BFS(int x, int flag) {
		std::priority_queue<int> q;
		int k = 1;
		q.push(x);
		cluster[flag][points[x].flag_ans]++;
		points[x].flag = flag;
		while (!q.empty()) {
			point &p = points[q.top()];
			q.pop();
			for (int i = 0; i < p.ddrs->size(); ++i) {
				if (points[p.ddrs->at(i)].flag == NOT_VISITED) {
					points[p.ddrs->at(i)].flag = flag;
					k++;
					cluster[flag][points[p.ddrs->at(i)].flag_ans]++;
					if (points[p.ddrs->at(i)].ddr >= minPts) q.push(p.ddrs->at(i));
				}
			}
		}
		return k;
	}
	double process(double E) {
		this->E = E;
		calcDdr();
		int flag = 0;
		int cluster_num[MAX_CLUSTER];

		memset(cluster, 0, sizeof(cluster));
		for (int i = 0; i < N; ++i) {
			points[i].flag = NOT_VISITED;
		}
		for (int i = 0; i < N; ++i) {
			if (points[i].flag == NOT_VISITED && points[i].ddr >= minPts) {
				cluster_num[flag] = BFS(i, flag);
				flag++;
			}
		}

		double accuracy = 0;
		for (int i = 0; i < flag; ++i) {
			double fi = 0;
			for (int j = 0; j < ATTRIBUTIONS; ++j) {
				double p = (double)cluster[i][j] / (double)cluster_num[i];
				double r = (double)cluster[i][j] / (double)a_num[j];
				double f = 2 * p*r / (p + r);
				if (fi < f) fi = f;
			}
			accuracy += ((double)cluster_num[i] / (double)N)*fi;
		}

#ifndef AUTO

#ifdef WRITE_TO_FILE
		std::ofstream fout("output.txt");
		for (int i = 0; i < N; ++i) {
			fout << points[i].flag << '\t' << points[i].flag_ans << std::endl;
		}
		fout << "The accuracy is " << accuracy << std::endl;
		fout.close();
#else
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < ATTRIBUTIONS; ++j) {
				std::cout << points[i].a[j] << ' ';
			}
			std::cout << points[i].flag << std::endl;
		}
		std::cout << "The accuracy is " << (1 - (double)m / N) << std::endl;
#endif

#endif
		return accuracy;
	}
private:
	int N;
	pointSet points;
	int cluster[MAX_CLUSTER][ATTRIBUTIONS];
	int a_num[ATTRIBUTIONS] = { 0 };
	double E;
};

int main()
{
#ifdef AUTO
	DBSCAN dbscan = DBSCAN(FILENAME);
	double l, r, h;
	std::cin >> l >> r;
	h = (r - l) / 10;
	for (int i = 0; i <= 10; ++i) {
		std::cout << "Accuracy = " << dbscan.process(l + h*i) << " where E = " << (l + h*i) << std::endl;
	}
	system("pause");
#else
	LARGE_INTEGER nFreq, nNow, nRead, nDone;
	double t_read, t_cal, t_all;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nNow);
	DBSCAN dbscan = DBSCAN(FILENAME);
	QueryPerformanceCounter(&nRead);
	double acc = dbscan.process(EP);
	QueryPerformanceCounter(&nDone);
	t_read = (double)(nRead.QuadPart - nNow.QuadPart) / (double)nFreq.QuadPart;
	t_cal = (double)(nDone.QuadPart - nRead.QuadPart) / (double)nFreq.QuadPart;
	t_all = t_read + t_cal;
	std::cout << "" << t_read << std::endl;
	std::cout << "" << t_cal << std::endl;
	std::cout << "" << t_all << std::endl;
	std::cout << "" << acc << std::endl;
	system("pause");
#endif
	return 0;
}