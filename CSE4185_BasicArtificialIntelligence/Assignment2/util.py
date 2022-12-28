import sys
import inspect
import heapq, random


class Stack:

  def __init__(self):
    self.list = []
    
  def push(self,item):

    self.list.append(item)

  def pop(self):

    return self.list.pop()

  def isEmpty(self):

    return len(self.list) == 0

class Queue:

  def __init__(self):
    self.list = []
  
  def push(self,item):

    self.list.insert(0,item)

  def pop(self):

    return self.list.pop()

  def isEmpty(self):

    return len(self.list) == 0
  
class PriorityQueue:

  def  __init__(self):  
    self.heap = []
    
  def push(self, item, priority):
      pair = (priority,item)
      heapq.heappush(self.heap,pair)

  def pop(self):
      (priority,item) = heapq.heappop(self.heap)
      return item
  
  def isEmpty(self):
    return len(self.heap) == 0

class PriorityQueueWithFunction(PriorityQueue):

  def  __init__(self, priorityFunction):

    self.priorityFunction = priorityFunction      # store the priority function
    PriorityQueue.__init__(self)        # super-class initializer
    
  def push(self, item):

    PriorityQueue.push(self, item, self.priorityFunction(item))

    
def manhattanDistance( xy1, xy2 ):

  return abs( xy1[0] - xy2[0] ) + abs( xy1[1] - xy2[1] )



class Counter(dict):
  """

    
  >>> a = Counter()
  >>> print a['test']
  0


  >>> a = Counter()
  >>> a['test'] = 2
  >>> print a['test']
  2
  

  >>> a['blah'] += 1
  >>> print a['blah']
  1

  """
  def __getitem__(self, idx):
    self.setdefault(idx, 0)
    return dict.__getitem__(self, idx)

  def incrementAll(self, keys, count):
    """

    
    >>> a = Counter()
    >>> a.incrementAll(['one','two', 'three'], 1)
    >>> a['one']
    1
    >>> a['two']
    1
    """
    for key in keys:
      self[key] += count
  
  def argMax(self):

    if len(list(self.keys())) == 0: return None
    all = list(self.items())
    values = [x[1] for x in all]
    maxIndex = values.index(max(values))
    return all[maxIndex][0]
  
  def sortedKeys(self):
    """
    
    >>> a = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> a['third'] = 1
    >>> a.sortedKeys()

    """
    sortedItems = list(self.items())
    compare = lambda x, y:  sign(y[1] - x[1])
    sortedItems.sort(cmp=compare)
    return [x[0] for x in sortedItems]
  
  def totalCount(self):

    return sum(self.values())
  
  def normalize(self):

    total = float(self.totalCount())
    if total == 0: return
    for key in list(self.keys()):
      self[key] = self[key] / total
      
  def divideAll(self, divisor):
    """
    Divides all counts by divisor
    """
    divisor = float(divisor)
    for key in self:
      self[key] /= divisor

  def copy(self):

    return Counter(dict.copy(self))
  
  def __mul__(self, y ):
    """

    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['second'] = 5
    >>> a['third'] = 1.5
    >>> a['fourth'] = 2.5
    >>> a * b
    14
    """
    sum = 0
    x = self
    if len(x) > len(y):
      x,y = y,x
    for key in x:
      if key not in y:
        continue
      sum += x[key] * y[key]      
    return sum
      
  def __radd__(self, y):
    """

    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['third'] = 1
    >>> a += b
    >>> a['first']
    1
    """ 
    for key, value in list(y.items()):
      self[key] += value   
      
  def __add__( self, y ):
    """
    
    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['third'] = 1
    >>> (a + b)['first']
    1
    """
    addend = Counter()
    for key in self:
      if key in y:
        addend[key] = self[key] + y[key]
      else:
        addend[key] = self[key]
    for key in y:
      if key in self:
        continue
      addend[key] = y[key]
    return addend
    
  def __sub__( self, y ):
    """
    
    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['third'] = 1
    >>> (a - b)['first']
    -5
    """      
    addend = Counter()
    for key in self:
      if key in y:
        addend[key] = self[key] - y[key]
      else:
        addend[key] = self[key]
    for key in y:
      if key in self:
        continue
      addend[key] = -1 * y[key]
    return addend
    
def raiseNotDefined():
  print(("Method not implemented: %s" % inspect.stack()[1][3]))    
  sys.exit(1)

def normalize(vectorOrCounter):

  normalizedCounter = Counter()
  if type(vectorOrCounter) == type(normalizedCounter):
    counter = vectorOrCounter
    total = float(counter.totalCount())
    if total == 0: return counter
    for key in list(counter.keys()):
      value = counter[key]
      normalizedCounter[key] = value / total
    return normalizedCounter
  else:
    vector = vectorOrCounter
    s = float(sum(vector))
    if s == 0: return vector
    return [el / s for el in vector]
                
def nSample(distribution, values, n):
  if sum(distribution) != 1:
    distribution = normalize(distribution)
  rand = [random.random() for i in range(n)]
  rand.sort()
  samples = []
  samplePos, distPos, cdf = 0,0, distribution[0]
  while samplePos < n:
    if rand[samplePos] < cdf:
      samplePos += 1
      samples.append(values[distPos])
    else:
      distPos += 1
      cdf += distribution[distPos]
  return samples
    
def sample(distribution, values = None):
  if type(distribution) == Counter: 
    items = list(distribution.items())
    distribution = [i[1] for i in items] 
    values = [i[0] for i in items] 
  if sum(distribution) != 1:
    distribution = normalize(distribution)
  choice = random.random()
  i, total= 0, distribution[0]
  while choice > total:
    i += 1
    total += distribution[i]
  return values[i]

def sampleFromCounter(ctr):
  items = list(ctr.items())
  return sample([v for k,v in items], [k for k,v in items])

def getProbability(value, distribution, values):

  total = 0.0
  for prob, val in zip(distribution, values):
    if val == value:
      total += prob
  return total

def flipCoin( p ):
  r = random.random()
  return r < p 

def chooseFromDistribution( distribution ):

  if type(distribution) == dict or type(distribution) == Counter:
    return sample(distribution)
  r = random.random()
  base = 0.0
  for prob, element in distribution:
    base += prob
    if r <= base: return element
    
def nearestPoint( pos ):

  ( current_row, current_col ) = pos

  grid_row = int( current_row + 0.5 ) 
  grid_col = int( current_col + 0.5 ) 
  return ( grid_row, grid_col )     

def sign( x ):

  if( x >= 0 ):
    return 1
  else:
    return -1

def arrayInvert(array):

  result = [[] for i in array]
  for outer in array:
    for inner in range(len(outer)):
      result[inner].append(outer[inner])
  return result

def matrixAsList( matrix, value = True ):

  rows, cols = len( matrix ), len( matrix[0] )
  cells = []
  for row in range( rows ):
    for col in range( cols ):
      if matrix[row][col] == value:
        cells.append( ( row, col ) )
  return cells

def lookup(name, namespace):

  dots = name.count('.')
  if dots > 0:
    moduleName, objName = '.'.join(name.split('.')[:-1]), name.split('.')[-1]
    module = __import__(moduleName)
    return getattr(module, objName)
  else:
    modules = [obj for obj in list(namespace.values()) if str(type(obj)) == "<type 'module'>"]
    options = [getattr(module, name) for module in modules if name in dir(module)]
    options += [obj[1] for obj in list(namespace.items()) if obj[0] == name ]
    if len(options) == 1: return options[0]
    if len(options) > 1: raise Exception('Name conflict for %s')
    raise Exception('%s not found as a method or class' % name)

def pause():

  print("<Press enter/return to continue>")
  eval(input())
  
  

import signal
class TimeoutFunctionException(Exception):
    pass

class TimeoutFunction:

    def __init__(self, function, timeout):
        self.timeout = timeout
        self.function = function

    def handle_timeout(self, signum, frame):
        raise TimeoutFunctionException()

    def __call__(self, *args):
        if not 'SIGALRM' in dir(signal):
            return self.function(*args)
        old = signal.signal(signal.SIGALRM, self.handle_timeout)
        signal.alarm(self.timeout)
        try:
            result = self.function(*args)
        finally:
            signal.signal(signal.SIGALRM, old)
        signal.alarm(0)
        return result
