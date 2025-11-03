#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ari.h"

/* 
 * 전역 변수
 * - input_data : 입력 파일의 각 라인 저장
 * - token_table : 파싱된 토큰 저장
 * - inst_table : 명령어 정보 저장
 */
char *input_data[MAX_LINES];
int line_num = 0;
token *token_table[MAX_LINES];
inst *inst_table[MAX_INST];
int inst_index = 0;

/*
 * init_inst_file()
 * - inst.data 파일을 읽어 inst_table에 명령어 정보 저장
 * - 형식: MNEMONIC FORMAT OPERANDS OPCODE
 */
int init_inst_file(char *inst_file) {
    FILE *file = fopen(inst_file, "r");
    if (!file) return printf("Error: Cannot open %s\n", inst_file), -1;

    char line[100], format[10], opcode_str[10];
    inst_index = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '\0') continue; // 빈 줄 무시

        inst *new_inst = malloc(sizeof(inst));
        if (!new_inst) return printf("Memory alloc fail\n"), -1;

        // 파일 한 줄을 파싱하여 구조체에 저장
        sscanf(line, "%s %s %d %s", new_inst->str, format, &new_inst->ops, opcode_str);

        // FORMAT 문자열을 숫자 형태로 변환
        if (!strcmp(format, "-")) new_inst->format = 0;
        else if (!strcmp(format, "M")) new_inst->format = 3;
        else new_inst->format = 2;

        // OPCODE(16진수) 저장
        sscanf(opcode_str, "%hhx", &new_inst->op);
        inst_table[inst_index++] = new_inst;
    }
    fclose(file);
    return 0;
}

/*
 * init_input_file()
 * - 어셈블리 소스코드를 input_data[]에 한 줄씩 저장
 */
int init_input_file(char *input_file) {
    FILE *file = fopen(input_file, "r");
    if (!file) return printf("Error: Cannot open %s\n", input_file), -1;

    char line[256];
    line_num = 0;

    // fgets로 파일의 각 줄을 읽어 저장
    while (fgets(line, sizeof(line), file) && line_num < MAX_LINES) {
        line[strcspn(line, "\n")] = 0;   // 개행 제거
        input_data[line_num++] = strdup(line); // 문자열 복사 및 저장
    }

    fclose(file);
    return 0;
}

/*
 * token_parsing()
 * - 한 줄의 소스코드를 label / op / operand / comment로 나눔
 * - token_table[]에 저장
 */
int token_parsing(char *str) {
    static int idx = 0;  // 현재 라인 인덱스
    if (idx >= MAX_LINES) return -1;

    // 빈 줄 또는 주석 줄('.' 시작)은 NULL로 처리
    if (!str[0] || str[0] == '.') {
        token_table[idx++] = NULL;
        return idx - 1;
    }

    // 토큰 구조체 생성 및 초기화
    token *t = calloc(1, sizeof(token));
    char temp[256]; 
    strcpy(temp, str);

    // 주석 분리('.' 기준)
    char *comment = strchr(temp, '.');
    if (comment) {
        strcpy(t->comment, comment);
        *comment = '\0'; // 주석 부분 제거
    }

    // 공백, 탭으로 구분하여 토큰 분리
    char *tok = strtok(temp, " \t"), *parts[10];
    int count = 0;
    while (tok && count < 10) parts[count++] = tok, tok = strtok(NULL, " \t");

    int pos = 0;

    // 첫 단어가 label이면 저장
    if (str[0] != ' ' && str[0] != '\t') t->label = strdup(parts[pos++]);

    // operator(opcode 문자열)
    if (pos < count) t->op = strdup(parts[pos++]);

    // operand (최대 3개)
    for (int i = 0; pos < count && i < MAX_OPERAND; i++, pos++)
        strcpy(t->operand[i], parts[pos]);

    token_table[idx++] = t;
    return idx - 1;
}

/*
 * search_opcode()
 * - inst_table에서 명령어 이름(str)을 검색
 * - 찾으면 인덱스 반환, 없으면 -1
 */
int search_opcode(char *str) {
    for (int i = 0; i < inst_index; i++)
        if (!strcmp(inst_table[i]->str, str))
            return i;
    return -1;
}

/*
 * make_opcode_output()
 * - 입력 소스와 파싱 결과를 바탕으로 각 명령어의 OPCODE를 터미널에 출력
 */
void make_opcode_output(char *unused) {
    printf("\n========== OUTPUT ==========\n\n");

    for (int i = 0; i < line_num; i++) {
        // 빈 줄 / 주석 줄 제외
        if (!input_data[i][0] || input_data[i][0] == '.') continue;

        // 원본 라인 출력
        printf("%-40s", input_data[i]);

        // OPCODE 출력
        if (token_table[i] && token_table[i]->op) {
            int idx = search_opcode(token_table[i]->op);
            if (idx != -1) printf("\t%02X", inst_table[idx]->op);
        }
        printf("\n");
    }

    printf("\n============================\n");
}
