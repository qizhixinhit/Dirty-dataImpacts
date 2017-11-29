#if defined(_WIN32)
#define pd "\\"
#define mkdir(aaa) {string t = "md " + aaa; system(t.c_str());}
#define rmdir(aaa) {string t = "rd /s /q " + aaa; system(t.c_str());}
//#elif defined(__linux__)
#else 
#define pd "/"
#define mkdir(aaa) {string t = "mkdir -p " + aaa; system(t.c_str());}
#define rmdir(aaa) {string t = "rm -Rf " + aaa; system(t.c_str());}
#endif
#include"bgd.h"
#include<cstdlib>
#include<cstdio>
#include<cmath>
#include<string>
//#include<cstring>
#include<ctime>
#include<fstream>
//#include <iomanip>
#include<string>
#include<random>
#include<iostream>
using namespace std;
default_random_engine gen((int)time(0));
ofstream fout;
void over(const string &s = "can't open file") {
	cerr << s << endl;
	cin.get();
	exit(0);
}

void t(const string &s1, const string &s2, double ans[][4], int m ,int multi=1) {
	clock_t start, end;
	for (int i = 10; i <= m; i += 10) {
		ifstream ftrain(s1 + pd + to_string(i) + "%-" + s2 + "7.data");
		ifstream ftest(s1 + +pd + to_string(i) + "%-" + s2 + "3.data");
		if (!ftrain || !ftest) over("testfile wrong");
		start = clock();
		batchgradientdescent::batchgradientdescent bgd;
		bgd.batchGR(ftrain,1 ,3000000, multi);
		bgd.test(ftest, ans[i / 10 - 1], multi);
		end = clock();
		ans[i / 10 - 1][3] = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
		ftrain.close();
		ftest.close();
	}
}

void split(const string &s2, const string &s3, int m) {
	string dir73 = s2 + "73";
	string dir = s2;
	for (int i = 10; i <= m; i += 10) {
		ifstream fin(dir + pd + to_string(i) + "%-" + s3 + ".data");
		ofstream fout1(dir73 + pd + to_string(i) + "%-" + s3 + "7.data");
		ofstream fout2(dir73 + pd + to_string(i) + "%-" + s3 + "3.data");
		if (!fin || !fout1 || !fout2) over("split wrong");
		vector<string> v;
		string s;
		while (fin) {
			getline(fin, s);
			if (s.size() <= 1) continue;
			v.push_back(s);
		}
		int tuplenum = (int)v.size();
		bool *is = new bool[tuplenum]();
		uniform_int_distribution<int> dis(0, tuplenum - 1);
		for (int i = 0; i < round(tuplenum*0.3); ++i) {
			int r = dis(gen);
			if (!is[r])
				is[r] = 1;
			else
				--i;
		}
		for (int i = 0; i < tuplenum; i++) {
			if (!is[i])
				fout1 << v[i] << "\n";
			else
				fout2 << v[i] << "\n";
		}
		fin.close();
		fout1.close();
		fout2.close();
	}
}

void tt(const string &s1, const string &s, int m, int multi =1) {
	double ans[5][4], tmp[5][4];
	memset(ans, 0, sizeof(ans));
	mkdir(s1 + "73")
	for (int i = 0; i < 3; ++i) {
		split(s1, s, m);
		t(s1 + "73", s, tmp, m, multi);
		for (int j = 0; j < m / 10; ++j)
			for (int k = 0; k < 4; ++k)
				ans[j][k] += tmp[j][k] / 3;
	}
	fout << "\"linearR_" << s1 << "\"\n";
	if (m == 50) fout << "\"uniform\","; else fout << "\"normal\",";
	for (int i = 10; i <= m; i += 10) fout << "\"" << to_string(i) + "%-" + s << "\",";
	fout << "\n";
	for (int j = 0; j < 4; ++j) {
		if (j==0) fout << "\"rmsd\","; else if (j==1) fout << "\"nrmsd\","; 
		else if (j==2) fout << "\"cv(rmsd)\","; else fout << "\"time(ms)\",";
		for (int i = 0; i < m / 10; i += 1)
			fout << "\"" << ans[i][j] << "\",";
		fout << "\n";
	}
	fout << "\n";
	rmdir(s1 + "73")
}
void ttt(const string &ss,int multi = 1) {
	fout.open("linearR_"+ss+".csv");
	if (!fout) over("can't open out.csv");
	//cout.rdbuf(fout.rdbuf());
	tt(ss, "miss", 50, multi);
	tt(ss, "incons", 50, multi);
	tt(ss, "conf", 50, multi);
	tt(ss+ "_normal", "miss", 30, multi);
	tt(ss+ "_normal", "conf", 30, multi);
	fout.close();
}
int main() {
	ttt("housing");
	ttt("iris");
	ttt("concrete");
	ttt("flare",3);
	ttt("servo");
	cerr << "ok";
	//cin.get();
	return 0;
}