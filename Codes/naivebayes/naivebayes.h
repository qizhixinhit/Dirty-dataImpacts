//Author: kkcckc
//Naive Bayes Classify
#ifndef NAIVEBAYES_NAIVEBAYES_NAIVEBAYES_H_
#define NAIVEBAYES_NAIVEBAYES_NAIVEBAYES_H_

#include<iomanip>
#include<iostream>
#include<string>
#include<array>
#include<fstream>
#include<map>

namespace naivebayes {
using namespace std;

class NaiveBayes {
public:
	void train(ifstream &fin) {
		if (!fin) {
			cerr << "can't open train file" << endl;
			return;
		}
		init();
		if (!getdimension(fin)) {
			cerr << "get deminsion error" << endl;
			return;
		}
		s = new string[d + 1];
		feature = new map<string, map<string, int>>[d];
		while (fin) {
			string line;
			getline(fin, line);
			if (line.size() > 1) {
				pretreatement(line);
				datasize++;
				ccount[s[d]]++;
				for (int i = 0; i < d; i++) {
					feature[i][s[i]][s[d]]++;
				}
			}
		}
	}

	double test(ifstream &fin) {//output: correct rate
		if (!fin) {
			cerr << "can't open test file" << endl;
			return 0;
		}
		if (!istrained()) {
			cerr << "not trained" << endl;
			return 0;
		}

		string maxstring;
		int k = 0;
		int cmd = ccount.size();//cm dimension
		int *cm = new int[cmd*cmd]{ 0 };//confusion matrix
		map<string, int> mm;
		for (auto m : ccount) mm[m.first] = k++;//classification map

		while (fin) {
			if (!predict_a_line(fin, maxstring)) continue;
			cm[mm[s[d]] * cmd + mm[maxstring]]++;
		}

		double F1sum(0), colsum, rowsum, P, R;
		int cmdd = cmd;
		for (int i = 0; i < cmd; ++i) {
			if (cm[i*cmd + i] == 0) continue;
			colsum = rowsum = 0;
			for (int j = 0; j < cmd; ++j) {
				colsum += cm[j*cmd + i];
				rowsum += cm[i*cmd + j];
			}
			if (colsum == 0) {
				--cmdd;
				continue;//预测集中没有
			}
			P = cm[i*cmd + i] / colsum;
			R = cm[i*cmd + i] / rowsum;
			F1sum += 2 * P * R / (P + R);
		}
		return F1sum/cmdd;
	}

	void predict(ifstream &fin, ofstream &fout) {
		if (!fin) {
			cerr << "can't open predict file" << endl;
			return;
		}
		if (!fout) {
			cerr << "can't open result file" << endl;
			return;
		}
		if (!istrained()) {
			cerr << "not trained" << endl;
			return;
		}
		string maxstring;
		while (fin) {
			if (!predict_a_line(fin, maxstring)) continue;
			for (int i = 0; i < d; ++i) {
				fout << s[i] << ", ";
			}
			fout << maxstring << endl;
		}
	}

	NaiveBayes() :datasize(0), s(NULL), d(0), feature(NULL) {}
private:
	string *s;
	int datasize, d;
	map<string, int> ccount;
	map<string, map<string, int>> *feature;
	void init() {
		d = datasize = 0;
		ccount.clear();
		delete[] feature;
		delete[] s;
	}

	bool istrained() {
		if (datasize == 0 || d == 0 || ccount.size() == 0 ||
			feature == NULL || s == NULL)
			return false;
		return true;
	}

	bool isdivide(const char &ch) {
		if (ch == ',' || ch == ';' || ch == '，' || ch == '；' || ch == ' ')
			return true;
		return false;
	}

	bool predict_a_line(ifstream &fin, string &maxstring) {
		string line;
		getline(fin, line);
		if (line.size() <= 1) return false;
		pretreatement(line);
		double k = 1, max, r;
		for (auto i = ccount.begin(); i != ccount.end(); i++) {
			double p = log(double(i->second)), t = i->second;
			for (int j = 0; j < d; j++) {
				r = feature[j][s[j]][i->first];
				p += log(r + k) - log(t + k*feature[j].size());
			}
			if (i == ccount.begin() || p > max) {
				maxstring = i->first;
				max = p;
			}
		}
		return true;
	}

	bool getdimension(ifstream &fin) {
		string line;
		getline(fin, line);
		if (line.size() <= 1) return false;
		for (auto ch : line) {
			if (isdivide(ch))
				d++;
		}
		fin.seekg(0, ios::beg);
		return true;
	}

	void pretreatement(string &line) {
		for (auto i = line.begin(); i != line.end(); ++i) {
			if (isdivide(*i))
				*i = ' ';
		}
		int q = -1, p;
		for (int i = 0; i <= d; i++) {
			p = q + 1;
			while (line[p] == ' ') ++p;
			q = line.find(' ', p);
			s[i] = line.substr(p, q - p);
		}
	}
};

}
#endif //NAIVEBAYES_NAIVEBAYES_NAIVEBAYES_H_