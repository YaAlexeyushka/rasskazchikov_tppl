from points.points import *

def main():
    args = parse_input()
    points = get_points(args.input_args)
    dists = calc_distances(points, args.mode)
           
    if args.mode == "sum":
        print(get_dists_sum(dists))
   
    
if __name__ == "__main__":
    main()