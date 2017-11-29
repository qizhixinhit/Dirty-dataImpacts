#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <cstring>
#include <bits/stdc++.h>
#include "vec.h"
#include "lvq.h"
using namespace std;

random_device rd;
default_random_engine dre(rd());

void seeds() {
	ifstream fin("seeds_dataset.data");
	ofstream log_out("log_seeds.txt");
	const size_t N(7);
	lvq<N> solver;
	vector<vec<double, N>> sample;
	vector<vec<double, N>> prototype;
	vector<int> sample_tag, prototype_tag, fake_tag;
	vec<double, N> x;
	int y;
	map<int, int> cnt_y;
	while (fin >> x[0]) {
		for (size_t i = 1; i < N; ++i) fin >> x[i];
		fin >> y;
		sample.push_back(x);
		++cnt_y[y];
		sample_tag.push_back(y);
		fake_tag.push_back(0);
	}
	uniform_int_distribution<int> ran(1, sample.size());
	for (size_t i = 0; i < 3; ++i) {
		prototype.push_back(sample[ran(dre) - 1]);
		prototype_tag.push_back(0);
	}
	auto result = solver.train(sample, fake_tag, prototype, prototype_tag, 0.2, 10000000, 2, 5000, log_out, sample_tag);
	solver.print(log_out, sample_tag);
}
void water() {
	ifstream fin("water-treatment.data");
	ofstream log_out("log_water.txt");
	const size_t N(38);
	lvq<N> solver;
	vector<vec<double, N>> sample;
	vector<vec<double, N>> prototype;
	vector<int> prototype_tag, actual_tag, fake_tag;
	vec<double, N> x;
	int y;
	while (true) {
		while ((y = fin.get()) != EOF && y != ',');
		if (y == EOF) break;
		for (size_t i = 0; i < N; ++i) {
			string tmp;
			tmp += static_cast<char>(fin.get());
			if (tmp[0] == '?') x[i] = 1e105;
			else {
				while ((y = fin.get()) != EOF && y != ',' && isgraph(y)) tmp += static_cast<char>(y);
				x[i] = atof(tmp.c_str());
			}
		}
		sample.push_back(x);
		fake_tag.push_back(0);
	}
	double avg, sd;
	int tot;
	for (size_t i = 0; i < N; ++i) {
		avg = sd = 0;
		tot = 0;
		for (const auto &v : sample)
			if (v[i] < 1e100) {
				++tot;
				avg += v[i];
			}
		avg /= tot;
		for (const auto &v : sample)
			if (v[i] < 1e100) sd += (v[i] - avg) * (v[i] - avg);
		sd = sqrt(sd / tot);
		for (auto &v : sample)
			if (v[i] < 1e100) v[i] = (v[i] - avg) / sd;
			else v[i] = 0;
	}
	actual_tag.assign(sample.size(), 0);
	uniform_int_distribution<int> ran(1, sample.size());
	for (size_t i = 0; i < 5; ++i) {
		prototype.push_back(sample[ran(dre) - 1]);
		prototype_tag.push_back(0);
	}
	auto result = solver.train(sample, fake_tag, prototype, prototype_tag, 0.2, 1000000, 2, 5000, log_out, actual_tag);
	solver.print(log_out, actual_tag);
}
void HTRU2() {
	ifstream fin("HTRU_2.csv");
	ofstream log_out("log_HTRU_2.txt");
	const size_t N(8);
	lvq<N> solver;
	vector<vec<double, N>> sample;
	vector<vec<double, N>> prototype;
	vector<int> prototype_tag, actual_tag, fake_tag;
	vec<double, N> x;
	int y;
	while (fin >> x[0]) {
		fin.get();
		for (size_t i = 1; i < N; ++i) {
			fin >> x[i];
			fin.get();
		}
		sample.push_back(x);
		fake_tag.push_back(0);
		fin >> y;
		actual_tag.push_back(y);
	}
	double avg, sd;
	int tot;
	for (size_t i = 0; i < N; ++i) {
		avg = sd = 0;
		tot = 0;
		for (const auto &v : sample) {
			++tot;
			avg += v[i];
		}
		avg /= tot;
		for (const auto &v : sample) sd += (v[i] - avg) * (v[i] - avg);
		sd = sqrt(sd / tot);
		for (auto &v : sample) v[i] = (v[i] - avg) / sd;
	}
	uniform_int_distribution<int> ran(1, sample.size());
	for (size_t i = 0; i < 2; ++i) {
		prototype.push_back(sample[ran(dre) - 1]);
		prototype_tag.push_back(0);
	}
	auto result = solver.train(sample, fake_tag, prototype, prototype_tag, 0.03, 1000000, 2, 2000, log_out, actual_tag);
	solver.print(log_out, actual_tag);
}
void road() {
	ifstream fin("3D_spatial_network.data");
	ofstream log_out("log_road.txt");
	const size_t N(3);
	lvq<N> solver;
	vector<vec<double, N>> sample;
	vector<vec<double, N>> prototype;
	vector<int> prototype_tag, actual_tag, fake_tag;
	vec<double, N> x;
	int y;
	map<int, int> cnt_y;
	while (fin >> y) {
		if (cnt_y.size() == 50 && y != *actual_tag.end()) break;
		fin.get();
		for (size_t i = 0; i < N; ++i) {
			fin >> x[i];
			fin.get();
		}
		sample.push_back(x);
		fake_tag.push_back(0);
		actual_tag.push_back(y);
		++cnt_y[y];
	}
	cerr << cnt_y.size() << endl;
	double avg, sd;
	int tot;
	for (size_t i = 0; i < N; ++i) {
		avg = sd = 0;
		tot = 0;
		for (const auto &v : sample) {
			++tot;
			avg += v[i];
		}
		avg /= tot;
		for (const auto &v : sample) sd += (v[i] - avg) * (v[i] - avg);
		sd = sqrt(sd / tot);
		for (auto &v : sample) v[i] = (v[i] - avg) / sd;
	}
	uniform_int_distribution<int> ran(1, sample.size());
	for (size_t i = 0; i < cnt_y.size(); ++i) {
		prototype.push_back(sample[ran(dre) - 1]);
		prototype_tag.push_back(0);
	}
	auto result = solver.train(sample, fake_tag, prototype, prototype_tag, 0.3, 600000, 2, 5000, log_out, actual_tag);
	solver.print(log_out, actual_tag);
}
int main() {
	//seeds();
	//water();
	//HTRU2();
	road();
	return 0;
}
