#ifndef LR_H
#define LR_H

#include "vec.h"
#include <vector>
#include <utility>
#include <cmath>

inline std::pair<vec<double>, vec<double>> normalize(std::vector<vec<double>> &sample, size_t dim) {
	vec<double> avg, sd;
	avg.resize(dim);
	sd.resize(dim);
	for (size_t i = 0; i < dim; ++i) {
		avg[i] = sd[i] = 0;
		for (const auto &v : sample) avg[i] += v[i];
		avg[i] /= sample.size();
		for (const auto &v : sample) sd[i] += (v[i] - avg[i]) * (v[i] - avg[i]);
		sd[i] = sqrt(sd[i] / sample.size());
		for (auto &v : sample) v[i] = (v[i] - avg[i]) / sd[i];
	}
	return {avg, sd};
}
inline std::pair<double, double> square_loss(const std::vector<vec<double>> &sample, const std::vector<double> &tag, const vec<double> &theta, double lambda) {
	double loss(0), lossr;
	for (size_t i = 0; i < sample.size(); ++i) {
		double diff(sample[i] * theta - tag[i]);
		loss += diff * diff;
	}
	lossr = loss + theta * theta;
	return {lossr * 0.5 / sample.size(), loss * 0.5 / sample.size()};
}
inline vec<double> linear_regression(const std::vector<vec<double>> &sample, const std::vector<double> &tag, size_t dim, double learning_rate, size_t max_iter, double lambda, double error_bound = 1e-4) {
	vec<double> theta, grad, ttheta;
	theta.resize(dim);
	ttheta = theta;
	size_t i, cnt;
	for (i = 0, cnt = 0; i < max_iter; ++i, ++cnt) {
		grad = lambda / sample.size() * theta;
		for (size_t j = 0; j < sample.size(); ++j) grad += (theta * sample[j] - tag[j]) * sample[j];
		theta -= learning_rate / sample.size() * grad;
		if (~cnt & 31) {
			size_t k;
			for (k = 0; k < dim; ++k)
				if (fabs(ttheta[k] - theta[k]) > error_bound) break;
			if (k < dim) ttheta = theta;
			else break;
		}
	}
	if (i < max_iter) std::cerr << i << std::endl;
	return theta;
}

#endif
