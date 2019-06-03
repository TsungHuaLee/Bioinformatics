//g++ -O3 -fopt-info -optall -std=c++11 final.cpp -march=native -fopenmp
#include <bits/stdc++.h>
#include <omp.h>
#include <cuda_runtime.h>
using namespace std;
vector<string> str;
#define DP_TYPE int

int lcs_len(const char *A, int na, const char *B, int nb) {
    if (na > nb)
        swap(A, B), swap(na, nb);

    int **dp = (int**)malloc(sizeof(int*)*3);
    for (int i = 0; i < 3; i++)
    {
        dp[i] = (int*)malloc(sizeof(int)*(na + nb + 2));
        memset(dp[i], 0, sizeof(int)*(nb + na + 2));
    }
    int last_l = 0, last_r = 0;

    dp[0][0]=0;
    dp[1][0]=dp[1][1]=-3;

    int band = na * 0.03;

    for (int i = 0; i < na+nb-1; i++) {
        // int l = max(0, i-na+1), r = min(i, nb-1);
        int l = (i <= band)? 0 : ((i-band)/2);
    	int r = l+band*2-1;
		if(r>i) r = i;
    	if(r>=nb-1) r = nb-1;

		dp[2][0]=(i+2)*(-3);
		dp[2][r+2]=(i+2)*(-3);

        #pragma omp simd
        for (int j = l; j <= r; j++) {
            int x = i-j, y = j;
            int diagnal = dp[0][j] + (A[x]==B[y]?8:-5);
			int maxupleft = (dp[1][j] > dp[1][j+1]? dp[1][j] : dp[1][j+1]) - 3;
            dp[2][j+1] = maxupleft > diagnal ? maxupleft:diagnal;
        }

        int *swapTemp = dp[0];
        dp[0] = dp[1];
        dp[1] = dp[2];
        dp[2] = swapTemp;
        last_l = l, last_r = r;
    }

    int re = dp[1][nb];
    for (int i = 0; i < 3; i++)
        free(dp[i]);
    free(dp);
    return re;
}

int main() {
    string temp;
    while (getline(cin, temp)) {
        str.push_back(temp);
    }

    int ansMax = 0, ansMin = 1e9;
    int size = str.size();

    for(int i = 0; i < size; i++)
    {
        #pragma omp parallel for
        for(int j = i + 1; j < size; j++)
        {
            int len = lcs_len(str[i].c_str(), str[i].length(), str[j].c_str(), str[j].length());
            #pragma omp critical
            {
                ansMax = (len>ansMax)?len:ansMax;
                ansMin = (len<ansMin)?len:ansMin;
                //printf("%d %d: %d %d %d\n", i, j, len, ansMax, ansMin);
            }
        }
    }
    printf("max = %d, min =  %d\n", ansMax, ansMin);
    return 0;
}
