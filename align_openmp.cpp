/* g++ align_openmp.cpp -fopenmp */
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <omp.h>
using namespace std;
const int MAXN = 500001;
//static char A[MAXN], B[MAXN];
const int Match = 8, MisMatch = -5, Gap = -3;
#define DP_TYPE int
int lcs_len(const char *A, int na, const char *B, int nb) {
	/*shorter one place top, longer one place left*/
	if (na > nb)
		swap(A, B), swap(na, nb);
	/* linear space lcs */
	static DP_TYPE dp[3][MAXN<<1];
	for (int i = 0; i < 3; i++)
		memset(dp[i], 0, sizeof(DP_TYPE)*(nb+na+2));
    /* Alignment must initialize score
		dp[0] and dp[1] will be replace later, so only at begin*/
	dp[0][0] = Gap * 0;
	dp[1][0] = Gap * 1; dp[1][1] = Gap * 1;
	int last_l = 0, last_r = 0;
	omp_set_num_threads(8);
	for (int i = 0; i < na+nb-1; i++) {
		int l = max(0, i-na+1), r = min(i, nb-1);

		dp[2][0] = Gap * (i+2);
		dp[2][r+2] = Gap * (i+2);

		#pragma omp parallel for schedule(static) firstprivate(na, A, B)
		for (int j = l; j <= r; j++) {
			int x = i-j, y = j;
          	int diag = (A[x]==B[y])?(dp[0][j] + Match) : (dp[0][j] + MisMatch);
          	dp[2][j+1] = max(max(diag, dp[1][j] + Gap), dp[1][j+1] + Gap);
        }
		memcpy(dp[0] + last_l, dp[1] + last_l,
			sizeof(DP_TYPE) * (last_r - last_l + 3));
	  	memcpy(dp[1] + l, dp[2] + l, sizeof(DP_TYPE) * (r - l + 3));
		last_l = l, last_r = r;
	}
	return dp[2][nb];
}
int main() {
	int dp[MAXN];
	char A[MAXN], B[MAXN];
	while (scanf("%s %s", A, B) == 2) {
		string a = A, b = B;
		int len = lcs_len(a.c_str(), strlen(A), b.c_str(), strlen(B));
		printf("%d\n", len);
	}

	return 0;
}
