"""
.c로 끝나는 c 파일들을 모두 읽어, 공백이나 주석, 괄호만 있는 줄 같은 의미 없는 줄들을 모두 제외하고
실질적으로 의미를 가지는 코드 줄의 개수를 세는 스크립트입니다.
실행되는 디랙토리와 그 하위 디렉토리에서 모든 .c 파일을 찾아서 코드 줄 수를 계산합니다.
"""

import os

def count_code_lines_in_c_files(directory):
    total_lines = 0
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.c'):
                file_path = os.path.join(root, file)
                with open(file_path, 'r', encoding='utf-8') as f:
                    for line in f:
                        stripped_line = line.strip()
                        # Check if the line is not empty and does not contain only comments or parentheses
                        if stripped_line and not (stripped_line.startswith('//') or stripped_line.startswith('/*') or stripped_line.endswith('*/') or stripped_line == '{' or stripped_line == '}'):
                            total_lines += 1
    return total_lines

if __name__ == "__main__":
    current_directory = os.getcwd()
    code_line_count = count_code_lines_in_c_files(current_directory)
    print(f"Total meaningful code lines in .c files: {code_line_count}")