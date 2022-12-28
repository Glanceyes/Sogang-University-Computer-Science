"""
DO NOT CHANGE THIS FILE
THIS FILE DOES NOT HAVE TO BE SUBMITTED
"""
import main

def string2list(s):
    import re
    res = []
    temp = re.findall("\('.', \d+\)", line)
    for x in temp:
        sign, num = re.search("\('(.)', (\d+)\)", x).groups()
        res.append((sign, int(num)))
    return res

if __name__ == '__main__':
    args = main.args
    read_paper = main.read_paper
    
    su_list = []
    index_list = [0,1,-2,-1]
    with open(args.output_file, 'r', encoding='utf8') as f:
        lines = f.readlines()
    
    for i, line in enumerate(lines):
        line = line.strip()
        if i==0:
            line = string2list(line)
            su_list.append('SUCCESS' if line == read_paper.get_sorted_sign() else 'FAIL')
        elif i==1:
            line = int(line)
            su_list.append('SUCCESS' if line == len(read_paper.sentences) else 'FAIL')
        else:
            su_list.append('SUCCESS' if line == read_paper[index_list[i-2]] else 'FAIL')
    for i in range(len(lines)):
        print(f'line{i+1}: {su_list[i]}')