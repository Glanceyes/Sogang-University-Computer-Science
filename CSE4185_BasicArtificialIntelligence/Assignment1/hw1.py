############# Write Your Library Here ###########
from itertools import permutations, combinations
from collections import defaultdict, deque
from math import inf
from maze import Maze
import queue
import copy
################################################


def search(maze, func):
    return {
        "bfs": bfs,
        "dfs":dfs,
        "astar": astar,
        "astar_four_circles": astar_four_circles,
        "astar_many_circles": astar_many_circles
    }.get(func)(maze)


def bfs(maze: Maze):
    """
    [Problem 01] 제시된 stage1 맵 세 가지를 BFS Algorithm을 통해 최단 경로를 return하시오.
    """
    start_point=maze.startPoint()
    path=[]
    ####################### Write Your Code Here ################################

    # Breath First Search Algorithm

    queue = list()
    visited = set()
    parent = dict()

    queue.append(start_point)
    visited.add(start_point)

    while queue:
        current = queue.pop(0)

        # Find the objective
        if maze.isObjective(current[0], current[1]):
            path.append(current)

            while path[-1] != start_point:
                path.append(parent[path[-1]])
            path.reverse()

            maze.isValidPath(path)
            return path

        for neighbor in maze.neighborPoints(current[0], current[1]):
            if neighbor not in visited:
                queue.append(neighbor)
                visited.add(neighbor)
                parent[neighbor] = current

    return path
    ############################################################################


def dfs(maze: Maze):
    """
    [Problem 02] 제시된 stage1 맵 세 가지를 DFS Algorithm을 통해 최단 경로를 return하시오.
    """
    start_point = maze.startPoint()
    path = []
    ####################### Write Your Code Here ################################

    # Depth First Search Algorithm by recursion
    cost = dict()
    visited = set()
    parent = dict()

    end_point = maze.circlePoints()[0]

    # Depth First Search function for finding the shortest path
    def dfs_shortest(current, current_cost):

        visited.add(current)

        if maze.isObjective(current[0], current[1]):   
            return

        for neighbor in maze.neighborPoints(current[0], current[1]):
            if neighbor not in visited:
                if (neighbor not in cost or cost[neighbor] > current_cost + 1):
                    cost[neighbor] = current_cost + 1
                    parent[neighbor] = current
                    dfs_shortest(neighbor, current_cost + 1)

        visited.discard(current)
        return False

    dfs_shortest(start_point, 0)


    # Find the shortest path
    path.append(end_point)

    while path[-1] != start_point:
        path.append(parent[path[-1]])
    path.reverse()

    maze.isValidPath(path)
    return path

    ############################################################################



def manhattan_dist(p1, p2):
    return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1])


def astar(maze: Maze):
    """
    [Problem 03] 제시된 stage1 맵 세가지를 A* Algorithm을 통해 최단경로를 return하시오.
    (Heuristic Function은 위에서 정의한 manhattan_dist function을 사용할 것.)
    """
    start_point = maze.startPoint()
    path = []
    ####################### Write Your Code Here ################################

    # A* Algorithm
    visited = set()
    parent = dict()

    end_point = maze.circlePoints()[0]

    priority_queue = queue.PriorityQueue()
    start_node = (manhattan_dist(start_point, end_point), 0, start_point)
    visited.add(start_point)
    priority_queue.put(start_node)

    while priority_queue:
        current = priority_queue.get()
        current_cost = current[1]
        current_point = current[2]
        
        if maze.isObjective(current_point[0], current_point[1]):
            path.append(current_point)

            while path[-1] != start_point:
                path.append(parent[path[-1]])
            path.reverse()

            maze.isValidPath(path)
            return path

        for neighbor in maze.neighborPoints(current_point[0], current_point[1]):
            if neighbor not in visited:
                visited.add(neighbor)
                parent[neighbor] = current_point
                priority_queue.put((current_cost + 1 + manhattan_dist(neighbor, end_point), current_cost + 1, neighbor))

    return path
    ############################################################################



def stage2_heuristic(current, points_left):
    if len(points_left) == 0:
        return 0

    min_cost = inf

    for points_left_seq in list(permutations(points_left)):
        cost = manhattan_dist(current, points_left_seq[0])
        for i in range(len(points_left_seq) - 1):
            cost += manhattan_dist(points_left_seq[i], points_left_seq[i + 1])
        if cost < min_cost:
            min_cost = cost

    return min_cost


def astar_four_circles(maze: Maze):
    """
    [Problem 04] 제시된 stage2 맵 세 가지를 A* Algorithm을 통해 최단경로를 return하시오.
    (Heuristic Function은 직접 정의할것 )
    """
    start_point = maze.startPoint()
    path = []
    ####################### Write Your Code Here ################################


    # A* Algorithm for visiting four circles

    visited = dict()
    end_points = maze.circlePoints()

    points_to_visit = set(end_points)
    points_to_visit.add(start_point)
    visited_points = set()
    visited_points.add(start_point)

    priority_queue = queue.PriorityQueue()
    visited_points.add(start_point)
    heuristic_cost = stage2_heuristic(start_point, points_to_visit - visited_points)
    visited[(start_point, tuple(sorted(list(visited_points))))] = heuristic_cost
    priority_queue.put((heuristic_cost, (0, start_point, visited_points, [start_point])))

    while priority_queue:
        _, current = priority_queue.get()
        current_cost = current[0]
        current_point = current[1]
        current_visited_points: set = current[2]
        current_path: list = current[3]

        if current_visited_points == points_to_visit:
            path = current_path
            return path

        for neighbor in maze.neighborPoints(current_point[0], current_point[1]):
            next_visited_points = copy.deepcopy(current_visited_points)

            # If the neighbor is an end point that has not been visited
            if neighbor in (points_to_visit - current_visited_points):
                next_visited_points.add(neighbor)

            next_visited_points_list = tuple(sorted(list(next_visited_points)))
            next_heuristic_cost = stage2_heuristic(neighbor, points_to_visit - next_visited_points)
            next_cost = current_cost + 1 + next_heuristic_cost

            if (neighbor, next_visited_points_list) not in visited or visited[neighbor, next_visited_points_list] > next_cost:
                visited[(neighbor, next_visited_points_list)] = next_cost
                priority_queue.put((next_cost, (current_cost + 1, neighbor, next_visited_points, current_path + [neighbor])))
                
    return path

    ############################################################################



# -------------------- Stage 03: Many circles - A* Algorithm -------------------- #
def mst(points_left):
    # Minimum Spanning Tree Algorithm
    # Prim's Algorithm

    mst_cost = 0
    selected = set()
    candidates = defaultdict(list)

    if len(points_left) == 0:
        return 0

    points_left_tuple = tuple(sorted(points_left))

    if points_left_tuple in cached_cost:
        return cached_cost[points_left_tuple]

    priority_queue = queue.PriorityQueue()

    for pair in combinations(points_left, 2):
        weight = edges[pair[0], pair[1]].weight
        candidates[pair[0]].append((weight, pair[1]))
        candidates[pair[1]].append((weight, pair[0]))

    selected.add(points_left[0])

    for neighbor in candidates[points_left[0]]:
        priority_queue.put(neighbor)

    while not priority_queue.empty():
        weight, point = priority_queue.get()

        if point not in selected:
            selected.add(point)
            mst_cost += weight

            for neighbor in candidates[point]:
                if neighbor[1] not in selected:
                    priority_queue.put(neighbor)

    cached_cost[points_left_tuple] = mst_cost
    return cached_cost[points_left_tuple]


def astar_many_circles(maze: Maze):
    """
    [Problem 04] 제시된 stage3 맵 다섯 가지를 A* Algorithm을 통해 최단 경로를 return하시오.
    (Heuristic Function은 직접 정의 하고, minimum spanning tree를 활용하도록 한다.)
    """
    start_point = maze.startPoint()
    path = []
    ####################### Write Your Code Here ################################
    global cached_cost, edges
    cached_cost = dict()
    edges = dict()

    class Node:
        def __init__(self, current_point, end_points, cost, heuristic, parent):
            self.current_point = current_point
            self.end_points = end_points
            self.parent = parent
            self.total_cost = cost + heuristic
            self.cost = cost
            
        def __lt__(self, a):
            return self.total_cost < a.total_cost

        def __le__(self, a):
            return self.total_cost <= a.total_cost

        def __gt__(self, a):
            return self.total_cost > a.total_cost

        def __ge__(self, a):
            return self.total_cost >= a.total_cost

    class Edge:
        def __init__(self, start, end, weight, path):
            self.start = start
            self.end = end
            self.weight = weight
            self.path = path

        def __lt__(self, a):
            return self.weight < a.weight

        def __le__(self, a):
            return self.weight <= a.weight

        def __gt__(self, a):
            return self.weight > a.weight

        def __ge__(self, a):
            return self.weight >= a.weight


    def initialize_path(start_point):
        path_list = list()
        for row in range(maze.rows):
            path_list.append([])
            for _ in range(maze.cols):
                path_list[row].append([])
        path_list[start_point[0]][start_point[1]] = [list(start_point)]
        return path_list


    def find_path(start_point, end_point):
        candidates = deque([start_point])
        path = initialize_path(start_point)

        while len(candidates) > 0:
            row, col = candidates.popleft()

            if (row, col) == end_point:
                return path[row][col]
            
            for neighbor in maze.neighborPoints(row, col):
                if path[neighbor[0]][neighbor[1]]:
                    continue
                
                next = [[neighbor[0], neighbor[1]]]
                if path[row][col]:
                    path[neighbor[0]][neighbor[1]] = path[row][col] + next
                else:
                    path[neighbor[0]][neighbor[1]] = next
                
                candidates.append([neighbor[0], neighbor[1]])

        return list()


    def set_edges(start_point, points_left):
        for point in points_left:
            if edges.get((start_point, point)) is None:
                path = find_path(start_point, point)
                edges[start_point, point] = Edge(start_point, point, len(path), path)
                # Add the reverse edge
                edges[point, start_point] = Edge(point, start_point, len(path), path[::-1])

        for pair in combinations(points_left, 2):
            if edges.get((pair[0], pair[1])) is None:
                path = find_path(pair[0], pair[1])
                edges[pair[0], pair[1]] = Edge(pair[0], pair[1], len(path), path)
                # Add the reverse edge
                edges[pair[1], pair[0]] = Edge(pair[1], pair[0], len(path), path[::-1])
            
        return

    visited = dict()
    end_points = maze.circlePoints()

    set_edges(start_point, end_points)

    priority_queue = queue.PriorityQueue()
    heuristic_cost = mst(end_points)
    start_node = Node(start_point, end_points, 0, heuristic_cost, None)
    priority_queue.put(start_node)
    visited[start_node] = True

    while priority_queue.qsize() > 0:
        current_node: Node = priority_queue.get()

        # If all end points are visited
        if len(current_node.end_points) == 0:
            node = current_node
            while node.parent:
                path += edges[node.current_point, node.parent.current_point].path[:-1]
                node = node.parent
            path.append(start_point)
            return path[::-1]

        for end_point in current_node.end_points:
            next_cost = current_node.cost + edges[current_node.current_point, end_point].weight
            next_end_points = copy.deepcopy(current_node.end_points)
            next_end_points.remove(end_point)
            next_heuristic_cost = mst(next_end_points)

            next_node = Node(end_point, next_end_points, next_cost, next_heuristic_cost, current_node)
            
            if next_node not in visited:
                priority_queue.put(next_node)
                visited[next_node] = True

    return path