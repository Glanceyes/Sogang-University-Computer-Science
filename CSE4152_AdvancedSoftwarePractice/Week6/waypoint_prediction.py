import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
from scipy.interpolate import splprep, splev
from scipy.optimize import minimize
import time
import sys


def normalize(v):
    norm = np.linalg.norm(v,axis=0) + 0.00001
    return v / norm.reshape(1, v.shape[1])

def curvature(waypoints_t):
    '''
    ##### TODO #####
    Curvature as  the sum of the normalized dot product between the way elements
    Implement second term of the smoothin objective.

    args: 
        waypoints [2, num_waypoints] !!!!!
    '''

    '''
    Example)
        norm_diff = normalize(arguments)
        norm_diff.shape : (2, num_waypoints)
        
        curvature example:
                3.9999937500073246
        
    '''
    curvature = 0
    waypoints_t = np.transpose(waypoints_t)
    
    for i in range(1, waypoints_t.shape[0] - 2):
        next_now_diff = waypoints_t[i + 1] - waypoints_t[i]
        now_prev_diff = waypoints_t[i] - waypoints_t[i - 1]
        dot_product = np.dot(next_now_diff, now_prev_diff)
        norm_diff = np.linalg.norm(next_now_diff) * np.linalg.norm(now_prev_diff)
        curvature += dot_product / norm_diff

    return curvature


def smoothing_objective(waypoints, waypoints_center, weight_curvature=40):
    '''
    Objective for path smoothing

    args:
        waypoints [2 * num_waypoints] !!!!!
        waypoints_center [2 * num_waypoints] !!!!!
        weight_curvature (default=40)
    '''
    # mean least square error between waypoint and way point center
    ls_tocenter = np.mean((waypoints_center - waypoints)**2)

    # derive curvature
    curv = curvature(waypoints.reshape(2,-1))

    return -1 * weight_curvature * curv + ls_tocenter


def waypoint_prediction(roadside1_spline, roadside2_spline, num_waypoints=6, way_type = "smooth"):
    '''
    ##### TODO #####
    Predict waypoint via two different methods:
    - center
    - smooth 

    args:
        roadside1_spline
        roadside2_spline
        num_waypoints (default=6)
        parameter_bound_waypoints (default=1)
        waytype (default="smoothed")
    '''
    if way_type == "center":
        ##### TODO #####
     
        # create spline arguments
        '''
        Example)
            t = np.linspace(arguments)
            t.shape : (num_waypoints,)
        '''
        
        t = np.linspace(0, 1, num_waypoints)

        # derive roadside points from spline
        '''
        Example)
            roadside1_points = np.array(splev(arguments))
            roadside2_points = np.array(splev(arguments))
            roadside1_points.shape : (2, num_waypoints)
            roadside2_points.shape : (2, num_waypoints)
            roadside1_points example :
                    array([[37. , 37. , 37. , 37. , 37. , 37. ],
                           [ 0. , 12.8, 25.6, 38.4, 51.2, 64. ]])
            roadside2_points example :
                    array([[58. , 58. , 58. , 58. , 58. , 58. ],
                           [ 0. , 12.8, 25.6, 38.4, 51.2, 64. ]])
        '''

        roadside1_points = np.array(splev(t, roadside1_spline))
        roadside2_points = np.array(splev(t, roadside2_spline))

        # derive center between corresponding roadside points
        '''
        Example)
            way_points = np.array( {derive center between corresponding roadside points} )
            way_points.shape : (2, num_waypoints)
            way_points example :
                    array([[47.5, 47.5, 47.5, 47.5, 47.5, 47.5],
                           [ 0. , 12.8, 25.6, 38.4, 51.2, 64. ]])
        '''

        way_points = np.array([
            (roadside1_points[0] + roadside2_points[0]) / 2, 
            (roadside1_points[1] + roadside2_points[1]) / 2
        ])

        return way_points
    
    elif way_type == "smooth":
        ##### TODO #####

        # create spline points
        '''
        Example)
            t = np.linspace(arguments)
            t.shape : (num_waypoints,)
        '''
        
        t = np.linspace(0, 1, num_waypoints)

        # roadside points from spline
        '''
        Example)
            roadside1_points = np.array(splev(arguments))
            roadside2_points = np.array(splev(arguments))
            roadside1_points.shape : (2, num_waypoints)
            roadside2_points.shape : (2, num_waypoints)
            roadside1_points example :
                    array([[37. , 37. , 37. , 37. , 37. , 37. ],
                           [ 0. , 12.8, 25.6, 38.4, 51.2, 64. ]])
            roadside2_points example :
                    array([[58. , 58. , 58. , 58. , 58. , 58. ],
                           [ 0. , 12.8, 25.6, 38.4, 51.2, 64. ]])
        '''
        roadside1_points = np.array(splev(t, roadside1_spline))
        roadside2_points = np.array(splev(t, roadside2_spline))

        # print("roadside1_points : ", roadside1_points)
        # print("roadside2_points : ", roadside2_points)
        
        # center between corresponding roadside points
        '''
        Example)
            way_points_center = (np.array( {derive center between corresponding roadside points} )).reshape(-1)
            way_points_center.shape : (num_waypoints*2,)
            way_points_center example :
                    array([47.5, 47.5, 47.5, 47.5, 47.5, 47.5,  0. , 12.8, 25.6, 38.4, 51.2, 64. ])
        '''
        
        way_points_center = np.array([
            (roadside1_points[0] + roadside2_points[0]) / 2, 
            (roadside1_points[1] + roadside2_points[1]) / 2
        ])

        way_points_center = way_points_center.reshape(num_waypoints * 2)
        # print("way_points_center: ", way_points_center)

        # optimization
        '''
        scipy.optimize.minimize Doc.)
            https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.minimize.html
        
        Example)
            way_points = minimize(arguments)
            way_points.shape : (num_way_points*2,)
            way_points example :
                    array([47.5, 47.5, 47.5, 47.5, 47.5, 47.5,  0. , 12.8, 25.6, 38.4, 51.2, 64. ])
        '''

        way_points = minimize(smoothing_objective, way_points_center, args=way_points_center)["x"]

        # print("way_points: ", way_points)

        return way_points.reshape(2,-1)


def target_speed_prediction(waypoints, num_waypoints_used=5,
                            max_speed=60, exp_constant=4.5, offset_speed=30):
    '''
    ##### TODO #####
    Predict target speed given waypoints
    Implement the function using curvature()

    args:
        waypoints [2,num_waypoints]         for curv_center
        num_waypoints_used (default=5)      for curv_center
        max_speed (default=60)              for target_speed
        exp_constant (default=4.5)          for target_speed
        offset_speed (default=30)           for target_speed
    
    output:
        target_speed (float)
    '''

    '''
    Example)
        curv_center = ~~~
        target_speed = ~~~
    '''
    
    curv_center = curvature(waypoints)
    speed_factor = np.exp(-exp_constant * np.abs(num_waypoints_used - 2 - curv_center))
    target_speed = (max_speed - offset_speed) * speed_factor + offset_speed

    return target_speed