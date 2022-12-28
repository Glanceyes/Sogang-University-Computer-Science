import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
from scipy.interpolate import splprep, splev
from scipy.optimize import minimize
import time


class LaneDetection:
    '''
    Lane detection module using edge detection and b-spline fitting

    args:
        cut_size (cut_size=65) cut the image at the front of the car
        spline_smoothness (default=10)
        gradient_threshold (default=14)
        distance_maxima_gradient (default=3)

    '''

    def __init__(self, cut_size=65, spline_smoothness=10, gradient_threshold=14, distance_maxima_gradient=3):
        self.car_position = np.array([48,0])
        self.spline_smoothness = spline_smoothness
        self.cut_size = cut_size
        self.gradient_threshold = gradient_threshold
        self.distance_maxima_gradient = distance_maxima_gradient
        self.lane_boundary1_old = 0
        self.lane_boundary2_old = 0


    def cut_gray(self, state_image_full):
        '''
        ##### TODO #####
        This function should cut the image at the front end of the car (e.g. pixel row 65)
        and translate to gray scale

        input:
            state_image_full 96x96x3

        output:
            gray_state_image 65x96x1

        '''

        state_image = np.array(state_image_full)
        row = state_image.shape[0]
        col = state_image.shape[1]

        gray_state_image = np.zeros((row, col, 1), dtype=np.uint8)
        # gray_state_image = np.dot(state_image[...,:3], [0.2989, 0.5870, 0.1140])

        gray_state_image = np.sum(state_image[...,:3], axis=2) / 3
        gray_state_image = gray_state_image[:self.cut_size, :]

        return gray_state_image[::-1]


    def edge_detection(self, gray_image):
        '''
        ##### TODO #####
        In order to find edges in the gray state image,
        this function should derive the absolute gradients of the gray state image.
        Derive the absolute gradients using numpy for each pixel.
        To ignore small gradients, set all gradients below a threshold (self.gradient_threshold) to zero.

        input:
            gray_state_image 65x96x1

        output:
            gradient_sum 65x96x1

        '''

        gray_state_image = np.reshape(gray_image, (self.cut_size, 96))
        gradient_sum = np.sum(np.abs(np.gradient(gray_state_image)), axis=0)
        # gradient_sum = np.gradient(gray_state_image)[1]
        gradient_sum[gradient_sum < self.gradient_threshold] = 0

        return gradient_sum


    def find_maxima_gradient_rowwise(self, gradient_sum):
        '''
        ##### TODO #####
        This function should output arguments of local maxima for each row of the gradient image.
        You can use scipy.signal.find_peaks to detect maxima.
        Hint: Use distance argument for a better robustness.

        input:
            gradient_sum 65x96x1

        output:
            maxima (np.array) shape : (Number_maxima, 2)

        '''
        
        argmaxima = np.empty((0,2))
        height = gradient_sum.shape[0]

        for i in range(height):
            peaks = find_peaks(gradient_sum[i],distance=self.distance_maxima_gradient)[0]
            if peaks.shape[0] > 0:
                points = np.array([peaks, np.ones(peaks.shape[0])*i])
                # points = np.array([np.ones(peaks.shape[0]) * i, peaks])
                points = np.column_stack((points[0], points[1]))
                argmaxima = np.append(argmaxima, points, axis=0)
        
        return argmaxima


    def find_first_lane_point(self, gradient_sum):
        '''
        Find the first lane_boundaries points above the car.
        Special cases like just detecting one lane_boundary or more than two are considered.
        Even though there is space for improvement ;)

        input:
            gradient_sum 65x96x1

        output:
            lane_boundary1_startpoint
            lane_boundary2_startpoint
            lanes_found  true if lane_boundaries were found
        '''

        # Variable if lanes were found or not
        lanes_found = False
        row = 0

        # loop through the rows
        while not lanes_found:

            # Find peaks with min distance of at least 3 pixel
            argmaxima = find_peaks(gradient_sum[row],distance=3)[0]

            # if one lane_boundary is found
            if argmaxima.shape[0] == 1:
                lane_boundary1_startpoint = np.array([[argmaxima[0],  row]])

                if argmaxima[0] < 48:
                    lane_boundary2_startpoint = np.array([[0,  row]])
                else:
                    lane_boundary2_startpoint = np.array([[96,  row]])

                lanes_found = True

            # if 2 lane_boundaries are found
            elif argmaxima.shape[0] == 2:
                lane_boundary1_startpoint = np.array([[argmaxima[0],  row]])
                lane_boundary2_startpoint = np.array([[argmaxima[1],  row]])
                lanes_found = True

            # if more than 2 lane_boundaries are found
            elif argmaxima.shape[0] > 2:
                # if more than two maxima then take the two lanes next to the car, regarding least square
                A = np.argsort((argmaxima - self.car_position[0])**2)
                lane_boundary1_startpoint = np.array([[argmaxima[A[0]],  0]])
                lane_boundary2_startpoint = np.array([[argmaxima[A[1]],  0]])
                lanes_found = True

            row += 1

            # if no lane_boundaries are found
            if row == self.cut_size:
                lane_boundary1_startpoint = np.array([[0,  0]])
                lane_boundary2_startpoint = np.array([[0,  0]])
                break

        return lane_boundary1_startpoint, lane_boundary2_startpoint, lanes_found


    def lane_detection(self, state_image_full):
        '''
        ##### TODO #####
        This function should perform the road detection

        args:
            state_image_full [96, 96, 3]

        out:
            lane_boundary1 spline
            lane_boundary2 spline
        '''

        # to gray
        gray_state = self.cut_gray(state_image_full)

        # edge detection via gradient sum and thresholding
        gradient_sum = self.edge_detection(gray_state)
        maxima = self.find_maxima_gradient_rowwise(gradient_sum)

        # first lane_boundary points
        lane_boundary1_points, lane_boundary2_points, lane_found = self.find_first_lane_point(gradient_sum)

        # if no lane was found,use lane_boundaries of the preceding step
        if lane_found:

            ##### TODO #####
            #  in every iteration:
            # 1- find maximum/edge with the lowest distance to the last lane boundary point
            # 2- append maximum to lane_boundary1_points or lane_boundary2_points
            # 3- delete maximum from maxima
            # 4- stop loop if there is no maximum left
            #    or if the distance to the next one is too big (>=100)

            # lane_boundary 1

            # lane_boundary 2

            ################

            last_lane_boundary1_point = lane_boundary1_points[-1]
            last_lane_boundary2_point = lane_boundary2_points[-1]
            
            while maxima.shape[0] > 0:
                # find minimum distance to last lane_boundary1_point
                dist1 = np.sum((maxima - last_lane_boundary1_point)**2, axis=1)
                argmin1 = np.argmin(dist1)

                # find minimum distance to last lane_boundary2_point
                dist2 = np.sum((maxima - last_lane_boundary2_point)**2, axis=1)
                argmin2 = np.argmin(dist2)

                if dist1[argmin1] < 100 and dist1[argmin1] < dist2[argmin2]:
                    if dist1[argmin1] > 0:
                        lane_boundary1_points = np.append(lane_boundary1_points, [maxima[argmin1]], axis=0)
                    last_lane_boundary1_point = lane_boundary1_points[-1]
                    maxima = np.delete(maxima, argmin1, axis=0)
                elif dist2[argmin2] < 100 and dist2[argmin2] < dist1[argmin1]:
                    if dist2[argmin2] > 0:
                        lane_boundary2_points = np.append(lane_boundary2_points, [maxima[argmin2]], axis=0)
                    
                    last_lane_boundary2_point = lane_boundary2_points[-1]
                    maxima = np.delete(maxima, argmin2, axis=0)
                else:
                    if dist1[argmin1] < 100 and dist2[argmin2] < 100 and dist1[argmin1] == dist2[argmin2]:
                        # pass
                        maxima = np.delete(maxima, argmin1, axis=0)
                    else:
                        break
        

            ##### TODO #####
            # spline fitting using scipy.interpolate.splprep
            # and the arguments self.spline_smoothness
            #
            # if there are more lane_boundary points points than spline parameters
            # else use perceding spline

            # print("lane_boundary1_points: ", lane_boundary1_points)
            # print("lane_boundary2_points: ", lane_boundary2_points)
            
            if lane_boundary1_points.shape[0] > 4 and lane_boundary2_points.shape[0] > 4:

                # Pay attention: the first lane_boundary point might occur twice
                # lane_boundary 1

                row1 = lane_boundary1_points[:,0]
                col1 = lane_boundary1_points[:,1]

                # lane_boundary1, _ = splprep(np.unique([row1, col1], axis=1), s=self.spline_smoothness, k=2)
                lane_boundary1, _ = splprep([row1, col1], s=self.spline_smoothness, k=2)

                # lane_boundary 2

                row2 = lane_boundary2_points[:,0]
                col2 = lane_boundary2_points[:,1]

                # lane_boundary2, _ = splprep(np.unique([row2, col2], axis=1), s=self.spline_smoothness, k=2)
                lane_boundary2, _ = splprep([row2, col2], s=self.spline_smoothness, k=2)

            else:
                lane_boundary1 = self.lane_boundary1_old
                lane_boundary2 = self.lane_boundary2_old
            ################

        else:
            lane_boundary1 = self.lane_boundary1_old
            lane_boundary2 = self.lane_boundary2_old

        self.lane_boundary1_old = lane_boundary1
        self.lane_boundary2_old = lane_boundary2

        # output the spline
        return lane_boundary1, lane_boundary2


    def plot_state_lane(self, state_image_full, steps, fig, waypoints=[]):
        '''
        Plot lanes and way points
        '''
        # evaluate spline for 6 different spline parameters.
        t = np.linspace(0, 1, 6)
        lane_boundary1_points_points = np.array(splev(t, self.lane_boundary1_old))
        lane_boundary2_points_points = np.array(splev(t, self.lane_boundary2_old))

        plt.gcf().clear()
        plt.imshow(state_image_full[::-1])
        plt.plot(lane_boundary1_points_points[0], lane_boundary1_points_points[1]+96-self.cut_size, linewidth=5, color='orange')
        plt.plot(lane_boundary2_points_points[0], lane_boundary2_points_points[1]+96-self.cut_size, linewidth=5, color='orange')
        if len(waypoints):
            plt.scatter(waypoints[0], waypoints[1]+96-self.cut_size, color='white')

        plt.axis('off')
        plt.xlim((-0.5,95.5))
        plt.ylim((-0.5,95.5))
        plt.gca().axes.get_xaxis().set_visible(False)
        plt.gca().axes.get_yaxis().set_visible(False)
        fig.canvas.flush_events()



