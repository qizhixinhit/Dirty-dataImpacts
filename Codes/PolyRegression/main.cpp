#include <cstdio>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define M_MAX 10000
#define ATTRIBUTION_MAX 100
#define INFINITE 10000
int M, N;
int ATTRIBUTION;
int power;
double alpha;
double y_avg, y_s;
double Data[M_MAX][ATTRIBUTION_MAX];
double Y[M_MAX];

double H(double theta[], int k) {
	double ans = 0;
	ans += theta[0];
	for (int i = 0; i < ATTRIBUTION; ++i) {
		ans += theta[i+1] * Data[k][i];
	}
	return ans;
}

double cost(double theta[]) {
	double sum = 0;
	for (int i = 0; i < N; ++i) {
		double d = H(theta, i) - Y[i];
		sum += d*d;
	}
	return sum / (2 * N);
}

void GradientDescent(double theta[]) {
	double new_theta[ATTRIBUTION_MAX];
	double sum = 0;
	for (int i = 0; i < N; ++i) {
		sum += H(theta, i)-Y[i];
	}
	new_theta[0] = theta[0] - (alpha / N)*sum;
	for (int attr = 0; attr < ATTRIBUTION; ++attr) {
		sum = 0;
		for (int i = 0; i < N; ++i) {
			sum += (H(theta, i) - Y[i])*Data[i][attr];
		}
		new_theta[attr+1] = theta[attr+1] - (alpha / N) * sum;//stimultaneously update 同步更新
	}
	for (int i = 0; i < ATTRIBUTION+1; ++i) theta[i] = new_theta[i];
}

void normalize(int attr) {
	double max, min, avg;
	int num = 0;
	for (int i = 0; i < attr; i += power) {
		max = -INFINITE; min = INFINITE; avg = 0; num = 0;
		for (int j = 0; j < M; ++j) {
			if (Data[j][i] != INFINITE) {
				num++;
				avg += Data[j][i];
				if (max < Data[j][i]) max = Data[j][i];
				if (min > Data[j][i]) min = Data[j][i];
			}
		}
		avg /= num;
		for (int j = 0; j < M; ++j) {
			if (Data[j][i] != INFINITE) {
				Data[j][i] = (Data[j][i] - avg) / (max - min);
				for (int k = 1; k < power; ++k) {
					Data[j][i + k] = Data[j][i + k - 1] * Data[j][i];
				}
			}
			else {
				Data[j][i] = 0;
				for (int k = 1; k < power; ++k) {
					Data[j][i + k] = 0;
				}
			}
		}
	}
	max = -INFINITE; min = INFINITE; avg = 0; num = 0;
	for (int j = 0; j < N; ++j) {
		if (Y[j] != INFINITE) {
			num++;
			avg += Y[j];
			if (max < Y[j]) max = Y[j];
			if (min > Y[j]) min = Y[j];
		}
	}
	avg /= num;
	y_avg = avg;
	y_s = max - min;
	for (int j = 0; j < N; ++j) {
		if (Y[j] != INFINITE) {
			Y[j] = (Y[j] - avg) / (max - min);
		}
		else {
			Y[j] = 0;
		}
	}
}

int main(int argc, char* argv[])//参数：文件名、属性个数、速率、迭代次数、最高次方、e
{
	double e = atof(argv[6]);
	power = atoi(argv[5]);
	int iter = atoi(argv[4]);
	alpha = atof(argv[3]);
	int attr = atoi(argv[2]);
	ATTRIBUTION = attr*power;
	std::string st;
	std::fstream fin;
	fin.open(argv[1], std::fstream::in);
	int n = 0;
	while (std::getline(fin, st)) {
		n++;
	}
	fin.close();
	fin.open(argv[1], std::fstream::in);
	for (M = 0; M < n; M++){
			int k = 0;
			for (int i = 0; i < attr; ++i) {
				double d;
				if (fin >> d) {
					Data[M][k++] = d;
					for (int j = 1; j < power; ++j) {
						Data[M][k] = Data[M][k - 1] * d;
						k++;
					}
				}
				else {
					fin.clear();
					char c;
					do { fin >> c; } while (c != '?');
					for (int j = 0; j < power; ++j) {
						Data[M][k++] = INFINITE;
					}
				}
			}
			fin >> Y[M];
	}
	fin.close();
	N = M * 7 / 10;
	normalize(ATTRIBUTION);

	double theta[ATTRIBUTION_MAX];
	srand(time(NULL));
	for (int i = 0; i < ATTRIBUTION+1; ++i) theta[i] = (rand() / 16384.0 - 1.0)/100.0;
	
	double new_c, c = cost(theta);
	for (int i = 0; i < iter; ++i) {
		GradientDescent(theta);
		new_c = cost(theta);
		if (fabs(new_c - c) < e) {
			std::cout << "Convergence " << i << std::endl;
			break;
		}
		c = new_c;
		if (i % 100 == 0) {
			for (int i = 0; i < ATTRIBUTION + 1; ++i)  std::cout << theta[i] << ' ';
			std::cout << c << std::endl;
		}
	}
	/*
	for (int i = 0; i < M; ++i) {
		std::cout << H(theta, i) << std::endl;
	}*/

	for (int i = 0; i < ATTRIBUTION+1; ++i)  std::cout << theta[i] << '\t'; std::cout << c << std::endl;
	std::ofstream log_out("log.txt", std::ios::app);
	double sum = 0;
	double ymin = INFINITE, ymax = -INFINITE;
	double avg = 0;
	for (int i = N; i < M; ++i) {
		double d = H(theta, i)*y_s + y_avg - Y[i];
		sum += d*d;
		if (Y[i] < ymin) ymin = Y[i];
		if (Y[i] > ymax) ymax = Y[i];
		avg += Y[i];
	}
	avg /= (M - N);
	sum /= (M - N);
	sum = sqrt(sum);
	log_out << argv[1] << ' ' << sum << ' ' << (sum / (ymax - ymin)) << ' ' << (sum / avg) << std::endl;
	system("pause");
	return 0;
}