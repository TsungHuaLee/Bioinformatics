#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <omp.h>

using namespace std;
const int MAXN = 500000;
const int Match = 8, Mis = -5, Open = -4, Gap = -3;
const int band = 301;
#define DP_TYPE int

int lcs_len(const char *A, int na, const char *B, int nb) {
	if (na > nb)
		swap(A, B), swap(na, nb);

    /*initialize*/
    static DP_TYPE dp[3][MAXN], dele[3][MAXN], insert[3][MAXN];

	for (int i = 0; i < 3; i++)
    {
		memset(dp[i], 0, sizeof(DP_TYPE)*(nb+na+2));
        memset(dele[i], 0, sizeof(DP_TYPE)*(nb+na+2));
        memset(insert[i], 0, sizeof(DP_TYPE)*(nb+na+2));
    }
    /* Alignment must initialize score
		dp[0] and dp[1] will be replace later, so only initialize at begin*/
	dp[0][0] = dele[0][0] = insert[0][0] = 0;
	dp[1][0] = dp[1][1] = Open + Gap;
    dele[1][0] = dele[1][1] = Open + Gap;
    insert[1][0] = insert[1][1] = Open + Gap;

	int last_l = 0, last_r = 0;

	omp_set_num_threads(8);

    for (int i = 0; i < na+nb-1; i++)
    {
		//int l = max(0, i-na+1), r = min(i, nb-1);
		int l = (i <= band)? 0 : ((i - band) / 2);
    	int r = l + band * 2 - 1;
		if(r > i) r = i;
    	if(r >= nb - 1) r = nb - 1;

        dp[2][0] = dp[2][r+2] = Open + Gap * (i+2);
		dele[2][0]  = dele[2][r+2] = Open + Gap * (i+2);
        insert[2][0] = insert[2][r+2] = Open + Gap * (i+2);

		#pragma omp parallel for schedule(static) firstprivate(na, A, B)
		for (int j = l; j <= r; j++)
		{
			int x = i - j;
			int y = j;
			int diag;

			dele[2][j + 1] = max(dele[1][j] + Gap, dp[1][j] + Gap + Open);
			insert[2][j + 1] = max(insert[1][j + 1] + Gap, dp[1][j + 1] + Gap + Open);
			diag = (A[x]==B[y]) ? dp[0][j] + Match : dp[0][j] + Mis;

            dp[2][j + 1] = max(diag, max(dele[2][j+1], insert[2][j + 1]));

		}

		memcpy(dp[0] + last_l, dp[1] + last_l, sizeof(DP_TYPE) * (last_r - last_l + 3));
	    memcpy(dp[1] + l, dp[2] + l, sizeof(DP_TYPE) * (r - l + 3));
        memcpy(dele[0] + last_l, dele[1] + last_l, sizeof(DP_TYPE) * (last_r - last_l + 3));
	    memcpy(dele[1] + l, dele[2] + l, sizeof(DP_TYPE) * (r - l + 3));
        memcpy(insert[0] + last_l, insert[1] + last_l, sizeof(DP_TYPE) * (last_r - last_l + 3));
        memcpy(insert[1] + l, insert[2] + l, sizeof(DP_TYPE) * (r - l + 3));
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
		printf("ans = %d\n", len);
	}
	return 0;
}
