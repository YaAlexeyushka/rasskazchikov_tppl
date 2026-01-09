import argparse
import sys

def calc_distance(p1, p2):
    return ((p2[0] - p1[0])**2 + (p2[1]- p1[1])**2)**0.5

def read_file(file_name):
    with open(file_name, 'r') as file:
        points_file = file.readlines()
        return points_file
    
def read_points(points_file):
    points = []
    for point in points_file:
        x, y = point.strip().split()
        points.append((float(x), float(y)))
    return points

def parse_input():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_args", nargs="*")
    parser.add_argument("--mode", "-m", choices=["all", "sum"], default="all")
    args = parser.parse_args()
    return args

def parse_points(args):
    points = []
    for x in range(0, len(args), 2):
        points.append((float(args[x]), float(args[x+1])))
    return points

def get_points(args):
    if len(args) == 1:
        file = read_file(args[0])
        points = read_points(file)
    elif len(args) > 1:
        if len(args) % 2 != 0:
            sys.exit("coordinates should be in pairs (x y)")
        points = parse_points(args)
    else:
        sys.exit('there should be at least 1 argument')
    return points

def calc_distances(points, mode):
    dists = []
    for point_num1 in range(len(points)):
        for point_num2 in range(point_num1, len(points)):
            if point_num1 != point_num2:
                dist = calc_distance(points[point_num1], points[point_num2])
                dists.append(dist)
                if mode == "all":
                    print(f"distance between point{point_num1} and point{point_num2} is {round(dist, 8)}")
    return dists

def get_dists_sum(dists):
    return round(sum(dists), 8)







