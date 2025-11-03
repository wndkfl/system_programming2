#ifndef ARI_H
#define ARI_H

#define MAX_INST 256       // 최대 명령어(instruction) 개수
#define MAX_LINES 5000     // 최대 입력 줄 수
#define MAX_OPERAND 3      // 한 명령어의 최대 피연산자 수

/* 
 * instruction 정보를 저장하는 구조체
 * 예: LDA M 3 00
 * str: "LDA"
 * op: 0x00
 * format: 3 (형식)
 * ops: operand 개수
 */
struct inst_unit {
    char str[10];        // 명령어 문자열 (예: "LDA")
    unsigned char op;    // 명령어의 OPCODE 값 (16진수)
    int format;          // 명령어 형식 (1, 2, 3 등)
    int ops;             // operand 개수
};
typedef struct inst_unit inst;

/*
 * 한 줄의 어셈블리 코드를 구성하는 요소들을 저장하기 위한 구조체
 * 예: LABEL  LDA  BUFFER  .COMMENT
 */
struct token_unit {
    char *label;                      // 라벨 (선택적)
    char *op;                         // 명령어 (예: LDA, STA 등)
    char operand[MAX_OPERAND][20];   // 피연산자 배열
    char comment[100];               // 주석 부분 ('.'부터 끝까지)
};
typedef struct token_unit token;

/* 전역 변수 선언부 */
extern char *input_data[MAX_LINES];   // 입력 파일의 각 줄 저장
extern int line_num;                  // 총 입력 줄 수
extern token *token_table[MAX_LINES]; // 각 줄별 토큰 구조체 포인터
extern inst *inst_table[MAX_INST];    // instruction 정의 테이블
extern int inst_index;                // inst_table 내 유효한 데이터 수

/* 함수 선언부 */
int init_inst_file(char *inst_file);   // inst.data 파일 초기화
int init_input_file(char *input_file); // input.txt 파일 읽기
int token_parsing(char *str);          // 한 줄을 파싱하여 token_table에 저장
int search_opcode(char *str);          // 명령어 이름으로 inst_table 검색
void make_opcode_output(char *file_name); // 파싱 결과 출력

#endif
