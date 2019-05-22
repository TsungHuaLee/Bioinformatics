/*單一比對， 沒有空格*/
//g++ align_xsimd.cpp  -march=native -I./include -std=c++11
#include <iostream>
#include <cstddef>
#include <vector>
#include "xsimd/xsimd.hpp"

using namespace std;

using vector_type = std::vector<int, xsimd::aligned_allocator<int, XSIMD_DEFAULT_ALIGNMENT>>;

void testBoolBatch(const vector_type& input1, const vector_type& input2, vector_type& res)
{
	xsimd::simd_type<int> match(13), mismatch(5);
	size_t size = input1.size();
	constexpr size_t simd_size = xsimd::simd_type<int>::size;
	size_t vec_size = size - size % simd_size;

	for(std::size_t i = 0; i < vec_size; i += simd_size)
	{
		auto avec = xsimd::load_aligned(&input1[i]);
		auto bvec = xsimd::load_aligned(&input2[i]);
		auto bool_batch =  (xsimd::simd_type<int>)(avec == bvec);;
		auto result =(bool_batch & match);
		result = result - mismatch;
		result.store_aligned(&res[i]);
	}

	for(std::size_t i = vec_size; i < size; ++i)
	{
		res[i] = (input1[i] == input2[i]) *  13 - 5;
	}
}

int main(){
	string A, B;
	cin >> A >> B;
	vector_type input1(A.begin(), A.end());
	vector_type input2(B.begin(), B.end());
	vector_type res(A.size());
	testBoolBatch(input1, input2, res);

  for(int i=0; i<A.size(); i++)
		printf("%d ", res[i]);

    return 0;
}
