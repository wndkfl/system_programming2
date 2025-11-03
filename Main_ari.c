#include <stdio.h>
#include <stdlib.h>
#include "ari.h"

int main(int argc, char *argv[]) {
    char *inst_file = "inst.data";   // 명령어 정의 파일
    char *input_file = "input.txt";  // 어셈블리 소스 파일
    char *output_file = "output.txt";// 출력 파일 이름 (현재 미사용)

    // 명령행 인자 처리
    if (argc >= 2) input_file = argv[1];
    if (argc >= 3) output_file = argv[2];

    // 1. 명령어 테이블 초기화
    if (init_inst_file(inst_file) < 0) return -1;

    // 2. 입력 파일 로드
    if (init_input_file(input_file) < 0) return -1;

    // 3. 각 줄 토큰화
    for (int i = 0; i < line_num; i++)
        if (token_parsing(input_data[i]) < 0) return -1;

    // 4. 결과 출력
    make_opcode_output(output_file);

    // 5. 메모리 해제
    for (int i = 0; i < line_num; i++) {
        if (input_data[i]) free(input_data[i]);
        if (token_table[i]) {
            if (token_table[i]->label) free(token_table[i]->label);
            if (token_table[i]->op) free(token_table[i]->op);
            free(token_table[i]);
        }
    }
    for (int i = 0; i < inst_index; i++)
        if (inst_table[i]) free(inst_table[i]);

    return 0;
}
