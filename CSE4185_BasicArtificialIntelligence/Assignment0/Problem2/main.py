"""
DO NOT CHANGE THIS FILE
THIS FILE DOES NOT HAVE TO BE SUBMITTED
"""
import argparse
from homework import ReadEliminateSignFromPaper

def get_args_parser():
    parser = argparse.ArgumentParser('homework0 AI 2022 fall', description='homework0 AI 2022 fall', add_help=True)
    parser.add_argument('--input_file', type=str, default='input.txt')
    parser.add_argument('--output_file', type=str, default='output.txt')
    return parser

if __name__ == '__main__':
    parser = get_args_parser()
    args = parser.parse_args()
    
    read_paper = ReadEliminateSignFromPaper(args.input_file)
    
    print(read_paper.get_sorted_sign())
    print(len(read_paper.sentences))
    print('The first sentence of the paper:', read_paper[0])
    print('The second sentence of the paper:', read_paper[1])
    print('The last second sentence of the paper:', read_paper[-2])
    print('The last sentence of the paper:', read_paper[-1])
    
else:
    parser = get_args_parser()
    args = parser.parse_args()
    
    read_paper = ReadEliminateSignFromPaper(args.input_file)