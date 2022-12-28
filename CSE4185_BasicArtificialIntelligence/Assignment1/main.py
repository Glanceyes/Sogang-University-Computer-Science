# --------- GUI Setting File --------- #

import pygame
import argparse
import time

from pygame.locals import *
from agent import Agent
from maze import Maze
from hw1 import search

class Pygame_APP:
    def __init__(self, keyboard=True, scale=20, speed=30):
        self.running = True
        self.displaySurface = None
        self.scale = scale
        self.speed = speed
        self.windowTitle = "2022_02 AI Assignment 01:"
        self.__keyboard = keyboard

    def initialize(self, filename):
        self.windowTitle += filename

        self.maze = Maze(filename)
        self.gridDim = self.maze.getDimensions()

        self.windowHeight = self.gridDim[0] * self.scale
        self.windowWidth = self.gridDim[1] * self.scale

        self.blockSizeX = int(self.windowWidth / self.gridDim[1])
        self.blockSizeY = int(self.windowHeight / self.gridDim[0])

        if self.__keyboard:
            self.agentRadius = min(self.blockSizeX, self.blockSizeY) / 4
            self.agent = Agent(self.maze.startPoint(), self.maze, self.blockSizeX, self.blockSizeY)

    def start(self, filename, method, save):
        self.initialize(filename)

        if self.maze is None:
            print("No maze created")
            raise SystemExit

        if not self.__keyboard:
            start_time = time.time()
            path = search(self.maze, method)
            total_time = time.time()-start_time
            statesSearch = self.maze.getStatesSearch()
        else:
            path, statesSearch = [], 0

        pygame.init()
        self.displaySurface = pygame.display.set_mode((self.windowWidth, self.windowHeight), pygame.HWSURFACE)
        self.displaySurface.fill((255, 255, 255))
        pygame.display.flip()
        pygame.display.set_caption(self.windowTitle)

        if self.__keyboard:
            self.drawPlayer()
        else:
            print("==========================================")
            print("[", method, "results ]")
            print("(1) Path Length:", len(path))
            print("(2) Search States:", statesSearch)
            print("(3) Execute Time:",format(total_time,".10f"),"seconds")
            print("==========================================")
            self.drawPath(path)

        self.drawMaze()
        self.drawStart()
        self.drawObjective()

        pygame.display.flip()
        if save is not None:
            pygame.image.save(self.displaySurface, save)
            self.running = False

        clock = pygame.time.Clock()

        while self.running:
            pygame.event.pump()
            keys = pygame.key.get_pressed()
            clock.tick(self.speed)

            if (keys[K_ESCAPE]):
                    raise SystemExit

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    raise SystemExit

            if self.__keyboard:
                if (keys[K_RIGHT]):
                    self.agent.moveRight()

                if (keys[K_LEFT]):
                    self.agent.moveLeft()

                if (keys[K_UP]):
                    self.agent.moveUp()

                if (keys[K_DOWN]):
                    self.agent.moveDown()

                self.gameLoop()


    def gameLoop(self):
        self.drawObjective()
        self.drawPlayer()
        self.agent.update()
        pygame.display.flip()

    def getColor(self, pathLength, index):

        start_color = (252,181, 122)
        end_color = (107,107, 182)

        r_step = (end_color[0] - start_color[0]) / pathLength
        g_step = (end_color[1] - start_color[1]) / pathLength
        b_step = (end_color[2] - start_color[2]) / pathLength

        red = start_color[0] + index * r_step
        green = start_color[1] + index * g_step
        blue = start_color[2] + index * b_step

        return (red, green, blue)

    def drawPath(self, path):
        for p in range(len(path)):
            color = self.getColor(len(path), p)
            self.drawSquare(path[p][0], path[p][1], color)

    def drawWall(self, row, col):
        pygame.draw.rect(self.displaySurface, (86, 86, 86),
                         (col * self.blockSizeX, row * self.blockSizeY, self.blockSizeX, self.blockSizeY), 0)

    def drawCircle(self, row, col, color, radius=None):
        if radius is None:
            radius = min(self.blockSizeX, self.blockSizeY) / 4
        pygame.draw.circle(self.displaySurface, color, (int(col * self.blockSizeX + self.blockSizeX / 2), int(row * self.blockSizeY + self.blockSizeY / 2)), int(radius))

    def drawSquare(self, row, col, color):
        pygame.draw.rect(self.displaySurface, color, (col * self.blockSizeX, row * self.blockSizeY, self.blockSizeX, self.blockSizeY), 0)

    def drawPlayer(self):
        if self.agent.lastRow is not None and self.agent.lastCol is not None:
            self.drawCircle(self.agent.lastRow, self.agent.lastCol, (0, 0, 255))
        self.drawCircle(self.agent.row, self.agent.col, self.agent.color)

    def drawObjective(self):
        for obj in self.maze.circlePoints():
            self.drawCircle(obj[0], obj[1], (0, 102, 0))

    def drawStart(self):
        row,col = self.maze.startPoint()
        pygame.draw.rect(self.displaySurface, (0,0,255), (col * self.blockSizeX + self.blockSizeX/4, row * self.blockSizeY + self.blockSizeY/4, self.blockSizeX * 0.5, self.blockSizeY * 0.5), 0)

    def drawMaze(self):
        for row in range(self.gridDim[0]):
            for col in range(self.gridDim[1]):
                if self.maze.isWall(row, col):
                    self.drawWall(row, col)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='CSE4185 Assignment01: Maze Game')

    parser.add_argument('filename',
                        help='path to maze file [REQUIRED]')
    parser.add_argument('--method', dest="search", type=str, default = "bfs",
                        choices = ["bfs", "dfs","astar", "astar_four_circles", "astar_many_circles"],
                        help='search method - default bfs')
    parser.add_argument('--scale', dest="scale", type=int, default = 20,
                        help='scale - default: 20')
    parser.add_argument('--speed', dest="speed", type=int, default = 15,
                        help='speed for the display - default 15')
    parser.add_argument('--keyboard', default = False, action = "store_true",
                        help='you can play the game - default False')
    parser.add_argument('--save', dest="save", type=str, default = None,
                        help='save output to image file - default not saved')


    args = parser.parse_args()
    app = Pygame_APP(args.keyboard, args.scale, args.speed)
    app.start(args.filename, args.search, args.save)