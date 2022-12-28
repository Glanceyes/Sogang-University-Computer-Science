from game import GameStateData
from game import Game
from game import Directions
from game import Actions
from util import nearestPoint
from util import manhattanDistance
import util, layout
import sys, types, time, random, os

class GameState:

  ## Returns the legal actions for the agent specified.
  def getLegalActions( self, agentIndex=0 ):

    if self.isWin() or self.isLose(): return []

    if agentIndex == 0:  # Pacman is moving
      return PacmanRules.getLegalActions( self )
    else:
      return GhostRules.getLegalActions( self, agentIndex )

  ## Returns the successor state after the specified agent takes the action.(Pac-Man is always agent 0.)
  def generateSuccessor( self, agentIndex, action):

    # Check that successors exist
    if self.isWin() or self.isLose(): raise Exception('Can\'t generate a successor of a terminal state.')

    # Copy current state
    state = GameState(self)

    if agentIndex == 0:  # Pacman is moving
      state.data._eaten = [False for i in range(state.getNumAgents())]
      PacmanRules.applyAction( state, action )
    else:                # A ghost is moving
      GhostRules.applyAction( state, action, agentIndex )

    # Time passes
    if agentIndex == 0:
      state.data.scoreChange += -TIME_PENALTY # Penalty for waiting around
    else:
      GhostRules.decrementTimer( state.data.agentStates[agentIndex] )

    # Resolve multi-agent effects
    GhostRules.checkDeath( state, agentIndex )

    state.data._agentMoved = agentIndex
    state.data.score += state.data.scoreChange
    return state

  def getLegalPacmanActions( self ):
    return self.getLegalActions( 0 )

  # Generates the successor state after the specified pacman move
  def generatePacmanSuccessor( self, action ):

    return self.generateSuccessor( 0, action )

  ## Returns an AgentState object for pacman (in game.py)
  def getPacmanState( self ):

    return self.data.agentStates[0].copy()

  def getPacmanPosition( self ):
    return self.data.agentStates[0].getPosition()

  ## Returns list of AgentState objects for the ghosts
  def getGhostStates( self ):
    return self.data.agentStates[1:]

  def getGhostState( self, agentIndex ):
    if agentIndex == 0 or agentIndex >= self.getNumAgents():
      raise Exception("Invalid index passed to getGhostState")
    return self.data.agentStates[agentIndex]

  def getGhostPosition( self, agentIndex ):
    if agentIndex == 0:
      raise Exception("Pacman's index passed to getGhostPosition")
    return self.data.agentStates[agentIndex].getPosition()

  def getGhostPositions(self):
    return [s.getPosition() for s in self.getGhostStates()]

  ## Returns the total number of agents in the game
  def getNumAgents( self ):
    return len( self.data.agentStates )

  ## Returns the score corresponding to the current state of the game
  def getScore( self ):
    return self.data.score

  ## Returns a list of positions (x,y) of the remaining capsules.
  def getCapsules(self):

    return self.data.capsules

  def getNumFood( self ):
    return self.data.food.count()

  ## Returns a Grid of boolean food indicator variables.
  def getFood(self):

    return self.data.food

  ## Returns a Grid of boolean wall indicator variables.
  def getWalls(self):
    return self.data.layout.walls

  def hasFood(self, x, y):
    return self.data.food[x][y]

  def hasWall(self, x, y):
    return self.data.layout.walls[x][y]

  # Returns True if it's a losing state
  def isLose( self ):
    return self.data._lose

  # Returns True if it's a winning state
  def isWin( self ):
    return self.data._win

  def __init__( self, prevState = None ):

    if prevState is not None:
      self.data = GameStateData(prevState.data)
    else:
      self.data = GameStateData()

  def deepCopy( self ):
    state = GameState( self )
    state.data = self.data.deepCopy()
    return state

  def __eq__( self, other ):

    if other is None: return False
    return self.data == other.data

  def __hash__( self ):

    return hash( self.data )

  def __str__( self ):

    return str(self.data)

  def initialize( self, layout, numGhostAgents=1000 ):

    self.data.initialize(layout, numGhostAgents)


SCARED_TIME = 40
COLLISION_TOLERANCE = 0.7
TIME_PENALTY = 1

class ClassicGameRules:

  def __init__(self, timeout=30):
    self.timeout = timeout
    self.count=0

  def newGame( self, layout, pacmanAgent, ghostAgents, display, quiet = False, catchExceptions=False):
    agents = [pacmanAgent] + ghostAgents[:layout.getNumGhosts()]
    initState = GameState()
    initState.initialize( layout, len(ghostAgents) )
    game = Game(agents, display, self, catchExceptions=catchExceptions)
    game.state = initState
    self.initialState = initState.deepCopy()
    self.quiet = quiet
    return game

  def process(self, state, game):

    if state.isWin(): self.win(state, game)
    if state.isLose(): self.lose(state, game)

  def win( self, state, game ):
    if self.count==0:
      print("======================================")
    if not self.quiet:
      self.count += 1
      print("["+str(self.count)+"] Pacman Win! Score: %d" % state.data.score)
    game.gameOver = True

  def lose( self, state, game ):
    if self.count==0:
      print("======================================")
    if not self.quiet:
      self.count += 1
      print("["+str(self.count)+"] Pacman Lose... Score: %d" % state.data.score)
    game.gameOver = True

  def getProgress(self, game):
    return float(game.state.getNumFood()) / self.initialState.getNumFood()

  def agentCrash(self, game, agentIndex):
    if agentIndex == 0:
      print("Pacman crashed")
    else:
      print("A ghost crashed")

  def getMaxTotalTime(self, agentIndex):
    return self.timeout

  def getMaxStartupTime(self, agentIndex):
    return self.timeout

  def getMoveWarningTime(self, agentIndex):
    return self.timeout

  def getMoveTimeout(self, agentIndex):
    return self.timeout

  def getMaxTimeWarnings(self, agentIndex):
    return 0

class PacmanRules:

  PACMAN_SPEED=1

  def getLegalActions( state ):

    possibleActions = Actions.getPossibleActions( state.getPacmanState().configuration, state.data.layout.walls )
    if Directions.STOP in possibleActions:
      possibleActions.remove( Directions.STOP )
    return possibleActions
  getLegalActions = staticmethod( getLegalActions )

  def applyAction( state, action ):

    legal = PacmanRules.getLegalActions( state )
    if action not in legal:
      raise Exception("Illegal action " + str(action))

    pacmanState = state.data.agentStates[0]

    # Update Configuration
    vector = Actions.directionToVector( action, PacmanRules.PACMAN_SPEED )
    pacmanState.configuration = pacmanState.configuration.generateSuccessor( vector )

    # Eat
    next = pacmanState.configuration.getPosition()
    nearest = nearestPoint( next )
    if manhattanDistance( nearest, next ) <= 0.5 :
      # Remove food
      PacmanRules.consume( nearest, state )
  applyAction = staticmethod( applyAction )

  def consume( position, state ):
    x,y = position
    # Eat food
    if state.data.food[x][y]:
      state.data.scoreChange += 10
      state.data.food = state.data.food.copy()
      state.data.food[x][y] = False
      state.data._foodEaten = position
      # TODO: cache numFood?
      numFood = state.getNumFood()
      if numFood == 0 and not state.data._lose:
        state.data.scoreChange += 500
        state.data._win = True
    # Eat capsule
    if( position in state.getCapsules() ):
      state.data.capsules.remove( position )
      state.data._capsuleEaten = position
      # Reset all ghosts' scared timers
      for index in range( 1, len( state.data.agentStates ) ):
        state.data.agentStates[index].scaredTimer = SCARED_TIME
  consume = staticmethod( consume )

class GhostRules:

  GHOST_SPEED=1.0
  def getLegalActions( state, ghostIndex ):

    conf = state.getGhostState( ghostIndex ).configuration
    possibleActions = Actions.getPossibleActions( conf, state.data.layout.walls )
    reverse = Actions.reverseDirection( conf.direction )
    if Directions.STOP in possibleActions:
      possibleActions.remove( Directions.STOP )
    if reverse in possibleActions and len( possibleActions ) > 1:
      possibleActions.remove( reverse )
    return possibleActions
  getLegalActions = staticmethod( getLegalActions )

  def applyAction( state, action, ghostIndex):

    legal = GhostRules.getLegalActions( state, ghostIndex )
    if action not in legal:
      raise Exception("Illegal ghost action " + str(action))

    ghostState = state.data.agentStates[ghostIndex]
    speed = GhostRules.GHOST_SPEED
    if ghostState.scaredTimer > 0: speed /= 2.0
    vector = Actions.directionToVector( action, speed )
    ghostState.configuration = ghostState.configuration.generateSuccessor( vector )
  applyAction = staticmethod( applyAction )

  def decrementTimer( ghostState):
    timer = ghostState.scaredTimer
    if timer == 1:
      ghostState.configuration.pos = nearestPoint( ghostState.configuration.pos )
    ghostState.scaredTimer = max( 0, timer - 1 )
  decrementTimer = staticmethod( decrementTimer )

  def checkDeath( state, agentIndex):
    pacmanPosition = state.getPacmanPosition()
    if agentIndex == 0: # Pacman just moved; Anyone can kill him
      for index in range( 1, len( state.data.agentStates ) ):
        ghostState = state.data.agentStates[index]
        ghostPosition = ghostState.configuration.getPosition()
        if GhostRules.canKill( pacmanPosition, ghostPosition ):
          GhostRules.collide( state, ghostState, index )
    else:
      ghostState = state.data.agentStates[agentIndex]
      ghostPosition = ghostState.configuration.getPosition()
      if GhostRules.canKill( pacmanPosition, ghostPosition ):
        GhostRules.collide( state, ghostState, agentIndex )
  checkDeath = staticmethod( checkDeath )

  def collide( state, ghostState, agentIndex):
    if ghostState.scaredTimer > 0:
      state.data.scoreChange += 200
      GhostRules.placeGhost(state, ghostState)
      ghostState.scaredTimer = 0
      # Added for first-person
      state.data._eaten[agentIndex] = True
    else:
      if not state.data._win:
        state.data.scoreChange -= 500
        state.data._lose = True
  collide = staticmethod( collide )

  def canKill( pacmanPosition, ghostPosition ):
    return manhattanDistance( ghostPosition, pacmanPosition ) <= COLLISION_TOLERANCE
  canKill = staticmethod( canKill )

  def placeGhost(state, ghostState):
    ghostState.configuration = ghostState.start
  placeGhost = staticmethod( placeGhost )


def default(str):
  return str + ' [Default: %default]'

def parseAgentArgs(str):
  if str is None: return {}
  pieces = str.split(',')
  opts = {}
  for p in pieces:
    if '=' in p:
      key, val = p.split('=')
    else:
      key,val = p, 1
    opts[key] = val
  return opts

def readCommand( argv ):

  from optparse import OptionParser

  usageStr = """
  USAGE:      python pacman.py <options>
  EXAMPLES:   (1) python pacman.py
                  - starts an interactive game
              (2) python pacman.py --map smallmap --zoom 2
              OR  python pacman.py -m smallmap -z 2
                  - starts an interactive game on a smaller board, zoomed in
  """
  parser = OptionParser(usageStr)

  parser.add_option('-n', '--numGames', dest='numGames', type='int',
                    help=default('the number of GAMES to play'), metavar='GAMES', default=1)
  parser.add_option('-m', '--map', dest='map',
                    help=default('the MAP_FILE from which to load the map layout'),
                    metavar='MAP_FILE', default='mediummap')
  parser.add_option('-p', '--pacman', dest='pacman',
                    help=default('the agent TYPE in the pacmanAgents module to use'),
                    metavar='TYPE', default='HumanAgent')
  parser.add_option('-t', '--textGraphics', action='store_true', dest='textGraphics',
                    help='Display output as text only', default=False)
  parser.add_option('-q', '--quietTextGraphics', action='store_true', dest='quietGraphics',
                    help='Generate minimal output and no graphics', default=False)
  parser.add_option('-g', '--ghosts', dest='ghost',
                    help=default('the ghost agent TYPE in the ghostAgents module to use'),
                    metavar = 'TYPE', default='RandomGhost')
  parser.add_option('-k', '--numghosts', type='int', dest='numGhosts',
                    help=default('The maximum number of ghosts to use'), default=4)
  parser.add_option('-z', '--zoom', type='float', dest='zoom',
                    help=default('Zoom the size of the graphics window'), default=1.0)
  parser.add_option('-f', '--fixRandomSeed', action='store_true', dest='fixRandomSeed',
                    help='Fixes the random seed to always play the same game', default=False)
  parser.add_option('-r', '--recordActions', action='store_true', dest='record',
                    help='Writes game histories to a file (named by the time they were played)', default=False)
  parser.add_option('--replay', dest='gameToReplay',
                    help='A recorded game file (pickle) to replay', default=None)
  parser.add_option('-a','--agentArgs',dest='agentArgs',
                    help='Comma separated values sent to agent. e.g. "opt1=val1,opt2,opt3=val3"')
  parser.add_option('-x', '--numTraining', dest='numTraining', type='int',
                    help=default('How many episodes are training (suppresses output)'), default=0)
  parser.add_option('--frameTime', dest='frameTime', type='float',
                    help=default('Time to delay between frames; <0 means keyboard'), default=0.1)
  parser.add_option('-c', '--catchExceptions', action='store_true', dest='catchExceptions', 
                    help='Turns on exception handling and timeouts during games', default=False)
  parser.add_option('--timeout', dest='timeout', type='int',
                    help=default('Maximum length of time an agent can spend computing in a single game'), default=30)

  options, otherjunk = parser.parse_args(argv)
  if len(otherjunk) != 0:
    raise Exception('Command line input not understood: ' + str(otherjunk))
  args = dict()

  # Fix the random seed
  if options.fixRandomSeed: random.seed('cse4185')

  # Choose a layout
  args['layout'] = layout.getLayout( options.map )
  if args['layout'] is None: raise Exception("The layout " + options.layout + " cannot be found")

  # Choose a Pacman agent
  noKeyboard = options.gameToReplay is None and (options.textGraphics or options.quietGraphics)
  pacmanType = loadAgent(options.pacman, noKeyboard)
  agentOpts = parseAgentArgs(options.agentArgs)
  if options.numTraining > 0:
    args['numTraining'] = options.numTraining
    if 'numTraining' not in agentOpts: agentOpts['numTraining'] = options.numTraining
  pacman = pacmanType(**agentOpts) # Instantiate Pacman with agentArgs
  args['pacman'] = pacman

  # Don't display training games
  if 'numTrain' in agentOpts:
    options.numQuiet = int(agentOpts['numTrain'])
    options.numIgnore = int(agentOpts['numTrain'])

  # Choose a ghost agent
  ghostType = loadAgent(options.ghost, noKeyboard)
  args['ghosts'] = [ghostType( i+1 ) for i in range( options.numGhosts )]

  # Choose a display format
  if options.quietGraphics:
      import textDisplay
      args['display'] = textDisplay.NullGraphics()
  elif options.textGraphics:
    import textDisplay
    textDisplay.SLEEP_TIME = options.frameTime
    args['display'] = textDisplay.PacmanGraphics()
  else:
    import gui
    args['display'] = gui.PacmanGraphics(options.zoom, frameTime = options.frameTime)
  args['numGames'] = options.numGames
  args['record'] = options.record
  args['catchExceptions'] = options.catchExceptions
  args['timeout'] = options.timeout

  # Special case: recorded games don't use the runGames method or args structure
  if options.gameToReplay != None:
    print(('Replaying recorded game %s.' % options.gameToReplay))
    import pickle
    f = open(options.gameToReplay)
    try: recorded = pickle.load(f)
    finally: f.close()
    recorded['display'] = args['display']
    replayGame(**recorded)
    sys.exit(0)

  return args

def loadAgent(pacman, nographics):
  # Looks through all pythonPath Directories for the right module,
  pythonPathStr = os.path.expandvars("$PYTHONPATH")
  if pythonPathStr.find(';') == -1:
    pythonPathDirs = pythonPathStr.split(':')
  else:
    pythonPathDirs = pythonPathStr.split(';')
  pythonPathDirs.append('.')

  for moduleDir in pythonPathDirs:
    if not os.path.isdir(moduleDir): continue
    moduleNames = [f for f in os.listdir(moduleDir) if f.endswith('gents.py') or f=='hw02.py']
    for modulename in moduleNames:
      try:
        module = __import__(modulename[:-3])
      except ImportError:
        continue
      if pacman in dir(module):
        if nographics and modulename == 'HumanAgents.py':
          raise Exception('Using the keyboard requires graphics (not text display)')
        return getattr(module, pacman)
  raise Exception('The agent ' + pacman + ' is not specified in any *Agents.py.')

def replayGame( layout, actions, display ):
    import hw02, ghostAgents
    rules = ClassicGameRules()
    # If replaying, change the agent from ExpectimaxAgent to whatever agent with which you want to play
    agents = [hw02.ExpectimaxAgent()] + [ghostAgents.RandomGhost(i + 1) for i in range(layout.getNumGhosts())]
    game = rules.newGame( layout, agents[0], agents[1:], display )
    state = game.state
    display.initialize(state.data)

    for action in actions:
      # Execute the action
      state = state.generateSuccessor( *action )
      # Change the display
      display.update( state.data )
      # Allow for game specific conditions (winning, losing, etc.)
      rules.process(state, game)

    display.finish()

def runGames( layout, pacman, ghosts, display, numGames, record, numTraining = 0, catchExceptions=False, timeout=30 ):
  import __main__
  __main__.__dict__['_display'] = display

  rules = ClassicGameRules(timeout)
  games = []
  sub_time=0
  total_time=0

  for i in range( numGames ):
    beQuiet = i < numTraining
    if beQuiet:
        # Suppress output and graphics
        import textDisplay
        gameDisplay = textDisplay.NullGraphics()
        rules.quiet = True
    else:
        gameDisplay = display
        rules.quiet = False
    game = rules.newGame(layout, pacman, ghosts, gameDisplay, beQuiet, catchExceptions)

    total_time+=game.run(sub_time)

    if not beQuiet: games.append(game)

    if record:
      import time, pickle
      fname = ('recorded-game-%d' % (i + 1)) +  '-'.join([str(t) for t in time.localtime()[1:6]])
      f = file(fname, 'w')
      components = {'layout': layout, 'actions': game.moveHistory}
      pickle.dump(components, f)
      f.close()

  if (numGames-numTraining) > 0:

    scores = [game.state.getScore() for game in games]
    wins = [game.state.isWin() for game in games]
    winRate = wins.count(True)/ float(len(wins))
    print('-----------Game Results--------------')
    print('Average Score:', sum(scores) / float(len(scores)))
    print('Score Results:', ', '.join([str(score) for score in scores]))
    print('Record:', ', '.join([ ['Lose', 'Win'][int(w)] for w in wins]))
    print('')
    print(("Win Rate: %d%% (%d/%d)" % (winRate * 100, wins.count(True), len(wins))))

    print('Total Time:', total_time)
    print('Average Time:',total_time/numGames)

    print("======================================")

  return games

if __name__ == '__main__':
  """
  The main function called when pacman.py is run
  from the command line:

  > python pacman.py

  See the usage string for more details.

  > python pacman.py --help
  """
  args = readCommand( sys.argv[1:] ) # Get game components based on input
  runGames( **args )

  pass
