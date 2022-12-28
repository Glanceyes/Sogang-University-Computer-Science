from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

## Example Agent
class ReflexAgent(Agent):

  def Action(self, gameState):

    move_candidate = gameState.getLegalActions()

    scores = [self.reflex_agent_evaluationFunc(gameState, action) for action in move_candidate]
    bestScore = max(scores)
    Index = [index for index in range(len(scores)) if scores[index] == bestScore]
    get_index = random.choice(Index)

    return move_candidate[get_index]

  def reflex_agent_evaluationFunc(self, currentGameState, action):

    successorGameState = currentGameState.generatePacmanSuccessor(action)
    newPos = successorGameState.getPacmanPosition()
    oldFood = currentGameState.getFood()
    newGhostStates = successorGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

    return successorGameState.getScore()



def scoreEvalFunc(currentGameState):

  return currentGameState.getScore()

class AdversialSearchAgent(Agent):

  def __init__(self, getFunc ='scoreEvalFunc', depth ='2'):
    self.index = 0
    self.evaluationFunction = util.lookup(getFunc, globals())

    self.depth = int(depth)



class MinimaxAgent(AdversialSearchAgent):
  """
    [문제 01] MiniMaxAgent의 Action을 구현하시오.
    (depth와 evaluation function은 위에서 정의한 self.depth and self.evaluationFunction을 사용할 것.)
  """
  def Action(self, gameState):
    ####################### Write Your Code Here ################################
    def is_terminal(gameState, currentDepth):
      return gameState.isWin() or gameState.isLose() or currentDepth == self.depth

    def minimax_search(gameState, depth, agent):
      # Pacman is an agent which number is 0
      if is_terminal(gameState, depth):
        return {'utility': self.evaluationFunction(gameState), 'move': None}

      agent = agent % gameState.getNumAgents()
      if agent == 0:
        return max_value(gameState, depth + 1, agent)
      else:
        return min_value(gameState, depth, agent)

      
    def max_value(gameState, depth, agent):
      highest_value = float("-inf")
      possible_moves = gameState.getLegalActions(agent)
      best_move = None

      for move in possible_moves:
        successor = gameState.generateSuccessor(agent, move)
        new_value = minimax_search(successor, depth, agent + 1)['utility']

        if new_value > highest_value:
          highest_value = new_value
          best_move = move
      
      return {'utility': highest_value, 'move': best_move}    
          
    def min_value(gameState, depth, agent):
      lowest_value = float("inf")
      possible_moves = gameState.getLegalActions(agent)
      best_move = None

      for move in possible_moves:
        successor = gameState.generateSuccessor(agent, move)
        new_value = minimax_search(successor, depth, agent + 1)['utility']

        if new_value < lowest_value:
          lowest_value = new_value
          best_move = move
      
      return {'utility': lowest_value, 'move': best_move}   

    return minimax_search(gameState, 0, 0)['move']

    ############################################################################




class AlphaBetaAgent(AdversialSearchAgent):
  """
    [문제 02] AlphaBetaAgent의 Action을 구현하시오.
    (depth와 evaluation function은 위에서 정의한 self.depth and self.evaluationFunction을 사용할 것.)
  """
  def Action(self, gameState):
    ####################### Write Your Code Here ################################
    alpha = float("-inf")
    beta = float("inf")

    def is_terminal(gameState, currentDepth):
      return gameState.isWin() or gameState.isLose() or currentDepth == self.depth

    def alpha_beta_search(gameState, depth, agent, alpha, beta):
      # Pacman is an agent which number is 0
      if is_terminal(gameState, depth):
        return {'utility': self.evaluationFunction(gameState), 'move': None}

      agent = agent % gameState.getNumAgents()
      if agent == 0:
        return max_value(gameState, depth + 1, agent, alpha, beta)
      else:
        return min_value(gameState, depth, agent, alpha, beta)

      
    def max_value(gameState, depth, agent, alpha, beta):
      highest_value = float("-inf")
      possible_moves = gameState.getLegalActions(agent)
      best_move = None

      for move in possible_moves:
        successor = gameState.generateSuccessor(agent, move)
        new_value = alpha_beta_search(successor, depth, agent + 1, alpha, beta)['utility']

        if new_value > highest_value:
          highest_value = new_value
          best_move = move
          alpha = max(alpha, highest_value)
        
        if highest_value >= beta:
          return {'utility': highest_value, 'move': move}
      
      return {'utility': highest_value, 'move': best_move}    
          
        
    def min_value(gameState, depth, agent, alpha, beta):
      lowest_value = float("inf")
      possible_moves = gameState.getLegalActions(agent)
      best_move = None

      for move in possible_moves:
        successor = gameState.generateSuccessor(agent, move)
        new_value = alpha_beta_search(successor, depth, agent + 1, alpha, beta)['utility']

        if new_value < lowest_value:
          lowest_value = new_value
          best_move = move
          beta = min(beta, lowest_value)

        if lowest_value <= alpha:
          return {'utility': lowest_value, 'move': move}

      return {'utility': lowest_value, 'move': best_move}   

    return alpha_beta_search(gameState, 0, 0, alpha, beta)['move']

    ############################################################################



class ExpectimaxAgent(AdversialSearchAgent):
  """
    [문제 03] ExpectimaxAgent의 Action을 구현하시오.
    (depth와 evaluation function은 위에서 정의한 self.depth and self.evaluationFunction을 사용할 것.)
  """
  def Action(self, gameState):
    ####################### Write Your Code Here ################################

    def is_terminal(gameState, currentDepth):
      return gameState.isWin() or gameState.isLose() or currentDepth == self.depth

    def expectimax_search(gameState, depth, agent):
      # Pacman is an agent which number is 0
      if is_terminal(gameState, depth):
        return {'utility': self.evaluationFunction(gameState), 'move': None}

      agent = agent % gameState.getNumAgents()
      if agent == 0:
        return max_value(gameState, depth + 1, agent)
      else:
        return min_value(gameState, depth, agent)

      
    def max_value(gameState, depth, agent):
      highest_value = float("-inf")
      possible_moves = gameState.getLegalActions(agent)
      best_move = None

      for move in possible_moves:
        successor = gameState.generateSuccessor(agent, move)
        new_value = expectimax_search(successor, depth, agent + 1)['utility']

        if new_value > highest_value:
          highest_value = new_value
          best_move = move
      
      return {'utility': highest_value, 'move': best_move}    
          
        
    def min_value(gameState, depth, agent):
      possible_moves = gameState.getLegalActions(agent)
      best_move = None
      expected_value = 0
      probability = 1 / len(possible_moves)
      
      for move in possible_moves:
        successor = gameState.generateSuccessor(agent, move)
        new_value = expectimax_search(successor, depth, agent + 1)['utility']
        expected_value += probability * new_value
      
      return {'utility': expected_value, 'move': best_move}   

    return expectimax_search(gameState, 0, 0)['move']

    ############################################################################
