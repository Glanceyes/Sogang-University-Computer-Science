# 2022_02 CSE4185 Assignment01: Maze Game

## Implement:
BFS,DFS와 A * Algorithm을 활용한 최단 경로 찾기 

## Requirements:
```
pygame library (pip install pygame) 
```
## How to run the code:

```
usage: main.py [-h] [--method {bfs,dfs,astar,astar_four_circles,astar_many_circles}] [--scale SCALE]
              [--speed speed] [--keyboard] [--save SAVE]
              filename
```

Examples of how to run main.py:
```
python3 main.py category/stage1/small.txt --method bfs
```
```
python3 main.py category/stage1/small.txt --scale 30 --speed 10 --keyboard
```

For help run:
```
python3 main.py -h
```
Help Output:
```
CSE4185 Assignment01: Maze Game

positional arguments:
  filename              path to maze file [REQUIRED]

optional arguments:
  -h, --help            show this help message and exit
  --method {bfs,dfs,astar,astar_four_circles,astar_many_circles}
                        search method - default bfs
  --scale SCALE         scale - default: 20
  --speed SPEED         speed for the display - default 15
  --keyboard            you can play the game - default False
  --save SAVE           save output to image file - default not saved
```
