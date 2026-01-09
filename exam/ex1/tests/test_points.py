import pytest
from points.points import *

class Test_point:
    def test_calc_distance(self):
        assert calc_distance((4, 4), (6, 4)) == float(2.0)
        
    def test_parse_points(self):
        args = ["0", "0", "3", "4"]
        points = parse_points(args)
        assert points == [(0.0,0.0),(3.0,4.0)]
    
    def test_get_points(self):
        args = ["0","0","1","1"]
        points = get_points(args)
        assert points == [(0.0,0.0),(1.0,1.0)]
            
    def test_get_points_odd_number(self):
        args = ["0","0","1"]
        with pytest.raises(SystemExit) as e:
            get_points(args)
        assert str(e.value) == "coordinates should be in pairs (x y)"
        
    def test_get_points_no_args(self):
        args = []
        with pytest.raises(SystemExit) as e:
            get_points(args)
        assert str(e.value) == "there should be at least 1 argument"

    def test_read_points(self, tmp_path):
        d = tmp_path/"test_data.txt"
        d.write_text("0 0\n1 1\n")
        lines = open(d).readlines()
        points = read_points(lines)
        assert points == [(0.0,0.0),(1.0,1.0)]
        
    def test_read_file(self, tmp_path):
        file_path = tmp_path / "data.txt"
        file_path.write_text("0 0\n1 1\n")
        lines = read_file(file_path)
        assert lines == ["0 0\n", "1 1\n"]

    def test_get_points_file(self, tmp_path):
        file_path = tmp_path / "data.txt"
        file_path.write_text("0 0\n1 1\n")
        points = get_points([str(file_path)])
        assert points == [(0.0,0.0),(1.0,1.0)]
        
    def test_calc_distances(self):
        points = [(0, 0), (3, 4), (6, 8)]
        dists = calc_distances(points, mode="sum")
        expected = [5.0, 10.0, 5.0]
        assert dists == expected
        
    def test_get_dists_sum(self):
        dists = [1.123456789, 2.987654321, 3.5]
        result = get_dists_sum(dists)
        assert result == 7.61111111