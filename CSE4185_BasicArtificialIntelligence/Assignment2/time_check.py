#!/usr/bin/env python3

import time_info

test = time_info.TimeCheck()
hw02 = test.load('hw02')

from game import Agent
from ghostAgents import RandomGhost, DirectionalGhost
import  sys

import pacman, time, layout, textDisplay
textDisplay.SLEEP_TIME = 0
textDisplay.DRAW_EVERY = 1000
thismodule = sys.modules[__name__]

TIME_LIMIT = 3000

def run(layname, pac, ghosts, nGames =300):

  if test.fatalError:
    return {'time': 65536, 'wins': 0, 'games': None, 'scores': [0]*nGames, 'timeouts': nGames}

  lay = layout.getLayout(layname, 3)
  disp = textDisplay.NullGraphics()

  games = pacman.runGames(lay, pac, ghosts, disp, nGames, False, numTraining =0,catchExceptions=False)
  stats = {'wins': [g.state.isWin() for g in games].count(True), 'games': games, 'scores': [g.state.getScore() for g in games], 'timeouts': [g.agentTimeout for g in games].count(True)}

  return stats


## test case 300
def medium_map(agentName):
  stats = {}
  if agentName == 'alphabeta':
    stats = run('mediummap', hw02.AlphaBetaAgent(depth=3), [DirectionalGhost(i + 1) for i in range(2)])
  elif agentName == 'minimax':
    stats = run('mediummap', hw02.MinimaxAgent(depth=3), [DirectionalGhost(i + 1) for i in range(2)])
  else:
    stats = run('mediummap', hw02.ExpectimaxAgent(depth=3), [DirectionalGhost(i + 1) for i in range(2)])
  if stats['timeouts'] > 0:
    test.fail('Your ' + agentName + ' agent timed out on mediummap.')
    return

## test case 1000
def minimax_map(agentName):
  stats = {}
  if agentName == 'alphabeta':
    stats = run('minimaxmap', hw02.AlphaBetaAgent(depth=4), [DirectionalGhost(i + 1) for i in range(2)],1000)
  elif agentName == 'minimax':
    stats = run('minimaxmap', hw02.MinimaxAgent(depth=4), [DirectionalGhost(i + 1) for i in range(2)],1000)
  else:
    stats = run('minimaxmap', hw02.ExpectimaxAgent(depth=4), [DirectionalGhost(i + 1) for i in range(2)],1000)
  if stats['timeouts'] > 0:
    test.fail('Your ' + agentName + ' agent timed out on minimaxmap.')
    return


test.addTest('MiniMax (depth=3) For Medium Map', lambda : medium_map('minimax'), maxSeconds=TIME_LIMIT, description='MiniMax for timeout on mediumMap.')
test.addTest('AlphaBeta (depth=3) For Medium Map', lambda : medium_map('alphabeta'), maxSeconds=TIME_LIMIT, description='AlphaBeta for timeout on mediumMap.')

test.addTest('MiniMax (depth=4) For Minimax Map', lambda : minimax_map('minimax'), maxSeconds=TIME_LIMIT, description='MiniMax for timeout on minimaxMap.')
test.addTest('AlphaBeta (depth=4) For Minimax Map', lambda : minimax_map('alphabeta'), maxSeconds=TIME_LIMIT, description='AlphaBeta for timeout on minimaxMap.')


test.start()
