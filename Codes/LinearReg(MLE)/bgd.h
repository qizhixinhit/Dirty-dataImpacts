//Author: kkcckc
//BATCH_GRADIENT_DECENT_
#ifndef BATCH_GRADIENT_DECENT_H_
#define BATCH_GRADIENT_DECENT_H_
#include<cstdlib>
#include<cstdio> 
#include<cmath> //abs 
#include<cstring> // for memset
#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<vector>
#include<limits>

namespace batchgradientdescent {
	using namespace std;
	class batchgradientdescent {
	public:
		void traininput(ifstream &fin) {
			if (!fin) over("can't open train file");
			d = 0; tuplenum = 0;
			if (!getdimension(fin)) over("get deminsion error");
			//file->string
			string s;
			fin.seekg(0, ios::end);
			s.reserve(fin.tellg());
			fin.seekg(0, ios::beg);
			s.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
			//string->double
			string::size_type s1 = 0, s2;
			for (; s1 < s.size();) {
				if (s[s1] == '\n') { s1++;;  continue; }
				tuplenum++;
				data.push_back(vector<double>(d + 2));
				data[tuplenum - 1][0] = 1;
				for (int j = 1; j <= d + 1; j++, s1 += s2 + 1) {
					if (s[s1] == missingsign) {
						data[tuplenum - 1][j] = mindouble;
						s2 = 1;
					}
					else if (s[s1]<'0' || s[s1] >'9') {
						data[tuplenum - 1][j] = (int)s[s1];//ascii
						s2 = 1;
					}
					else
						data[tuplenum - 1][j] = stod(&s[s1], &s2);
				}
			}
		}
		//void mtest(ifstream &fin, double ans[]);
		void singlebatchGR(double a = 1, int iters = 3000000) {
			const double e = 0.00001;
			//scaling(mean normalization)
			//calculate mean range
			mean.assign(d + 2, 0);//0-d+1
			vector<double> range(d + 2), max(d + 2, mindouble), min(d + 2, maxdouble);
			vector<int> num(d + 2, tuplenum);
			for (int i = 0; i < tuplenum; ++i) {
				for (int j = 0; j <= d; ++j) {
					if (data[i][j] == mindouble) {
						num[j]--; continue;
					}
					if (min[j] > data[i][j]) min[j] = data[i][j];
					if (max[j] < data[i][j]) max[j] = data[i][j];
					mean[j] += data[i][j];
				}
			}

			for (int i = 0; i <= d; ++i) {
				range[i] = max[i] - min[i];
				mean[i] /= num[i];
			}

			for (int i = 0; i < tuplenum; ++i) {
				for (int j = 0; j <= d; ++j) {
					if (!range[j]) continue;
					data[i][j] = data[i][j] != mindouble ?
						(data[i][j] - mean[j]) / range[j] : 0;//mean imputation
				}
			}

			b.assign(d + 1, 0);
			double *_b = new double[d + 1]();
			double sum = 0, tmp, J = -1, _J = 0;
			//iterate
			int i = 0;
			for (bool f = true; i < iters && f; i++) {
				for (int j = 0; j <= d; ++j) {
					//calculate _b[j]
					sum = 0;
					for (int k = 0; k < tuplenum; ++k) {
						//calculate h(x)
						tmp = 0;
						for (int l = 0; l <= d; ++l) {
							tmp += data[k][l] * b[l];
						}
						sum += (tmp - data[k][d + 1])*data[k][j];
					}
					_b[j] = b[j] - sum*a / tuplenum;
				}
				//calculate _J;
				sum = 0;
				for (int k = 0; k < tuplenum; ++k) {
					//calculate h(x)
					tmp = 0;
					for (int l = 0; l <= d; ++l) {
						tmp += data[k][l] * _b[l];
					}
					sum += pow(tmp - data[k][d + 1], 2);
				}
				_J = sum / 2 / tuplenum;
				//judge J update a
				if (_J > J && J != -1) {
					//cerr << "alpha = " << a << "is too large" << endl;
					a = a / 3;
					//cerr << "try alpha = " << a << endl;
					continue;
				}
				if (J - _J < e && J != -1) {
					//cerr << "J-_J<e ok" << endl;
					f = false;
				}
				//update J&b
				for (int j = 0; j <= d; ++j) b[j] = _b[j];
				J = _J;
				//if (i % 50 == 0)
					//cerr << "I = " << i << ", J = " << J << endl;
			}
			//cerr << "iterate " << i << " times" << endl;
			//data.clear(); data.shrink_to_fit(); delete[] _b;
			delete[] _b;
			//recovery
			//b[0] *= range[d + 1];
			for (int i = 1; i <= d; ++i) {
				b[i] = b[i] *range[d+1] / range[i];
				b[0] -= b[i] * mean[i];
			}
		}

		void test(ifstream &fin, double ans[], int multi = 1) {
			if (!fin) over("can't open test file");
			if (d <= 0) over("not trained");
			//file->string
			string s;
			fin.seekg(0, ios::end);
			s.reserve(fin.tellg());
			fin.seekg(0, ios::beg);
			s.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
			//string->double
			string::size_type s1 = 0, s2;
			vector<double> data(d + multi-1 + 2), rmsd(multi, 0), sum(multi, 0),
				max(multi, mindouble), min(multi, maxdouble);
			double tmp;
			tuplenum = 0;
			//d = d + multi - 1;
			for (; s1 < s.size();) {
				if (s[s1] == '\n') { s1++;  continue; }
				tuplenum++;
				data[0] = 1;
				for (int j = 1; j <= d + 1 + multi - 1; j++, s1 += s2 + 1) {
					if (s[s1] == missingsign) {
						data[j] = mean[j];//x
						s2 = 1;
					}
					else if (s[s1]<'0' || s[s1] >'9') {
						data[j] = (int)s[s1];//ascii
						s2 = 1;
					}
					else
						data[j] = stod(&s[s1], &s2);
				}
				//predict
				//d = d - multi + 1;
				for (int j = 0; j < multi; ++j) {
					tmp = 0;
					for (int l = 0; l <= d; ++l) tmp += data[l] * mb[j][l];
					double t = data[d + 1 + j] ;
					rmsd[j] += pow(tmp - t, 2);
					if (t > max[j]) max[j] = t;
					if (t < min[j]) min[j] = t;
					sum[j] += t;
				}
			}
			ans[0] = ans[1] = ans[2] = 0;
			for (int i = 0; i < multi; ++i) {
				ans[0] += sqrt(rmsd[i] / tuplenum) /multi;//rmsd small is good
				ans[1] += ans[0] / (max[i] - min[i]) /multi;//nrmsd small is good
				ans[2] += ans[0] / (sum[i] / tuplenum) /multi;//cv(rmsd)
			}
		}

		void over(const string &s) {
			cerr << s << endl;
			cin.get();
			exit(0);
		}

		void batchGR(ifstream &fin, double a = 1, int iters = 3000000, int multi = 1) {
			//_multi = multi;
			traininput(fin);
			if (multi == 1) {
				singlebatchGR(a, iters);
				mb.assign(1, b);
				return;
			}
			//mmean.assign(multi, (vector<double>) (d + 1, 0));//mean
			mb.assign(multi, (vector<double>) (d + 1, 0));//b
			d = d - multi + 1;
			for (int i = 0; i < multi; ++i) {
				singlebatchGR(a, iters);
				//mmean[i] = mean;
				mb[i] = b;
				if (i == multi - 1) continue;
				//swap class;
				for (int j = 0; j < tuplenum; ++j) {
					data[j][d + 1] = data[j][d + 2 + i];
				}
			}
			data.clear(); data.shrink_to_fit();
		}

		batchgradientdescent() {}

		~batchgradientdescent() {}
	private:
		const char missingsign = '?';
		const double mindouble = numeric_limits<double>::min();
		const double maxdouble = numeric_limits<double>::max();
		vector<double> mean, b;
		vector<vector<double>> mb;//for multi class
		int  d, tuplenum;

		vector<vector<double>> data;

		bool isdivide(const char &ch) {
			//if (ch == ',' || ch == ';' || ch == '£¬' || ch == '£»' || ch == ' ')
			if (ch == ' ' || ch == ',')
				return true;
			return false;
		}

		bool getdimension(ifstream &fin) {
			string line;
			getline(fin, line);
			if (line.size() <= 1) return false;
			bool f = false;
			for (auto ch : line) {
				if (isdivide(ch)) {
					if (f) {
						f = false;
						d++;
					}
				}
				else
					f = true;
			}
			fin.clear();
			fin.seekg(0, ios::beg);
			return true;
		}
	};
}//namespace
#endif