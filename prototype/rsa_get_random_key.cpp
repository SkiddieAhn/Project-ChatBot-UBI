#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> 

/*
 * RSA 랜덤 키 생성 프로그램
 *
 * n값은 소수를 정해서 구함 ex) 100~300의 소수를 구하고 곱셈을 조합해서 n구함 -> 666개
 * e값 랜덤 (700가지 이상의 경우의 수)
*/

// key(n,e,d) 리스트 구조체 
typedef struct key_list {
	int* array;
	int depth; // 크기와 인덱스 역할 둘 다 
}key_list;
key_list* n, *e, *d;
int num; // 수치를 세는데 필요한 변수 (초기화하며 자주 쓰임)

// 나눗셈 연산 
int mod(int a, int b) {
	if (a > 0)
		return a % b; // a-b*(a/b)
	else {
		return a - b * -((-a / b) + 1);
	}
}

// 최대공약수 연산
int gcd(int a, int b) {
	long long max, min;
	if (a >= b) {
		max = a;
		min = b;
	}
	else {
		max = b;
		min = a;
	}
	while (min != 0) {
		int tmp = min;
		min = mod(max, min);
		max = tmp;
	}
	return max;
}

// RSA 암호화 C = M^e mod n
int make_C(int M, int e, int n) {
	unsigned long long C = 1; // 곱하기 하면서 오버플로우 될 수 있으므로 크게 설정
	for (int i = 0; i < e; i++) {
		C *= M;
		C %= n;
	}
	return C; // 최종 나머지 값은 int형으로 해결 가능
}

// RSA 복호화 M = C^d mod n
int make_M(int C, int d, int n) {
	unsigned long long M = 1; // 곱하기 하면서 오버플로우 될 수 있으므로 크게 설정
	for (int i = 0; i < d; i++) {
		M *= C;
		M %= n;
	}
	return M; // 최종 나머지 값은 int형으로 해결 가능
}

// 키 리스트 정의 
void key_init() {
	n = (key_list*)malloc(sizeof(key_list)); // 공개키 n
	n->array = (int*)calloc(1, sizeof(int));
	n->depth = 0;

	e = (key_list*)malloc(sizeof(key_list)); // 공개키 n
	e->array = (int*)calloc(1, sizeof(int));
	e->depth = 0;

	d = (key_list*)malloc(sizeof(key_list)); // 개인키 d
	d->array = (int*)calloc(1, sizeof(int));
	d->depth = 0;
}

int main(void) {

	// 키 리스트 초기화 
	key_init();

	// 100이상 300이하의 소수 찾고 저장하는 알고리즘
	int* store = (int*)malloc(sizeof(int) * 250); // 소수 저장 
	int length = 0; // store의 idx이자 크기 역할 

	for (int i = 100; i <= 350; i++) {
		for (int j = 2; j <= i; j++) { // i가 판별하는 수, j가 1씩 커지는 나눠보는 수 일 때 j가 i보다 작으면서 계속 나눠지지 않는다면 소수 (단 j는 1이 아님)
			if (j != i && i % j == 0)
				break;
			else if (j == i) {
				store[length++] = i;
			}
		}
	}

	// 저장한 소수간의 곱셈으로 n리스트 구축  <약 2500개의 n값 가짐> (10403~245009)
	// n은 두 소수(p,q)의 곱 <p와 q의 조합>
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			n->array[n->depth++] = store[i] * store[j];
			n->array = (int*)realloc(n->array, sizeof(int) * ((n->depth) + 1)); //n->depth가 현재 크기인 상태, 공간을 1늘림 
		}
	}

	// n리스트로 e리스트 구축
	// e는 (p-1)*(q-1)의 서로소, 1 < e < n, n의 범위:(10403~245009)
	srand(time(NULL)); // 난수 발생 초기화 
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			int tmp = (store[i] - 1) * (store[j] - 1);
			int compare = (rand() % 701) + 300; // 랜덤 상태 서로소 찾기 (300~1000)
			while (gcd(tmp,compare) != 1)
				compare++;
			e->array[e->depth++] = compare;
			e->array = (int*)realloc(e->array, sizeof(int) * ((e->depth) + 1)); //e->depth가 현재 크기인 상태, 공간을 1늘림 
		}
	}

	// store,e리스트로 d리스트 구축 
	// e mod (p-1)(q-1)의 역 구하기 -> e*d mod (p-1)(q-1)=1일 때 d가 역
	num = 0;
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			int P = store[i];
			int Q = store[j];
			int N = n->array[num];
			int E = e->array[num];
			int D = 1;

			// 무작위 대입 연산 
			// 300,000번 정도 시행함 
			while (mod(E * D, (P - 1) * (Q - 1)) != 1 && D < 300000)
				D++;

			if (D < 300000) {
				d->array[d->depth++] = D;
				d->array = (int*)realloc(d->array, sizeof(int) * ((d->depth) + 1)); //d->depth가 현재 크기인 상태, 공간을 1늘림 
			}
			else { // D==300,000 -> 개인키(d)가 너무 큼 
				printf("%d:%d\n",num,D);
				d->array[d->depth++] = -1; // 오류 수 대입 
				d->array = (int*)realloc(d->array, sizeof(int) * ((d->depth) + 1)); //d->depth가 현재 크기인 상태, 공간을 1늘림 
			}
			num++;
		}
		/* RSA 계산(공개키(n,e)에 맞는 각각의 d값 계산) 진행 상태 알림 */

		float volume = (float)n->depth; // key(n,e,d)의 개수

		if ((float)num / volume >= 0.05 && (float)num / volume < 0.3) { // 10% 진행 상황
			system("cls");
			printf("RSA Engine Loading (5%%)");
		}
		else if ((float)num / volume >= 0.3 && (float)num / volume < 0.6) { // 30% 진행 상황
			system("cls");
			printf("RSA Engine Loading (30%%)");
		}
		else if ((float)num / volume >= 0.6 && (float)num / volume < 0.9) { // 60% 진행 상황
			system("cls");
			printf("RSA Engine Loading (60%%)");
		}
		else if ((float)num / volume >= 0.9) { // 90% 진행 상황
			system("cls");
			printf("RSA Engine Loading (90%%)");
		}
	}
	system("cls");


	// RSA 암호화 & 복호화 테스트 
	// n,e,d 리스트 이용!
	num = 0;
	int congruent, incongruent; // 적합한 키, 부적합한 키 개수 세기 
	congruent = incongruent = 0;

	printf("\n<< Key List >>\n");
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			if (d->array[num] != -1) { // 오류 값 제외 출력
				printf("\nNo.%d / p:%d, q:%d, n:%d, Φ(n):%d, e:%d, d:%d", num + 1, store[i], store[j], n->array[num], (store[i] - 1) * (store[j] - 1), e->array[num], d->array[num]);
				int O = 10000; // Original Message (원래 숫자)
				int C = make_C(O, e->array[num], n->array[num]); // 암호화 값
				int M = make_M(C, d->array[num], n->array[num]); // 복호화 값
				printf(" - M:%d -> C:%d -> M:%d", O, C, M);
				congruent++;
			}
			else {
				printf("\nNo.%d / Incongruent, e:%d, n:%d", num + 1, e->array[num], n->array[num]); // 개인키(d)가 너무 큼 
				incongruent++;
			}
			num++;
		}
	}

	// 적합한 키, 부적합한 키 개수 출력 
	printf("\n\nCongruent Keys:%d", congruent);
	printf("\nIncongruent Keys:%d\n", incongruent);

	// 랜덤키,테스트 값 생성
	int rand_key_idx, rand_n, rand_e, rand_d, test_C, test_M;
	rand_key_idx = rand() % (n->depth);
	while (d->array[rand_key_idx] == -1) // 부적합한 키면 다시 랜덤화
		rand_key_idx = rand() % (n->depth);
	rand_n = n->array[rand_key_idx];
	rand_e = e->array[rand_key_idx];
	rand_d = d->array[rand_key_idx];
	test_C = make_C(10000, rand_e, rand_n); // 랜덤 키 테스트 Original Number: 10000
	test_M = make_M(test_C, rand_d, rand_n);
	printf("\nRandom Key -> [No.%d] Public(n,e):%d,%d, Private(d):%d", rand_key_idx + 1, rand_n, rand_e, rand_d);
	printf("\nRandom Key Test -> [Original] %d, [Encoded] %d, [Decoded] %d\n", 10000, test_C, test_M);

	free(store); // 저장한 소수 배열 버리기
	free(n); // 저장한 n리스트 버리기
	free(e); // 저장한 e리스트 버리기
	free(d); // 저장한 d리스트 버리기 
}
