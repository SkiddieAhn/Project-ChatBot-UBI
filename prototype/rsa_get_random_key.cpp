#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> 

/*
 * RSA ���� Ű ���� ���α׷�
 *
 * n���� �Ҽ��� ���ؼ� ���� ex) 100~300�� �Ҽ��� ���ϰ� ������ �����ؼ� n���� -> 666��
 * e�� ���� (700���� �̻��� ����� ��)
*/

// key(n,e,d) ����Ʈ ����ü 
typedef struct key_list {
	int* array;
	int depth; // ũ��� �ε��� ���� �� �� 
}key_list;
key_list* n, *e, *d;
int num; // ��ġ�� ���µ� �ʿ��� ���� (�ʱ�ȭ�ϸ� ���� ����)

// ������ ���� 
int mod(int a, int b) {
	if (a > 0)
		return a % b; // a-b*(a/b)
	else {
		return a - b * -((-a / b) + 1);
	}
}

// �ִ����� ����
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

// RSA ��ȣȭ C = M^e mod n
int make_C(int M, int e, int n) {
	unsigned long long C = 1; // ���ϱ� �ϸ鼭 �����÷ο� �� �� �����Ƿ� ũ�� ����
	for (int i = 0; i < e; i++) {
		C *= M;
		C %= n;
	}
	return C; // ���� ������ ���� int������ �ذ� ����
}

// RSA ��ȣȭ M = C^d mod n
int make_M(int C, int d, int n) {
	unsigned long long M = 1; // ���ϱ� �ϸ鼭 �����÷ο� �� �� �����Ƿ� ũ�� ����
	for (int i = 0; i < d; i++) {
		M *= C;
		M %= n;
	}
	return M; // ���� ������ ���� int������ �ذ� ����
}

// Ű ����Ʈ ���� 
void key_init() {
	n = (key_list*)malloc(sizeof(key_list)); // ����Ű n
	n->array = (int*)calloc(1, sizeof(int));
	n->depth = 0;

	e = (key_list*)malloc(sizeof(key_list)); // ����Ű n
	e->array = (int*)calloc(1, sizeof(int));
	e->depth = 0;

	d = (key_list*)malloc(sizeof(key_list)); // ����Ű d
	d->array = (int*)calloc(1, sizeof(int));
	d->depth = 0;
}

int main(void) {

	// Ű ����Ʈ �ʱ�ȭ 
	key_init();

	// 100�̻� 300������ �Ҽ� ã�� �����ϴ� �˰���
	int* store = (int*)malloc(sizeof(int) * 250); // �Ҽ� ���� 
	int length = 0; // store�� idx���� ũ�� ���� 

	for (int i = 100; i <= 350; i++) {
		for (int j = 2; j <= i; j++) { // i�� �Ǻ��ϴ� ��, j�� 1�� Ŀ���� �������� �� �� �� j�� i���� �����鼭 ��� �������� �ʴ´ٸ� �Ҽ� (�� j�� 1�� �ƴ�)
			if (j != i && i % j == 0)
				break;
			else if (j == i) {
				store[length++] = i;
			}
		}
	}

	// ������ �Ҽ����� �������� n����Ʈ ����  <�� 2500���� n�� ����> (10403~245009)
	// n�� �� �Ҽ�(p,q)�� �� <p�� q�� ����>
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			n->array[n->depth++] = store[i] * store[j];
			n->array = (int*)realloc(n->array, sizeof(int) * ((n->depth) + 1)); //n->depth�� ���� ũ���� ����, ������ 1�ø� 
		}
	}

	// n����Ʈ�� e����Ʈ ����
	// e�� (p-1)*(q-1)�� ���μ�, 1 < e < n, n�� ����:(10403~245009)
	srand(time(NULL)); // ���� �߻� �ʱ�ȭ 
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			int tmp = (store[i] - 1) * (store[j] - 1);
			int compare = (rand() % 701) + 300; // ���� ���� ���μ� ã�� (300~1000)
			while (gcd(tmp,compare) != 1)
				compare++;
			e->array[e->depth++] = compare;
			e->array = (int*)realloc(e->array, sizeof(int) * ((e->depth) + 1)); //e->depth�� ���� ũ���� ����, ������ 1�ø� 
		}
	}

	// store,e����Ʈ�� d����Ʈ ���� 
	// e mod (p-1)(q-1)�� �� ���ϱ� -> e*d mod (p-1)(q-1)=1�� �� d�� ��
	num = 0;
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			int P = store[i];
			int Q = store[j];
			int N = n->array[num];
			int E = e->array[num];
			int D = 1;

			// ������ ���� ���� 
			// 300,000�� ���� ������ 
			while (mod(E * D, (P - 1) * (Q - 1)) != 1 && D < 300000)
				D++;

			if (D < 300000) {
				d->array[d->depth++] = D;
				d->array = (int*)realloc(d->array, sizeof(int) * ((d->depth) + 1)); //d->depth�� ���� ũ���� ����, ������ 1�ø� 
			}
			else { // D==300,000 -> ����Ű(d)�� �ʹ� ŭ 
				printf("%d:%d\n",num,D);
				d->array[d->depth++] = -1; // ���� �� ���� 
				d->array = (int*)realloc(d->array, sizeof(int) * ((d->depth) + 1)); //d->depth�� ���� ũ���� ����, ������ 1�ø� 
			}
			num++;
		}
		/* RSA ���(����Ű(n,e)�� �´� ������ d�� ���) ���� ���� �˸� */

		float volume = (float)n->depth; // key(n,e,d)�� ����

		if ((float)num / volume >= 0.05 && (float)num / volume < 0.3) { // 10% ���� ��Ȳ
			system("cls");
			printf("RSA Engine Loading (5%%)");
		}
		else if ((float)num / volume >= 0.3 && (float)num / volume < 0.6) { // 30% ���� ��Ȳ
			system("cls");
			printf("RSA Engine Loading (30%%)");
		}
		else if ((float)num / volume >= 0.6 && (float)num / volume < 0.9) { // 60% ���� ��Ȳ
			system("cls");
			printf("RSA Engine Loading (60%%)");
		}
		else if ((float)num / volume >= 0.9) { // 90% ���� ��Ȳ
			system("cls");
			printf("RSA Engine Loading (90%%)");
		}
	}
	system("cls");


	// RSA ��ȣȭ & ��ȣȭ �׽�Ʈ 
	// n,e,d ����Ʈ �̿�!
	num = 0;
	int congruent, incongruent; // ������ Ű, �������� Ű ���� ���� 
	congruent = incongruent = 0;

	printf("\n<< Key List >>\n");
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			if (d->array[num] != -1) { // ���� �� ���� ���
				printf("\nNo.%d / p:%d, q:%d, n:%d, ��(n):%d, e:%d, d:%d", num + 1, store[i], store[j], n->array[num], (store[i] - 1) * (store[j] - 1), e->array[num], d->array[num]);
				int O = 10000; // Original Message (���� ����)
				int C = make_C(O, e->array[num], n->array[num]); // ��ȣȭ ��
				int M = make_M(C, d->array[num], n->array[num]); // ��ȣȭ ��
				printf(" - M:%d -> C:%d -> M:%d", O, C, M);
				congruent++;
			}
			else {
				printf("\nNo.%d / Incongruent, e:%d, n:%d", num + 1, e->array[num], n->array[num]); // ����Ű(d)�� �ʹ� ŭ 
				incongruent++;
			}
			num++;
		}
	}

	// ������ Ű, �������� Ű ���� ��� 
	printf("\n\nCongruent Keys:%d", congruent);
	printf("\nIncongruent Keys:%d\n", incongruent);

	// ����Ű,�׽�Ʈ �� ����
	int rand_key_idx, rand_n, rand_e, rand_d, test_C, test_M;
	rand_key_idx = rand() % (n->depth);
	while (d->array[rand_key_idx] == -1) // �������� Ű�� �ٽ� ����ȭ
		rand_key_idx = rand() % (n->depth);
	rand_n = n->array[rand_key_idx];
	rand_e = e->array[rand_key_idx];
	rand_d = d->array[rand_key_idx];
	test_C = make_C(10000, rand_e, rand_n); // ���� Ű �׽�Ʈ Original Number: 10000
	test_M = make_M(test_C, rand_d, rand_n);
	printf("\nRandom Key -> [No.%d] Public(n,e):%d,%d, Private(d):%d", rand_key_idx + 1, rand_n, rand_e, rand_d);
	printf("\nRandom Key Test -> [Original] %d, [Encoded] %d, [Decoded] %d\n", 10000, test_C, test_M);

	free(store); // ������ �Ҽ� �迭 ������
	free(n); // ������ n����Ʈ ������
	free(e); // ������ e����Ʈ ������
	free(d); // ������ d����Ʈ ������ 
}
