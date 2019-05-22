//g++ -O3 -fopt-info -std=c++11 -march=native –I./include align_xsimd.cpp
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include "xsimd/xsimd.hpp"

using namespace std;
using vector_type = std::vector<int, xsimd::aligned_allocator<int, XSIMD_DEFAULT_ALIGNMENT>>;
const int MAXN = 500001;
constexpr int Match = 8, MisMatch = -5, Gap = -3;

int Align(const string A, int na, const string B, int nb) {

	const xsimd::simd_type<int> simdMatch(8), simdMisMatch(-5), simdGap(-3);
	vector_type inputA(A.begin(), A.end());
	/*dp是斜角計算，右上到左下，load_unaligned 需要反轉A陣列*/
	reverse(inputA.begin(), inputA.end());
	vector_type inputB(B.begin(), B.end());
	/* linear space lcs */
	vector<vector_type> dp(3, vector_type(MAXN));
	/* Alignment must initialize score
		dp[0] and dp[1] will be replace later, so only at begin*/
	dp[0][0] = 0;
	dp[1][0] = Gap; dp[1][1] = Gap;
	int last_l = 0, last_r = 0;

	for(int i = 0; i < na+nb-1; i++)
	{
		int l = max(0, i-na+1), r = min(i, nb-1);

		dp[2][0] = -3 * (i+2);
		dp[2][r+2] = -3 * (i+2);
		size_t range = r-l+1;
		constexpr size_t simd_size = xsimd::simd_type<int>::size;
		size_t vec_size = range - range % simd_size;

		for(std::size_t j = l; j < l + vec_size; j += simd_size)
		{
			int x = A.size() - (i - j) - 1, y = j;
			auto avec = xsimd::load_unaligned(&inputA[x]);
			auto bvec = xsimd::load_unaligned(&inputB[y]);
			auto dp0j = xsimd::load_unaligned(&dp[0][j]);
			auto diag = xsimd::select(avec == bvec, dp0j + simdMatch, dp0j + simdMisMatch);
			auto up = xsimd::load_unaligned(&dp[1][j]);
			auto left = xsimd::load_unaligned(&dp[1][j+1]);
			auto maxUpLeft = xsimd::select(up > left, up + simdGap, left + simdGap);
			auto maxDp = xsimd::select(diag > maxUpLeft, diag, maxUpLeft);
			maxDp.store_unaligned(&dp[2][j+1]);
		}

		for(std::size_t j = l + vec_size; j <= r; ++j)
		{
			int x = A.size() - (i - j) - 1, y = j;
			int diag = (inputA[x]==inputB[y])?(dp[0][j] + Match) : (dp[0][j] + MisMatch);
			dp[2][j+1] = max(max(diag, dp[1][j] + Gap), dp[1][j+1] + Gap);
		}

		copy(dp[1].begin() + last_l, dp[1].begin() + last_r + 3, dp[0].begin() + last_l);
		copy(dp[2].begin() + l, dp[2].begin() + r + 3, dp[1].begin() + l);
		last_l = l, last_r = r;
	}

	return dp[2][nb];
}
int main() {
	int dp[MAXN];
	char A[MAXN], B[MAXN];
	while (scanf("%s %s", A, B) == 2) {
		string input1 = A, input2 = B;
		if(input1.length() > input2.length())
			input1.swap(input2);
		int len = Align(input1, input1.length(), input2, input2.length());
		printf("ans = %d\n", len);
	}

	return 0;
}
