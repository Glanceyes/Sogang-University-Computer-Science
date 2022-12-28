import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
from scipy.interpolate import splprep, splev
from scipy.optimize import minimize
import time
import sys


def normalize(v):
    norm = np.linalg.norm(v,axis=0) + 1e-5
    return v / norm.reshape(1, v.shape[1])

def curvature(waypoints):
    '''
    ##### TODO #####
    Curvature as  the sum of the normalized dot product between the way elements
    Implement second term of the smoothin objective.

    args: 
        waypoints [2, num_waypoints] !!!!!
    '''
    
    norm_diff = normalize(np.diff(waypoints, axis=1))
    curvature = np.sum(norm_diff[:,:-1] * norm_diff[:,1:])
    
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


def waypoint_prediction(roadside1_spline, roadside2_spline, num_waypoints=6, way_type = "center"):
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
    ##### TODO #####
 
    # create spline arguments
    t = np.linspace(0, 1, num_waypoints)

    # derive roadside points from spline
    roadside1_points = np.array(splev(t, roadside1_spline))
    roadside2_points = np.array(splev(t, roadside2_spline))

    # derive center between corresponding roadside points
    way_points = np.array(roadside1_points + roadside2_points)/2

    # output way_points with shape(2 x Num_waypoints)
    return way_points


def target_speed_prediction(waypoints, num_waypoints_used=5,
                            max_speed=60, exp_constant=4.5, offset_speed=30):
    '''
    ##### TODO #####
    Predict target speed given waypoints
    Implement the function using curvature()

    args:
        waypoints [2,num_waypoints]
        num_waypoints_used (default=5)
        max_speed (default=60)
        exp_constant (default=4.5)
        offset_speed (default=30)
    
    output:
        target_speed (float)
    '''
    
    curv_center = abs(num_waypoints_used - 2 - curvature(waypoints[:,:num_waypoints_used]))
    target_speed = (max_speed - offset_speed)* np.exp(- exp_constant * curv_center) + offset_speed

    return target_speed