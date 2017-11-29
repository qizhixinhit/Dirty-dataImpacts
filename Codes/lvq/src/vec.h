#ifndef VEC_H
#define VEC_H

#include <cstdlib>
#include <vector>
#include <cstring>
#include <cmath>

template <typename T, size_t N> struct vec {
	T a[N];
	vec() {
		memset(a, 0, sizeof(a));
	}
	T &operator[](size_t i) {
		return a[i];
	}
	const T &operator[](size_t i) const {
		return a[i];
	}
	static constexpr size_t length() {
		return N;
	}
	double sum_abs_exp(double e) const {
		double ret(0);
		for (size_t i = 0; i < N; ++i) ret += pow(fabs(a[i]), e);
		return ret;
	}
	double sum_exp(double e) const {
		double ret(0);
		for (size_t i = 0; i < N; ++i) ret += pow(a[i], e);
		return ret;
	}
	void clear() {
		for (size_t i = 0; i < N; ++i) a[i] = T();
	}
};
template <typename T, size_t N> vec<T, N> operator+(const vec<T, N> &lhs, const vec<T, N> &rhs) {
	vec<T, N> ret;
	for (size_t i = 0; i < N; ++i) ret[i] = lhs[i] + rhs[i];
	return ret;
}
template <typename T, size_t N> vec<T, N> &operator+=(vec<T, N> &lhs, const vec<T, N> &rhs) {
	for (size_t i = 0; i < N; ++i) lhs[i] += rhs[i];
	return lhs;
}
template <typename T, size_t N> vec<T, N> operator-(const vec<T, N> &lhs, const vec<T, N> &rhs) {
	vec<T, N> ret;
	for (size_t i = 0; i < N; ++i) ret[i] = lhs[i] - rhs[i];
	return ret;
}
template <typename T, size_t N> vec<T, N> &operator-=(vec<T, N> &lhs, const vec<T, N> &rhs) {
	for (size_t i = 0; i < N; ++i) lhs[i] -= rhs[i];
	return lhs;
}
template <typename T, size_t N> T operator*(const vec<T, N> &lhs, const vec<T, N> &rhs) {
	T ret(0);
	for (size_t i = 0; i < N; ++i) ret += lhs[i] * rhs[i];
	return ret;
}
template <typename T, size_t N> vec<T, N> operator*(const T &lhs, const vec<T, N> &rhs) {
	vec<T, N> ret;
	for (size_t i = 0; i < N; ++i) ret[i] = lhs * rhs[i];
	return ret;
}
template <typename T, size_t N> vec<T, N> operator*(const vec<T, N> &lhs, const T &rhs) {
	vec<T, N> ret;
	for (size_t i = 0; i < N; ++i) ret[i] = lhs[i] * rhs;
	return ret;
}
template <typename T, size_t N> vec<T, N> &operator*=(vec<T, N> &lhs, const T &rhs) {
	for (size_t i = 0; i < N; ++i) lhs[i] *= rhs;
	return lhs;
}
template <typename T, size_t N> vec<T, N> operator/(const vec<T, N> &lhs, const T &rhs) {
	vec<T, N> ret;
	for (size_t i = 0; i < N; ++i) ret[i] = lhs[i] / rhs;
	return ret;
}
template <typename T, size_t N> vec<T, N> &operator/=(vec<T, N> &lhs, const T &rhs) {
	for (size_t i = 0; i < N; ++i) lhs[i] /= rhs;
	return lhs;
}

#endif
