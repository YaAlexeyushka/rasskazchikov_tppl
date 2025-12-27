from text_analyzer import *
import unittest


class TestFileAnalyzer(unittest.TestCase):

    def test_get_lines_len_empty(self):
        self.assertEqual(get_lines_len([]), 0)

    def test_get_lines_len_multiple(self):
        self.assertEqual(get_lines_len(["a\n", "b\n", "c\n"]), 3)

    def test_get_characters_len_empty(self):
        self.assertEqual(get_characters_len([]), 0)

    def test_get_characters_len_with_text(self):
        self.assertEqual(get_characters_len(["hello\n"]), 6)

    def test_get_empty_lines_none(self):
        self.assertEqual(get_empty_lines_len(["text\n"]), 0)

    def test_get_empty_lines_all_empty(self):
        self.assertEqual(get_empty_lines_len(["\n", "  \n"]), 2)

    def test_get_freq_map_basic(self):
        result = get_freq_map(["aab"])
        self.assertEqual(result, {'a': 2, 'b': 1})


if __name__ == '__main__':
    unittest.main()
