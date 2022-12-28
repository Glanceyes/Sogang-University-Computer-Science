from functools import cmp_to_key

global pivot
pivot = [0, 0]

def ccw(point1, point2, point3):
    val = 0
    val = point1[0] * point2[1] + point2[0] * point3[1] + point3[0] * point1[1]
    val -= point1[1] * point2[0] + point2[1] * point3[0] + point3[1] * point1[0]

    if val > 0:
        return 1
    elif val < 0:
        return -1
    
    return 0


def distance(point1, point2):
    x = point1[0] - point2[0]
    y = point1[1] - point2[1]
    return x * x + y * y


def compare(point1, point2):
    result = ccw(pivot, point1, point2)
    if result == 0:
        return distance(pivot, point1) <= distance(pivot, point2)
    
    return -result


def cross(line1, line2):
    c1 = ccw(line1[0], line1[1], line2[0]) * ccw(line1[0], line1[1], line2[1])
    c2 = ccw(line2[0], line2[1], line1[0]) * ccw(line2[0], line2[1], line1[1])
    
    return c1 < 0 and c2 < 0


def cross_product(vector1, vector2):
    result = vector1[0] * vector2[1] - vector1[1] * vector2[0]
    return result


def computing_area_of_polygon(polygon_points):
    area = 0

    for i in range(0, len(polygon_points)):
        area += cross_product(polygon_points[i], polygon_points[(i + 1) % len(polygon_points)])
    
    return abs(area) / 2


def convex_hull(points):
    global pivot
    pivot = points[0]

    for i in range(1, len(points)):
        if points[i][0] < pivot[0] or (points[i][0] == pivot[0] and points[i][1] < pivot[1]):
            pivot = points[i]

    points = list(filter(lambda x: x != pivot, points))
    points.sort(key=cmp_to_key(compare))
    stack = [pivot]

    for i in range(len(points)):
        while len(stack) >= 2 and ccw(stack[-2], stack[-1], points[i]) <= 0:
            stack.pop()
        stack.append(points[i])
    return stack


def checking_inner_point(polygon_points, point):
    for i in range(len(polygon_points)):
        next_point_index = 0
        if i < len(polygon_points) - 1:
            next_point_index = i + 1
        
        result = ccw(polygon_points[i], polygon_points[next_point_index], point)

        if result == 0:
            return False
            
    cnt = 0
    pivot = [1, 2**31 - 1]
    for i in range(len(polygon_points)):
        next_point_index = 0
        if i < len(polygon_points) - 1:
            next_point_index = i + 1

        pivot_line = [ pivot, point ]
        cnt += cross(pivot_line, [polygon_points[i], polygon_points[next_point_index]])

    return cnt % 2 == 1


def main():
    start_index = 0
    student_number = "20171665"
    student_number = student_number + "_"

    input_file_names = [ 
        "input_example.txt", 
        "input1.txt", 
        "input2.txt",
        "input3.txt",
    ]
    output_file_names = [ 
        "output_example.txt", 
        "{}output1.txt".format(student_number), 
        "{}output2.txt".format(student_number),
        "{}output3.txt".format(student_number), 
    ]

    for i in range(start_index, len(input_file_names)):
        convex_hull_points = []

        file_to_read = open(input_file_names[i], "r")
        for line in file_to_read:
            line = line.strip()
            line = line.split()
            convex_hull_points.append([int(line[0]), int(line[1])])

        file_to_read.close()

        convex_hull_points = convex_hull(convex_hull_points)
        polygon_area = computing_area_of_polygon(convex_hull_points)

        file_to_write = open(output_file_names[i], "w")
        file_to_write.write(str(polygon_area) + "\n")
        for point in convex_hull_points:
            file_to_write.write(str(point[0]) + " " + str(point[1]) + "\n")

        file_to_write.close()

    inner_points = []
    inner_point_file_to_read = open("point_in_polygon_input.txt", "r")
    for line in inner_point_file_to_read:
        line = line.strip()
        line = line.split()
        inner_points.append([int(line[0]), int(line[1])])
    
    inner_point_file_to_read.close()

    for i in range(start_index, len(output_file_names)):
        convex_hull_points = []

        output_file_to_read = open(output_file_names[i], "r")
        polygon_area = float(output_file_to_read.readline().strip())

        for line in output_file_to_read:
            line = line.strip()
            line = line.split()
            convex_hull_points.append([int(line[0]), int(line[1])])

        output_file_to_read.close()

        file_to_write = open("{0}point_in_polygon_output{1}.txt".format(
            student_number if i > 0 else "", 
            i if i > 0 else "_example"), 
        "w")

        for inner_point in inner_points:
            result = checking_inner_point(convex_hull_points, inner_point)

            if result:
                file_to_write.write("in\n")
            else:
                file_to_write.write("out\n")

        file_to_write.close()
        
if __name__ == "__main__":
    main()