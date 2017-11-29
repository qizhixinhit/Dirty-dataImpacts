#ifndef VEC_H
#define VEC_H

#include <cstdlib>
#include <vector>
#include <cmath>

template<typename T> struct vec {
	std::vector<T> a;
	vec() {
		clear();
	}
	T &operator[](size_t i) {
		return a[i];
	}
	const T &operator[](size_t i) const {
		return a[i];
	}
	size_t size() const {
		return a.size();
	}
	size_t length() const {
		return a.size();
	}
	double sum_abs_exp(double e) const {
		double ret(0);
		for (const auto &x : a) ret += pow(fabs(x), e);
		return ret;
	}
	double sum_exp(double e) const {
		double ret(0);
		for (const auto &x : a) ret += pow(x, e);
		return ret;
	}
	void clear() {
		a.clear();
	}
	void resize(size_t s) {
		a.resize(s);
	}
	vec<T> concate(const T &rhs) const {
		vec<T> ret;
		ret.a.assign(a.begin(), a.end());
		ret.a.push_back(rhs);
		return ret;
	}
	vec<T> concate(const vec<T> &rhs) const {
		vec<T> ret;
		ret.a.assign(a.begin(), a.end());
		for (const auto &e : rhs) ret.a.push_back(e);
		return ret;
	}
};
template<typename T> vec<T> operator+(const vec<T> &lhs, const vec<T> &rhs) {
	vec<T> ret;
	ret.resize(lhs.size());
	for (size_t i = 0; i < lhs.size(); ++i) ret[i] = lhs[i] + rhs[i];
	return ret;
}
template<typename T> vec<T> &operator+=(vec<T> &lhs, const vec<T> &rhs) {
	for (size_t i = 0; i < lhs.size(); ++i) lhs[i] += rhs[i];
	return lhs;
}
template<typename T> vec<T> operator-(const vec<T> &lhs, const vec<T> &rhs) {
	vec<T> ret;
	ret.resize(lhs.size());
	for (size_t i = 0; i < lhs.size(); ++i) ret[i] = lhs[i] - rhs[i];
	return ret;
}
template<typename T> vec<T> &operator-=(vec<T> &lhs, const vec<T> &rhs) {
	for (size_t i = 0; i < lhs.size(); ++i) lhs[i] -= rhs[i];
	return lhs;
}
template<typename T> T operator*(const vec<T> &lhs, const vec<T> &rhs) {
	T ret(0);
	for (size_t i = 0; i < lhs.size(); ++i) ret += lhs[i] * rhs[i];
	return ret;
}
template<typename T> vec<T> operator*(const T &lhs, const vec<T> &rhs) {
	vec<T> ret;
	ret.resize(rhs.size());
	for (size_t i = 0; i < rhs.size(); ++i) ret[i] = lhs * rhs[i];
	return ret;
}
template<typename T> vec<T> operator*(const vec<T> &lhs, const T &rhs) {
	vec<T> ret;
	ret.resize(lhs.size());
	for (size_t i = 0; i < lhs.size(); ++i) ret[i] = lhs[i] * rhs;
	return ret;
}
template<typename T> vec<T> &operator*=(vec<T> &lhs, const T &rhs) {
	for (size_t i = 0; i < lhs.size(); ++i) lhs[i] *= rhs;
	return lhs;
}
template<typename T> vec<T> operator/(const vec<T> &lhs, const T &rhs) {
	vec<T> ret;
	ret.resize(lhs.size());
	for (size_t i = 0; i < lhs.size(); ++i) ret[i] = lhs[i] / rhs;
	return ret;
}
template<typename T> vec<T> &operator/=(vec<T> &lhs, const T &rhs) {
	for (size_t i = 0; i < lhs.size(); ++i) lhs[i] /= rhs;
	return lhs;
}

#include <iostream>
#include <vector>
#include <utility>
template<typename T> std::ostream &operator<<(std::ostream &os, const std::vector<T> &rhs) {
	os << '{';
	for (const auto &e : rhs) os << e << ',';
	return os << '}';
}
template<typename T1, typename T2> std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &rhs) {
	return os << '<' << rhs.first << ',' << rhs.second << '>';
}

#endif
