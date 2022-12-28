import gym
from gym.envs.box2d.car_racing import CarRacing

from lane_detection import LaneDetection
from waypoint_prediction import waypoint_prediction, target_speed_prediction
import matplotlib.pyplot as plt
import numpy as np
import pyglet
from pyglet import gl
from pyglet.window import key
import pygame

# action variables
action = np.array([0.0, 0.0, 0.0])
def register_input():

    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                action[0] = -1.0
            if event.key == pygame.K_RIGHT:
                action[0] = +1.0
            if event.key == pygame.K_UP:
                action[1] = +0.5
            if event.key == pygame.K_DOWN:
                action[2] = +0.8  # set 1.0 for wheels to block to zero rotation
            if event.key == pygame.K_r:
                global retry
                retry = True
            if event.key == pygame.K_s:
                global record
                record = True
            if event.key == pygame.K_q:
                global quit
                quit = True

        if event.type == pygame.KEYUP:
            if event.key == pygame.K_LEFT and action[0] < 0.0:
                action[0] = 0
            if event.key == pygame.K_RIGHT and action[0] > 0.0:
                action[0] = 0
            if event.key == pygame.K_UP:
                action[1] = 0
            if event.key == pygame.K_DOWN:
                action[2] = 0


# init environement
env = CarRacing()
env.render()
env.reset()

# define variables
total_reward = 0.0
steps = 0
restart = False

# init modules of the pipeline
LD_module = LaneDetection()

# init extra plot
fig = plt.figure()
plt.ion()
plt.show()

while True:
    # perform step
    register_input()
    s, r, done, speed = env.step(action)
    
    # lane detection
    lane1, lane2 = LD_module.lane_detection(s)

    # waypoint and target_speed prediction
    waypoints = waypoint_prediction(lane1, lane2)
    target_speed = target_speed_prediction(waypoints)

    # reward
    total_reward += r

    # outputs during training
    if steps % 2 == 0 or done:
        print("\naction " + str(["{:+0.2f}".format(x) for x in action]))
        print("step {} total_reward {:+0.2f}".format(steps, total_reward))

        LD_module.plot_state_lane(s, steps, fig, waypoints=waypoints)
        
    steps += 1
    env.render()

    # check if stop
    if done or restart or steps>=600: 
        print("step {} total_reward {:+0.2f}".format(steps, total_reward))
        break

env.close()