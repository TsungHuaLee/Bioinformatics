#include <cstring>
#include <stdio.h>

const int MAX_DIM = 800;
const int match = 8;
const int mis = -4;
const int gap = -2;

#define max(a, b) ((a)>(b)?a:b)

int main() {
	
	int nt;
	scanf(" %d",&nt);
	
	while(nt--){
		char line1[MAX_DIM], line2[MAX_DIM];
		scanf(" %s", line1);
		scanf(" %s", line2);
		
		int m = strlen(line1);
		int n = strlen(line2);
		
		int M[MAX_DIM][MAX_DIM];
		
		for (int i=0; i <= m; i++) M[i][0] = gap*i;
		for (int j=0; j <= n; j++) M[0][j] = gap*j;
		
		for (int i=1; i <= m; i++) {
			for (int j=1; j <= n; j++) {
				int Diagnal = M[i-1][j-1]+ (line1[i-1] == line2[j-1] ? match : mis);
				M[i][j] = max(max(Diagnal, M[i-1][j]+gap), M[i][j-1]+gap);
			}
		}
		
		printf("%d\n", M[m][n]);
	}
}
