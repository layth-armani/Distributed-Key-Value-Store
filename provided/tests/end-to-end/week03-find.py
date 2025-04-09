import unittest
from lib import DKVSTests
import re


class Week03Find(DKVSTests):
    # ======================================================================
    # find
    # ------------------------------------------------------------
    # Tool function
    def assertFindEquals(self, haystack_key, needle_key, value, fake=None):
        (ret, out, err) = self.client("find", haystack_key, needle_key, fake=fake)

        self.assertErr(ret, "ERR_NONE")
        match = next(re.finditer(r"OK\s*(-?\d+)", out))
        self.assertIsNotNone(match, msg='Output should end with "OK <INDEX>"')
        self.assertEqual(match.group(1), str(value))

    # ------------------------------------------------------------
    # Tests
    # ------------------------------------------------------------
    def test_A_handout_examples_find_1(self):
        self.assertFindEquals("ab", "a", 0, fake="2")

    # ------------------------------------------------------------
    def test_A_handout_examples_find_2(self):
        self.assertFindEquals("a", "ab", -1, fake="2")

    # ------------------------------------------------------------
    def test_A_handout_examples_find_3(self):
        (ret, out, err) = self.client("find", "ab", "invalid_key", fake="2")

        self.assertErr(ret, "ERR_NOT_FOUND")
        self.assertRegex(out, self.FAIL_REGEX)

    # ------------------------------------------------------------
    def test_A_handout_examples_find_4(self):
        (ret, out, err) = self.client("find", "a", fake="2")

        self.assertErr(ret, "ERR_INVALID_COMMAND")

    # ------------------------------------------------------------
    def test_A_handout_examples_find_5(self):
        (ret, out, err) = self.client("find", fake="2")

        self.assertErr(ret, "ERR_INVALID_COMMAND")

# ======================================================================
if __name__ == "__main__":
    unittest.main(verbosity=2)
