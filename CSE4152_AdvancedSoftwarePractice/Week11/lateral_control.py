import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
from scipy.interpolate import splprep, splev
from scipy.optimize import minimize
import time


class LateralController:
    '''
    Lateral control using the Stanley controller

    functions:
        stanley 

    init:
        gain_constant (default=5)
        damping_constant (default=0.5)
    '''


    def __init__(self, gain_constant=1, damping_constant=0.02):

        self.gain_constant = gain_constant
        self.damping_constant = damping_constant
        self.previous_steering_angle = 0


    def stanley(self, waypoints, speed):
        '''
        ##### TODO #####
        one step of the stanley controller with damping
        args:
            waypoints (np.array) [2, num_waypoints]
            speed (float)
        '''

        # derive orientation error as the angle of the first path segment to the car orientation

        # derive stanley control law
        # derive cross track error as distance between desired waypoint at spline parameter equal zero ot the car position
        # prevent division by zero by adding as small epsilon 
        # derive damping
        
        # clip to the maximum stering angle (0.4) and rescale the steering action space

        """
        crosstrack error : waypoints의 맨 처음값 - (48,0)
        heading error : waypoints의 첫 번째 인덱스 값과 그 다음 인덱스에 해당되는 값 사용
        """

        k_constant = self.gain_constant
        epsilon = 1e-6

        crosstrack_error = waypoints[0, 0] - 48
        heading_error = np.arctan((waypoints[0, 1] - waypoints[0, 0]) / (waypoints[1, 1] - waypoints[1, 0]))

        steering_angle = heading_error + np.arctan((k_constant * crosstrack_error) / (speed + epsilon))
        
        damping = self.damping_constant * (steering_angle - self.previous_steering_angle)
        # Not necessary
        # absolute_steering_angle = steering_angle if steering_angle > 0 else -steering_angle
        # damping = np.clip(damping, -absolute_steering_angle - 0.1 * steering_angle, absolute_steering_angle + 0.1 * steering_angle)
        steering_angle = steering_angle - damping
        # steering_angle = steering_angle + damping

        steering_angle = np.clip(steering_angle, -0.4, 0.4)
        steering_angle = steering_angle / 0.4

        self.previous_steering_angle = steering_angle

        return steering_angle




