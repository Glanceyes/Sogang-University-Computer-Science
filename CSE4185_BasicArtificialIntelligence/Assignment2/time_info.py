
import datetime, signal, os
import gc


class TimeoutFunctionException(Exception):
    pass
class TimeoutFunction:
    def __init__(self, function, maxSeconds):
        self.maxSeconds = maxSeconds
        self.function = function

    def handle_maxSeconds(self):
        print('TIMEOUT!')
        raise TimeoutFunctionException()

    def __call__(self, *args):
        if os.name == 'nt':
            timeStart = datetime.datetime.now()
            result = self.function(*args)
            timeEnd = datetime.datetime.now()
            if timeEnd - timeStart > datetime.timedelta(seconds=self.maxSeconds + 1):
                raise TimeoutFunctionException()
            return result
        old = signal.signal(signal.SIGALRM, self.handle_maxSeconds)
        signal.alarm(self.maxSeconds + 1)
        result = self.function(*args)
        signal.alarm(0)
        return result

class Part:
    def __init__(self, number, gradeFunc, maxSeconds,description):

        self.number = number
        self.description = description
        self.gradeFunc = gradeFunc
        self.maxSeconds = maxSeconds

        self.failed = False

    def fail(self):
        self.failed = True


class TimeCheck:
    def __init__(self):
        self.parts = []
        self.fatalError = False

    def addTest(self, agentname, gradeFunc, maxSeconds,description=""):

        self.assertNewNumber(agentname)
        part = Part(agentname, gradeFunc,  maxSeconds,description)
        self.parts.append(part)

    def assertNewNumber(self, number):
        if number in [part.number for part in self.parts]:
            raise Exception("Part number %s already exists" % number)


    def load(self, moduleName):
        try:
            return __import__(moduleName)
        except Exception as e:
            self.fail("Threw exception when importing '%s': %s" % (moduleName, e))
            self.fatalError = True
            return None
        except:
            self.fail("Threw exception when importing '%s'" % moduleName)
            self.fatalError = True
            return None

    def timePart(self, part):

        print('----- START PART  %s: ' % (part.number))
        print()

        try:
            TimeoutFunction(part.gradeFunc, part.maxSeconds)()
        except KeyboardInterrupt:
            raise
        except MemoryError as e:
            signal.alarm(0)
            gc.collect()
            self.fail('Memory limit exceeded.')
        except TimeoutFunctionException as e:
            signal.alarm(0)
            self.fail('Time limit (%s seconds) exceeded.' % part.maxSeconds)
        except Exception as e:
            signal.alarm(0)
            self.fail('Exception thrown: %s -- %s' % (str(type(e)), str(e)))
        except SystemExit as e:
            self.fail('Unexpected exit.')

        print('----- END  %s ' % (part.number))
        print('')

    def start(self):

        #parts=[]

        #for part in self.parts:
            #parts.append(part)
        print()
        print('=============================== START TIME CHECK ========================================')
        for part in self.parts:
            self.timePart(part)