#include <bits/stdc++.h>
#include "swr.h"
using namespace std;
size_t N;
void test(
			const std::vector<std::vector<std::pair<size_t, int>>> &term,
			const std::vector<vec<double>> &train_set,
			const std::vector<double> &train_tag,
			const std::vector<vec<double>> &test_set,
			const std::vector<double> &test_tag,
			std::ostream &log_out) {
	std::vector<vec<double>> train_sample, test_sample;
	for (const auto &x : train_set) {
		vec<double> ins;
		ins.resize(term.size() + 1);
		size_t cnt(0);
		for (const auto &t : term) {
			ins[++cnt] = 1;
			for (const auto &v : t) ins[cnt] *= pow(x[v.first], v.second);
		}
		train_sample.push_back(ins);
	}
	normalize(train_sample, term.size() + 1);
	for (auto &x : train_sample) x[0] = 1;
	vec<double> theta = linear_regression(train_sample, train_tag, term.size() + 1, 0.05, 30000, 1);
	for (const auto &x : test_set) {
		vec<double> ins;
		ins.resize(term.size() + 1);
		size_t cnt(0);
		for (const auto &t : term) {
			ins[++cnt] = 1;
			for (const auto &v : t) ins[cnt] *= pow(x[v.first], v.second);
		}
		test_sample.push_back(ins);
	}
	normalize(test_sample, term.size() + 1);
	for (auto &x : test_sample) x[0] = 1;
	double rss = 2 * test_sample.size() * square_loss(test_sample, test_tag, theta, 0).second;
	log_out << "RMSD = " << sqrt(rss / test_sample.size()) << std::endl;
	log_out << "NRMSD = " << sqrt(rss / test_sample.size()) / (*std::max_element(test_tag.begin(), test_tag.end()) - *std::min_element(test_tag.begin(), test_tag.end())) << std::endl;
	log_out << "CV(RMSD) = " << sqrt(rss / test_sample.size()) / (std::accumulate(test_tag.begin(), test_tag.end(), 0.0) / test_sample.size()) << std::endl;
}
void concrete(istream &fin, vector<vec<double>> &sample, vector<double> &tag) {
	N = 8;
	vec<double> x;
	x.resize(N);
	double y;
	string line;
	while (getline(fin, line) && !line.empty()) {
		size_t i = 0;
		auto idx = line.begin();
		while (i < N) {
			while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
			if (*idx == '?')
				x[i++] = 1e150;
			else {
				auto from = idx;
				while (idx != line.end() && *idx != ',' && !isspace(*idx)) ++idx;
				x[i++] = stod(line.substr(from - line.begin(), idx - from));
			}
			++idx;
		}
		while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
		y = stod(line.substr(idx - line.begin(), line.end() - idx));
		sample.push_back(x);
		tag.push_back(y);
	}
}
void housing(istream &fin, vector<vec<double>> &sample, vector<double> &tag) {
	N = 13;
	vec<double> x;
	x.resize(N);
	double y;
	string line;
	while (getline(fin, line) && !line.empty()) {
		size_t i = 0;
		auto idx = line.begin();
		while (i < N) {
			while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
			if (*idx == '?')
				x[i++] = 1e150;
			else {
				auto from = idx;
				while (idx != line.end() && *idx != ',' && !isspace(*idx)) ++idx;
				x[i++] = stod(line.substr(from - line.begin(), idx - from));
			}
			++idx;
		}
		while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
		y = stod(line.substr(idx - line.begin(), line.end() - idx));
		sample.push_back(x);
		tag.push_back(y);
	}
}
void iris(istream &fin, vector<vec<double>> &sample, vector<double> &tag) {
	N = 4;
	vec<double> x;
	x.resize(N);
	double y;
	string line, ys;
	while (getline(fin, line) && !line.empty()) {
		size_t i = 0;
		auto idx = line.begin();
		while (i < N) {
			while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
			if (*idx == '?')
				x[i++] = 1e150;
			else {
				auto from = idx;
				while (idx != line.end() && *idx != ',' && !isspace(*idx)) ++idx;
				x[i++] = stod(line.substr(from - line.begin(), idx - from));
			}
			++idx;
		}
		while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
		ys = line.substr(idx - line.begin(), line.end() - idx);
		if (ys.find("setosa") < ys.size()) y = 1;
		else if (ys.find("versicolor") < ys.size()) y = 2;
		else y = 3;
		sample.push_back(x);
		tag.push_back(y);
	}
}
void servo(istream &fin, vector<vec<double>> &sample, vector<double> &tag) {
	N = 4;
	vec<double> x;
	x.resize(N);
	double y;
	string line, xs;
	while (getline(fin, line) && !line.empty()) {
		size_t i = 0;
		auto idx = line.begin();
		while (i < N) {
			while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
			if (*idx == '?')
				x[i++] = 1e150;
			else {
				auto from = idx;
				while (idx != line.end() && *idx != ',' && !isspace(*idx)) ++idx;
				xs = line.substr(from - line.begin(), idx - from);
				if (i < 2) x[i++] = xs[0] - 'A';
				else x[i++] = stod(xs);
			}
			++idx;
		}
		while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
		y = stod(line.substr(idx - line.begin(), line.end() - idx));
		sample.push_back(x);
		tag.push_back(y);
	}
}
void flare(istream &fin, vector<vec<double>> &sample, vector<double> &tag) {
	N = 12;
	vec<double> x;
	x.resize(N);
	double y;
	string line, xs;
	while (getline(fin, line) && !line.empty()) {
		size_t i = 0;
		auto idx = line.begin();
		while (i < N) {
			while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
			if (*idx == '?')
				x[i++] = 1e150;
			else {
				auto from = idx;
				while (idx != line.end() && *idx != ',' && !isspace(*idx)) ++idx;
				xs = line.substr(from - line.begin(), idx - from);
				if (i < 3) x[i++] = xs[0] - 'A';
				else x[i++] = stod(xs);
			}
			++idx;
		}
		while (idx != line.end() && (*idx == ',' || isspace(*idx))) ++idx;
		y = stod(line.substr(idx - line.begin(), line.end() - idx));
		sample.push_back(x);
		tag.push_back(y);
	}
}
int main(int argc, char **argv) {
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);
	size_t max_step = 200;
	if (argc > 3) max_step = stoi(argv[3]);
	if (max_step < 1) max_step = 200;
	vector<vec<double>> sample, train_sample, test_sample;
	vector<double> tag, train_tag, test_tag;
#ifdef CONCRETE
	concrete(fin, sample, tag);
#endif
#ifdef HOUSING
	housing(fin, sample, tag);
#endif
#ifdef IRIS
	iris(fin, sample, tag);
#endif
#ifdef SERVO
	servo(fin, sample, tag);
#endif
#ifdef FLARE
	flare(fin, sample, tag);
#endif
	double avg, sd;
	int tot;
	for (size_t i = 0; i < N; ++i) {
		avg = sd = 0;
		tot = 0;
		for (const auto &v : sample)
			if (fabs(v[i]) < 1e100) {
				++tot;
				avg += v[i];
			}
		avg /= tot;
		for (const auto &v : sample)
			if (fabs(v[i]) < 1e100) sd += (v[i] - avg) * (v[i] - avg);
		sd = sqrt(sd / tot);
		for (auto &v : sample)
			if (fabs(v[i]) < 1e100) v[i] = (v[i] - avg) / sd;
			else v[i] = 0;
	}
	random_device rd;
	default_random_engine dre(rd());
	for (size_t i = sample.size() - 1; i > 0; --i) {
		uniform_int_distribution<size_t> uid(0, i);
		size_t j = uid(dre);
		swap(sample[i], sample[j]);
		swap(tag[i], tag[j]);
	}
	size_t s = sample.size() * 0.7;
	train_sample.assign(sample.begin(), sample.begin() + s);
	test_sample.assign(sample.begin() + s, sample.end());
	train_tag.assign(tag.begin(), tag.begin() + s);
	test_tag.assign(tag.begin() + s, tag.end());
	auto res = stepwise_regression(train_sample, train_tag, N, max_step, fout);
	test(res, train_sample, train_tag, test_sample, test_tag, fout);
	return 0;
}
