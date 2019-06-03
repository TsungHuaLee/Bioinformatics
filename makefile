final: final.cpp
	g++ -O3 -fopt-info-optall -std=c++11 final.cpp -march=native -fopenmp -o final

test_final_10: final
	time ./final < T10.dat

test_final_100: final
	time ./final < T100.dat
	
test_final_1000: final
	time ./final < T1000.dat
test: test.cpp
	g++ -O3 -fopt-info-optall -std=c++11 test.cpp -march=native -fopenmp -o test

INC=-I/usr/include/cuda/
NVCC=/usr/bin//nvcc
NVCC_OPT=-std=c++11

gpu_example: gpu_example.cpp
	$(NVCC) $(NVCC_OPT) $(INC) gpu_example.cpp -o gpu_example
