#ifndef SWR_H
#define SWR_H

#include "vec.h"
#include "lr.h"
#include <vector>
#include <set>
#include <cmath>
#include <iostream>

inline double aicc(size_t num_sample, double rss, size_t num_para) {
	return num_sample * log(rss) + 2 * num_para + 2.0 * num_para * (num_para + 1) / (num_sample - num_para - 1);
}
inline double test_poly(const std::set<size_t> &poly, const std::vector<std::vector<std::pair<size_t, int>>> &candidate, const std::vector<vec<double>> &sample, const std::vector<double> &tag) {
	std::vector<vec<double>> tsample;
	for (const auto &x : sample) {
		vec<double> ins;
		ins.resize(poly.size() + 1);
		size_t cnt(0);
		for (const auto &t : poly) {
			ins[++cnt] = 1;
			for (const auto &v : candidate[t]) ins[cnt] *= pow(x[v.first], v.second);
		}
		tsample.push_back(ins);
	}
	auto avg_sd = normalize(tsample, poly.size() + 1);
	for (auto &x : tsample) x[0] = 1;
	double ret;
	vec<double> theta = linear_regression(tsample, tag, poly.size() + 1, 0.05, 30000, 1);
	ret = aicc(tsample.size(), 2 * tsample.size() * square_loss(tsample, tag, theta, 1).second, poly.size() + 1);
	return ret;
}
inline std::vector<std::vector<std::pair<size_t, int>>> stepwise_regression(const std::vector<vec<double>> &sample, const std::vector<double> &tag, size_t dim, size_t max_step, std::ostream &os) {
	std::vector<std::vector<std::pair<size_t, int>>> candidate, ret;
	std::set<size_t> poly;
	int change_state;
	double aicc, taicc;
	size_t step(0);
	for (size_t i = 0; i < dim; ++i)
		candidate.push_back({{i, 1}});
	for (size_t i = 0; i < dim; ++i)
		for (size_t j = i + 1; j < dim; ++j)
			candidate.push_back({{i, 1}, {j, 1}});
	for (int i = 2; i <= 4; ++i)
		for (size_t j = 0; j < dim; ++j)
			candidate.push_back({{j, i}});
	for (size_t i = 0; i < dim; ++i) poly.insert(i);
	aicc = test_poly(poly, candidate, sample, tag);
	std::set<size_t> tpoly(poly);
	for (const auto &t : poly) {
		tpoly.erase(t);
		taicc = test_poly(tpoly, candidate, sample, tag);
		if (taicc < aicc) {
			aicc = taicc;
			os << aicc << std::endl;
			for (const auto &x : tpoly) os << candidate[x] << " ";
			os << std::endl;
		} else tpoly.insert(t);
	}
	poly = tpoly;
	do {
		change_state = 0;
		for (size_t i = 0; !change_state && i < candidate.size(); ++i)
			if (poly.find(i) == poly.end()) {
				++step;
				poly.insert(i);
				taicc = test_poly(poly, candidate, sample, tag);
				if (taicc < aicc) {
					change_state = 1;
					aicc = taicc;
					os << aicc << std::endl;
					for (const auto &x : poly) os << candidate[x] << " ";
					os << std::endl;
					std::set<size_t> tpoly(poly);
					for (const auto &t : poly) {
						++step;
						tpoly.erase(t);
						taicc = test_poly(tpoly, candidate, sample, tag);
						if (taicc < aicc) {
							aicc = taicc;
							os << aicc << std::endl;
							for (const auto &x : tpoly) os << candidate[x] << " ";
							os << std::endl;
						}
						else tpoly.insert(t);
					}
					poly = tpoly;
				} else poly.erase(i);
			}
	} while (change_state && step < max_step);
	for (const auto &x : poly) ret.push_back(candidate[x]);
	return ret;
}

#endif
