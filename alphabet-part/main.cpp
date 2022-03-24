#include <cmath>
#include <iostream>
#include <vector>

#define LOG2E 1.4426950408889634

double redundancy(std::vector<long long> m) {
	long long n = 0;
	double M = 0;
	for (int i = 0; i < m.size(); i++) {
		n += m[i];
		for (long long l = 1; l <= m[i]; l++) {
			double r = l * log((double)m[i] / l) / (double)(n + l);
			if (r > M) {
				M = r;
			}
		}
	}
	return M;
}

double f(long long m, long long n) {
	double r = 0;
	double tmp = 0;
	long long l = 1;
	if (m > 5) {
		double gleft = 1;
		double gright = m;
		while (gright - gleft > 1) {
			double gmid = (gright + gleft) / 2;
			if ((n * log(m / gmid) - (n + gmid)) <= 0) {
				gright = gmid;
			} else {
				gleft = gmid;
			}
		}
		l = floor(gleft - 1);
		if (l < 1) {
			l = 1;
		}
	}

	while (l <= m && tmp <= r) {
		tmp = (l * log((double)m / l)) / (double)(n + l);
		if (tmp > r) {
			r = tmp;
		}
		l++;
	}
	return r * LOG2E;
}

void Arithmetic(double delta, long long len) {
	std::vector<long long> m(1);
	m[0] = 1;
	long long n = 1;
	long long i = 0;
	double r = 0.0;
	while (n <= len) {
		r = 0;
		m.push_back(m[i]);
		long long left = m[i];
		long long right = 4 * m[i];
		i++;
		while (right - left > 1) {
			long long mid = (left + right) / 2;
			if (f(mid, n) > delta) {
				right = mid;
			} else {
				left = mid;
			}
		}
		m[i] = left;
		n += m[i];
		printf("m[%lld]=%lld   n[%lld]=%lld\n", (i + 1), m[i], (i + 1), n);
	}
	printf("s: %lld\n", m.size());
	printf("redundancy: %2f\n", redundancy(m));
}

void Huffman(double delta, long long len) {
	std::vector<long long> m(1);
	m[0] = 1;
	long long n = 1;
	long long i = 0;
	double r = 0.0;

	while (n <= len) {
		r = 0;
		m.push_back(m[i]);
		i++;
		while (r < delta) {
			m[i] = 2 * m[i];
			r = f(m[i], n);
		}
		m[i] = m[i] / 2;

		n += m[i];
		printf("m[%lld]=%lld   n[%lld]=%lld\n", (i + 1), m[i], (i + 1), n);
	}
	printf("s: %lld\n", m.size());
	printf("redundancy: %2f\n", redundancy(m));
}

void calcul(uint8_t CodeFlag, double delta, long long len) {
	printf("delta: %f\n", delta);
	printf("len: %lld\n", len + 1);
	printf("m[1]=1   n[1]=1\n");
	if (CodeFlag) {
		Arithmetic(delta, len);
	} else {
		Huffman(delta, len);
	}
}
int main() {
	double delta = 0.08;
	long long len = 123456;
	calcul(1, delta, len - 1);
}