#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 2000
#define n 16637 // ���� Ű < (0 <= M <=n-1)�� �����ؼ� ����, ���� M�� �ִ밪�� 7575 >
#define e 14453 // ���� Ű
#define d 17 // ���� Ű 

/*
- ����Ʈ ������ �Ұ� -

* code = RSA ��ó�� �ڵ� ���� ���� (ex) a�� 1��, b�� 2��)
* bundle = �� ���� ���� ���� (ex) 102,203,1020,...)
* encode = ��ȣ ���� ���� ���� (ex) 2120,3366, ...)
* decode = ��ȣȭ ����(�� ����) ���� ���� (ex) 102,203,1020,...)

*/

typedef struct list { // ����Ʈ ����ü 
    int size;
    int* array;
}list;
list* code, * bundle, * encode, * decode; // ���⼭ �������� ������ '��۸� ������'�� �Ǿ� ���� 

/*
- �Լ� �Ұ� (�۾� ���� �������) -

* init() = RSA �۾��� �ʿ��� ����Ʈ ����  (code,bundle,encode,decode ����Ʈ)
* get_symbol(char* tmp) = string�� ���ڿ��� �������� ������ ���� (ex) ab-> 'a','b')
* make_Code(char* string,list* code) = ���� �������� ��ó�� �ڵ� ���� ���� <code> (ex) a->1, b->2)
* get_integer(list* code) = ��ó�� �ڵ� 2���� �� ���� ����� (ex) 1,2 -> 102)
* make_Bundle(list * code,list* bundle) = �� ���� ���� ���� <bundle> (ex) 102,1021,1200,..)
* make_C(int M) = RSA ��ȣȭ <�� ���� -> ��ȣ ���ڷ� ����> (ex) 102 -> 3366)
* make_Encode(list *bundle,list *encode) = ��ȣ ���� ���� ���� <encode> (ex) 3366,3450,9800,..)
* make_M(int C) = RSA ��ȣȭ <��ȣ ���� -> �� ���ڷ� ����> (ex) 3366 -> 102)
* make_Decode(list* encode, list* decode) = ��ȣ ����(�� ����) ���� ���� <decode> (ex) 102,1021,1200,..)
* get_Code(list* decode) = ��ȣ ����(�� ����) �����ؼ�  ��ó�� �ڵ� ��ȯ (ex) 102 -> 1�� 2 ��ȯ)
* decode_Effect(list* decode) = RSA ��ó�� �ڵ带 �ƽ�Ű�ڵ�� ��ȯ �� ���� ���
��(��ó�� �ڵ带 �� �������� �ݺ�) (ex) 1->97->'a'���, 2->98->'b'���, ...)

*/

void init();
char get_symbol(char* tmp);
void make_Code(char* string, list* code);
int get_integer(list* code);
void make_Bundle(list* code, list* bundle);
int make_C(int M);
void make_Encode(list* bundle, list* encode);
int make_M(int C);
void make_Decode(list* encode, list* decode);
int* get_Code(list* decode);
void decode_Effect(list* decode);


/* �Ʒ��� (���ڿ� ó��)�� �ʿ��� */

 /* ------------------------------------------------------------------------ */
// �����ڿ� ����
char string[MAX] = "";
// Ư������ �迭 
char sign[14] = { ' ',',','.',';',':','(',')','!','?','%','"',39,'-' }; // 39 = `
// Ư�����ڿ��� �Ÿ� 
int sign_distance[14] = { 63 - 32,64 - 44,65 - 46,66 - 59,67 - 58,68 - 40,69 - 41,70 - 33,71 - 63,72 - 37,73 - 34,74 - 39,75 - 45 };
/* ------------------------------------------------------------------------ */


// ���� �Լ� 
int main() {

    init(); // RSA �۾��� �ʿ��� ����Ʈ ����

    printf("�� ��ȣȭ�� ������ ���� �ּ���:\n\n");
    gets(string);// �� ���ڿ� �Է��ؼ� ������ �迭(string)�� ����(ex)string�迭�� 'abdcdi' ����)

    /* ��ȣȭ ���� */
    make_Code(string, code); // ��ó�� �ڵ� ���� ����
    make_Bundle(code, bundle); // �� ���� ���� ����
    make_Encode(bundle, encode); // ��ȣ ���� ���� ���� 

    /* ��ȣȭ ���� */
    make_Decode(encode, decode); // ��ȣ ����(�� ����) ���� ����
    printf("\n�� ������ ���� ��ȣȭ�Ǿ����ϴ�:\n\n");
    decode_Effect(decode); // ��ȣ ���� �̿� -> ���� ��±��� 
}

// RSA �۾��� �ʿ��� ����Ʈ ���� 
void init() {

    // RSA ��ó�� �ڵ� ���� ����Ʈ 
    code = (list*)malloc(sizeof(list));
    code->size = 0;
    code->array = (int*)malloc(sizeof(int) * MAX);

    // �� ���� ����(���� ����) ���� ����Ʈ 
    bundle = (list*)malloc(sizeof(list));
    bundle->size = 0;
    bundle->array = (int*)malloc(sizeof(int) * MAX);

    // ��ȣȭ �ڵ� ���� ����Ʈ
    encode = (list*)malloc(sizeof(list));
    encode->size = 0;
    encode->array = (int*)malloc(sizeof(int) * MAX);

    // ��ȣȭ �ڵ� ���� ����Ʈ
    decode = (list*)malloc(sizeof(list));
    decode->size = 0;
    decode->array = (int*)malloc(sizeof(int) * MAX);

}

// string�� ���ڿ��� �������� ������ ���� (ex) ab-> 'a','b') 
char get_symbol(char* tmp) {
    static int j = 0;
    return tmp[j++];
}

// ���� �������� RSA ��ó�� �ڵ� ���� (ex) a->1, b->2)
void make_Code(char* string, list* code) {

    char piece; // ���� ���� 

    // RSA ��ó�� �ڵ� ���� 
    while ((piece = get_symbol(string)) != 0) {
        // ���� �ҹ��� (a~z)
        // RSA ��ó�� �� -> 1~26���� ���� 
        if (piece >= 97 && piece <= 122) {
            code->array[code->size] = piece - 96;
            code->size++;
        }

        // ���� �빮��(A~Z)
        // RSA ��ó�� �� -> 27~52���� ���� 
        else if (piece >= 65 && piece <= 90) {
            code->array[code->size] = piece - 38;
            code->size++;
        }

        // ���� (0~9)
        // RSA ��ó�� �� -> 53~62���� ���� 
        else if (piece >= 48 && piece <= 57) {
            code->array[code->size] = piece + 5;
            code->size++;
        }

        // Ư�� ���� 
        // RSA ��ó�� �� -> 63~73�� ���� 
        else {
            for (int i = 0; i < 13; i++) {
                if (piece == sign[i]) {
                    code->array[code->size] = i + 63;
                    break;
                }
                else if (i == 12)
                    code->array[code->size] = 63; // ������ ���� ���� 
            }
            code->size++;
        }
    }
}

// ��ó�� �ڵ� 2���� �� ���� �����
// ex) ab(12)�� ���� (a*100+b)�� ��ȯ, c(3)�� ���� c*100�� ��ȯ 
int get_integer(list* code) {

    static int i = 0;

    // i�� ����Ʈ�� �ְ� idx���� ���� �� 
    if (i >= code->size) {
        return -1;
    }

    // ������ ���Ҹ� �ٷ� �� (���� ������ Ȧ��)
    // ex) size=17, i=16
    else if (i == code->size - 1)
        return code->array[i++] * 100;

    // �Ϲ����� ���� �ٷ� ��
    // ex) size=17, i=5; size=17; i=15;
    else {
        int first = code->array[i] * 100;
        int second = code->array[i + 1];
        i += 2;
        return first + second;
    }

}

// �� ���� ����(bundle) ���� (ex) 102,1021,1200,..)
void make_Bundle(list* code, list* bundle) {
    int receive;
    while ((receive = get_integer(code)) != -1) {
        bundle->array[bundle->size++] = receive;
    }
}

// RSA ��ȣȭ C = M^e mod n
int make_C(int M) {
    unsigned long long C = 1; // ���ϱ� �ϸ鼭 �����÷ο� �� �� �����Ƿ� ũ�� ����
    for (int i = 0; i < e; i++) {
        C *= M;
        C %= n;
    }
    return C; // ���� ������ ���� int������ �ذ� ����
}

// ��ȣ ���� ���� ���� (ex) 3366,3450,9800,..)
void make_Encode(list* bundle, list* encode) {
    printf("\ne:%d, n:%d\n", e, n);
    for (int i = 0; i < bundle->size; i++) {
        int M = bundle->array[i];
        encode->array[encode->size] = make_C(M); // C = M^e mod n
        printf("�� ���� %d => ��ȣ ���� %d\n", M, encode->array[encode->size]);
        encode->size++;
    }
}

// RSA ��ȣȭ M = C^d mod n
int make_M(int C) {
    unsigned long long M = 1; // ���ϱ� �ϸ鼭 �����÷ο� �� �� �����Ƿ� ũ�� ����
    for (int i = 0; i < d; i++) {
        M *= C;
        M %= n;
    }
    return M; // ���� ������ ���� int������ �ذ� ����
}

// ��ȣ ����(�� ����) ���� ����(ex) 102, 1021, 1200, ..)
void make_Decode(list* encode, list* decode) {
    printf("\n\nd:%d, n:%d\n", d, n);
    for (int i = 0; i < encode->size; i++) {
        int C = encode->array[i];
        decode->array[decode->size] = make_M(C); // M = C^d mod n
        printf("��ȣ ���� %d => �� ���� %d\n", C, decode->array[decode->size]);
        decode->size++;
    }
}


// ��ȣ ����(�� ����) �����ؼ�  ��ó�� �ڵ� ��ȯ (ex) 102-> 1�� 2)
int* get_Code(list* decode) {

    static int z = 0; // idx���� 
    static int tmp[2]; // ���� ó�� -> �����ص� ���ÿ��� �� ������� 
    tmp[0] = tmp[1] = 0; // �� �Ź� 0���� �ʱ�ȭ 

    // z�� ����Ʈ�� �ְ� idx���� ���� �� 
    if (z >= decode->size) {
        return 0;
    }

    // ������ ���Ҹ� �ٷ� �� 
    // ex) 1819 1920 2100������ �� 2100
    else if (decode->array[z] % 100 == 0) {
        tmp[0] = decode->array[z++] / 100;
    }

    // �Ϲ����� ���Ҹ� �ٷ� �� 
    else {
        tmp[0] = decode->array[z] / 100;
        tmp[1] = decode->array[z] % 100;
        z++;
    }
    return tmp;

}

/* RSA ��ó�� �ڵ带 �ƽ�Ű�ڵ�� ��ȯ �� ���� ���
��(��ó�� �ڵ带 �� �������� �ݺ�) (ex) 1->97->'a'���, 2->98->'b'���, ...) */
void decode_Effect(list* decode) {

    int* receive; // ��ó�� �ڵ��� �ּҸ� ���� 
    int get = 0; // receive[0]�� receive[1]�� ����� 

    // get_Code �̿��ؼ� ��ó�� �ڵ� ��ȯ, ��ó�� �ڵ� ��� ���� ������ ���� ��� 
    while ((receive = get_Code(decode)) != 0) {
        for (int i = 0; i < 2; i++) {
            get = receive[i];
            if (get != 0) {
                // ���� �ҹ���
                if (get >= 1 && get <= 26)
                    printf("%c", get + 96);

                // ���� �빮��
                else if (get >= 27 && get <= 52)
                    printf("%c", get + 38);

                // ����
                else if (get >= 53 && get <= 62)
                    printf("%c", get - 5);

                // Ư������
                else {
                    for (int i = 0; i < 13; i++) {
                        // �Ѿ�� ����-Ư�����ڿ��� �Ÿ�=Ư�������� �� 
                        // ex) 63-29=32(' ')
                        if (get - sign_distance[i] == sign[i]) {
                            printf("%c", sign[i]);
                            break;
                        }
                    }
                }
            }
        }
    }
    printf("\n");

}
