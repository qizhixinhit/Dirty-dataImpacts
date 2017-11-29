#ifndef LVQ_H
#define LVQ_H

#include "vec.h"
#include <vector>
#include <iostream>
#include <random>
#include <map>
#include <cmath>

template<size_t N> class lvq {
public:
	lvq() : dre(rd()) {}
	std::pair<std::vector<vec<double, N>>, std::vector<int>> train (
		const std::vector<vec<double, N>> &sample,
		const std::vector<int> &sample_tag,
		const std::vector<vec<double, N>> &prototype_vector,
		const std::vector<int> &prototype_vector_tag,
		double learning_rate,
		size_t max_iter,
		double mink_dis_p = 2,
		size_t log_gap = 0,
		std::ostream &log_out = std::cout,
		const std::vector<int> &actual_tag = std::vector<int>()) {

		std::uniform_int_distribution<size_t> ri(0, sample.size() - 1);
		std::vector<vec<double, N>> pbest;
		double sdbw_min;

		s.assign(sample.begin(), sample.end());
		st.assign(sample_tag.begin(), sample_tag.end());
		p.assign(prototype_vector.begin(), prototype_vector.end());
		pt.assign(prototype_vector_tag.begin(), prototype_vector_tag.end());
		c = prototype_vector_tag.size();
		if (p.size() != pt.size()) {
			p.clear();
			for (size_t i = 0; i < pt.size(); ++i) p.push_back(sample[ri(dre)]);
		}
		dis_p = mink_dis_p;
		pbest.assign(p.begin(), p.end());
		sdbw_min = sdbw(true);

		for (size_t num_iter = 1; num_iter <= max_iter; ++num_iter) {
			double dis, dis_min(0);
			size_t vec_min(0);
			vec_min = ~vec_min;
			size_t x(ri(dre));
			for (size_t i = 0; i < p.size(); ++i) {
				dis = (p[i] - s[x]).sum_abs_exp(dis_p);
				if (!~vec_min || dis < dis_min) {
					dis_min = dis;
					vec_min = i;
				}
			}
			if (st[x] == pt[vec_min]) p[vec_min] += learning_rate * (s[x] - p[vec_min]);
			else p[vec_min] -= learning_rate * (s[x] - p[vec_min]);
			if (num_iter % ((s.size() + 4) / 5) == 0) {
				std::cerr << num_iter << std::endl;
				double tmp = sdbw(true);
				if (tmp < sdbw_min) {
					sdbw_min = tmp;
					pbest.assign(p.begin(), p.end());
				}
			}
			if (log_gap > 0 && num_iter % log_gap == 0) {
				log_out << "iteration " << num_iter << std::endl;
				print(log_out, actual_tag);
			}
		}
		p.assign(pbest.begin(), pbest.end());
		return {p, pt};
	}
	void print(std::ostream &log_out = std::cout, const std::vector<int> &actual_tag = std::vector<int>()) {
		partition_by_prototype_vector();
		log_out << "prototype vectors:" << std::endl;
		for (size_t i = 0; i < p.size(); ++i) {
			log_out << "[" << pt[i] << "]\t";
			for (size_t j = 0; j < N; ++j) log_out << p[i][j] << " ";
			log_out << std::endl;
		}
		std::vector<std::map<int, int>> cnt(p.size());
		for (const auto &x : actual_tag) cnt[0][x] = 0;
		for (const auto &x : cnt[0])
			for (size_t i = 1; i < cnt.size(); ++i)
				cnt[i][x.first] = 0;
        log_out << "distributions:" << std::endl;
        for (const auto &x : cnt[0]) log_out << x.first << "\t";
        log_out << std::endl;
        for (size_t i = 0; i < p.size(); ++i) {
			for (const auto &x : cls[i]) ++cnt[i][actual_tag[x]];
			for (const auto &x : cnt[i]) log_out << x.second << "\t";
			log_out << std::endl;
        }
		log_out << "F1-measure = " << f_measure(1, actual_tag, false) << std::endl;
		log_out << "S_Dbw index = " << sdbw(false) << std::endl;
		log_out << std::endl;
	}
	double f_measure(double alpha, const std::vector<int> &actual_tag, bool repartition = true) {
		if (repartition) partition_by_prototype_vector();
		double pre, rec, f(0);
		std::vector<std::map<int, int>> cnt;
		int pp, pn, np;
		std::vector<int> tag_now;
		for (size_t i = 0; i < p.size(); ++i) {
			int tag_max(0), cnt_max;
			tag_max = ~tag_max;
			cnt.push_back(std::map<int, int>());
			for (const auto &j : cls[i]) ++cnt[i][actual_tag[j]];
			for (const auto &j : cnt[i])
				if (!~tag_max || cnt_max < j.second) {
					tag_max = j.first;
					cnt_max = j.second;
				}
			tag_now.push_back(tag_max);
		}
		for (size_t i = 0; i < p.size(); ++i) {
			pp = pn = np = 0;
			for (size_t j = 0; j < s.size(); ++j)
				if (nv[j] == i)
					if (actual_tag[j] == tag_now[i]) ++pp;
					else ++pn;
				else if (actual_tag[j] == tag_now[i]) ++np;
			if (pp || pn) pre = static_cast<double>(pp) / (pp + pn);
			else pre = 1;
			if (pp || np) rec = static_cast<double>(pp) / (pp + np);
			else rec = 1;
			f += (1 + alpha * alpha) * pre * rec / (alpha * alpha * pre + rec);
		}
		return f / p.size();
	}
	double sdbw(bool repartition = true) {
		if (repartition) partition_by_prototype_vector();
		vec<double, N> tmp;
		double stdev_2(0), scat(0), densbw(0);
		for (const auto &cl : cls) {
			tmp = var_vec(cl);
			double v = sqrt(tmp * tmp);
			stdev_2 += v;
			scat += v;
		}
		stdev_2 /= c * c;
		std::vector<int> vtmp;
		for (size_t i = 0; i < s.size(); ++i) vtmp.push_back(i);
		tmp = var_vec(vtmp);
		scat /= sqrt(tmp * tmp) * c;
		for (size_t i = 0; i < c; ++i)
			for (size_t j = i + 1; j < c; ++j) {
				tmp = (p[i] + p[j]) * 0.5;
				double u = sdbw_dens(cls[i], tmp, stdev_2) + sdbw_dens(cls[j], tmp, stdev_2);
				double d = std::max(sdbw_dens(cls[i], p[i], stdev_2) + sdbw_dens(cls[j], p[i], stdev_2),
							sdbw_dens(cls[i], p[j], stdev_2) + sdbw_dens(cls[j], p[j], stdev_2));
				if (fabs(d) < 1e-9) densbw = 1;
				else densbw += u / d;
			}
		if (c > 1) densbw /= c * (c - 1) / 2;
		return densbw + scat;
	}
	void partition_by_prototype_vector() {
		cls.clear();
		nv.clear();
		for (size_t i = 0; i < p.size(); ++i) cls.push_back(std::vector<int>());
		for (size_t i = 0; i < s.size(); ++i) {
			double dis, dis_min;
			size_t arg_min(0);
			arg_min = ~arg_min;
			for (size_t j = 0; j < p.size(); ++j) {
				dis = (p[j] - s[i]).sum_abs_exp(dis_p);
				if (!~arg_min || dis < dis_min) {
					dis_min = dis;
					arg_min = j;
				}
			}
			cls[arg_min].push_back(i);
			nv.push_back(arg_min);
		}
	}
//private:
	std::random_device rd;
	std::default_random_engine dre;
	std::vector<vec<double, N>> s, p;
	std::vector<int> st, pt;
	size_t c;
	std::vector<int> nv;
	std::vector<std::vector<int>> cls;
	double dis_p;
	vec<double, N> avg_vec(const std::vector<int> &svec) {
		vec<double, N> ret;
		for (const auto &x : svec) ret += s[x] / static_cast<double>(svec.size());
		return ret;
	}
	vec<double, N> var_vec(const std::vector<int> &svec) {
		vec<double, N> avg(avg_vec(svec)), ret, tmp;
		for (const auto &x : svec) {
			tmp = s[x] - avg;
			for (size_t i = 0; i < N; ++i)
				ret[i] += tmp[i] * tmp[i] / svec.size();
		}
		return ret;
	}
	double sdbw_dens(const std::vector<int> &svec, const vec<double, N> &ctl, double stdev_2) {
		double ret(0);
		for (const auto &i : svec)
			if ((s[i] - ctl).sum_abs_exp(2) < stdev_2) ++ret;
		return ret;
	}
};

#endif
