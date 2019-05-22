//g++ -O3 -fopt-info -std=c++11 -march=native alignompSIMD.cpp
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <omp.h>

using namespace std;
const int MAXN = 65536;
static char A[MAXN], B[MAXN];
#define DP_TYPE int
int lcs_len(const char *A, int na, const char *B, int nb, int dpf[]) {
    if (na > nb)
        swap(A, B), swap(na, nb);
    static DP_TYPE dp[3][MAXN<<1];
    for (int i = 0; i < 3; i++)
        memset(dp[i], 0, sizeof(DP_TYPE)*(nb+na+2));
    memset(dpf, 0, sizeof(DP_TYPE)*(nb+1));

	// omp_set_num_threads(4);
    int last_l = 0, last_r = 0;

    dp[0][0]=0;
    dp[1][0]=dp[1][1]=-3;

    for (int i = 0; i < na+nb-1; i++) {
        int l = max(0, i-na+1), r = min(i, nb-1);

		dp[2][0]=(i+2)*(-3);
		dp[2][r+2]=(i+2)*(-3);

		#pragma omp simd
        for (int j = l; j <= r; j++) {
            int x = i-j, y = j;
            int diagnal = dp[0][j] + (A[x]==B[y]?8:-5);
			int maxupleft = (dp[1][j] > dp[1][j+1]? dp[1][j] : dp[1][j+1]) - 3;
            dp[2][j+1] = maxupleft > diagnal ? maxupleft:diagnal;
        }

        if (i-l == na-1)
            dpf[l+1] = dp[2][l+1];

        memcpy(dp[0]+last_l, dp[1]+last_l, sizeof(DP_TYPE)*(last_r-last_l+3));
        memcpy(dp[1]+l, dp[2]+l, sizeof(DP_TYPE)*(r-l+3));
        last_l = l, last_r = r;
    }
    return dpf[nb];
}

int main() {
    int dp[MAXN];
    while (scanf("%s %s", A, B) == 2) {
        string a = A, b = B;
        int len = lcs_len(a.c_str(), strlen(A), b.c_str(), strlen(B), dp);
        printf("%d\n", len);
    }
    return 0;
}
