# CSE4185 Assignment02: Pacman

## Implement:
Minimax, Alpha-beta Pruning, Expectimax를 활용한 Multi-Agent 구현하기 

![upload img](https://i.esdrop.com/d/koodoeiok4nv/1qNP1c9liu.png)

## How to run the code:
The main file to run the pacman is pacman.py:


Examples of how to run pacman.py:
```
python pacman.py -p ReflexAgent -m bigmap -n 1 -g DirectionalGhost -z 0.8 -k 1
```
```
python pacman.py -p MinimaxAgent -m minimaxmap -a depth=4
```

```
python pacman.py -p AlphaBetaAgent -m 4185map -a depth=2
```

```
python pacman.py -p ExpectimaxAgent -m mediummap -a depth=2
```

For help run:
```
python pacman.py -h
```
Help Output:
```
CSE4185 Assignment02: Pacman

Usage:
  USAGE:      python pacman.py <options>
  EXAMPLES:   (1) python pacman.py
                  - starts an interactive game
              (2) python pacman.py --map smallmap --zoom 2
              OR  python pacman.py -m smallmap -z 2
                  - starts an interactive game on a smaller board, zoomed in


Options:
  -h, --help            show this help message and exit
  -n GAMES, --numGames=GAMES
                        the number of GAMES to play [Default: 1]
  -m MAP_FILE, --map=MAP_FILE
                        the MAP_FILE from which to load the map layout
                        [Default: mediummap]
  -p TYPE, --pacman=TYPE
                        the agent TYPE in the pacmanAgents module to use
                        [Default: HumanAgent]
  -t, --textGraphics    Display output as text only
  -q, --quietTextGraphics
                        Generate minimal output and no graphics
  -g TYPE, --ghosts=TYPE
                        the ghost agent TYPE in the ghostAgents module to use
                        [Default: RandomGhost]
  -k NUMGHOSTS, --numghosts=NUMGHOSTS
                        The maximum number of ghosts to use [Default: 4]
  -z ZOOM, --zoom=ZOOM  Zoom the size of the graphics window [Default: 1.0]
  -f, --fixRandomSeed   Fixes the random seed to always play the same game
  -r, --recordActions   Writes game histories to a file (named by the time
                        they were played)
  --replay=GAMETOREPLAY
                        A recorded game file (pickle) to replay
  -a AGENTARGS, --agentArgs=AGENTARGS
                        Comma separated values sent to agent. e.g.
                        "opt1=val1,opt2,opt3=val3"
  -x NUMTRAINING, --numTraining=NUMTRAINING
                        How many episodes are training (suppresses output)
                        [Default: 0]
  --frameTime=FRAMETIME
                        Time to delay between frames; <0 means keyboard
                        [Default: 0.1]
  -c, --catchExceptions
                        Turns on exception handling and timeouts during games
  --timeout=TIMEOUT     Maximum length of time an agent can spend computing in
                        a single game [Default: 30]



```