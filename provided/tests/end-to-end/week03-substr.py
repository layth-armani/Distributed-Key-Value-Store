import unittest
from lib import DKVSTests
from error import *


class Week03(DKVSTests):
    # ======================================================================
    # substr
    # ------------------------------------------------------------
    # Tool function
    def assertSubstrResultMatches(self, out, get_key, position, length, put_key):
        new_map, old_map = self.parse_fake_output(out, get_old_map=True)

        if put_key != get_key:
            self.assertEqual(new_map[get_key], old_map[get_key])

        if position < 0 and -position == length:
            substr = old_map[get_key][position:]
        else:
            substr = old_map[get_key][position : position + length]

        self.assertEqual(new_map[put_key], substr)

        self.assertRegex(out, f"OK$")

    def assertSubstrEquals(self, get_key, position, length, put_key, fake=None):
        (ret, out, err) = self.client(
            "substr", get_key, str(position), str(length), put_key, fake=fake
        )
        self.assertErr(ret, "ERR_NONE")
        self.assertSubstrResultMatches(out, get_key, position, length, put_key)

    # ------------------------------------------------------------
    # Tests
    # ------------------------------------------------------------
    def test_E_handout_examples_substr_1(self):
        self.assertSubstrEquals("bbb", 2, 5, "new_key", fake="2")

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_2(self):
        self.assertSubstrEquals("bbb", 0, 2, "new_key", fake="2")

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_3(self):
        self.assertSubstrEquals("bbb", 0, 9, "new_key", fake="2")

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_4(self):
        self.assertSubstrEquals("bbb", -1, 1, "new_key", fake="2")

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_5(self):
        self.assertSubstrEquals("bbb", -5, 4, "new_key", fake="2")

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_6(self):
        (ret, out, err) = self.client("substr", "bbb", "3", "new_key", fake="2")

        self.assertErr(ret, "ERR_INVALID_COMMAND")

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_7(self):
        (ret, out, err) = self.client("substr", "bbb", "3", "7", "new_key", fake="2")

        self.assertErr(ret, "ERR_INVALID_COMMAND")
        self.assertRegex(out, self.FAIL_REGEX)

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_8(self):
        (ret, out, err) = self.client("substr", "bbb", "3", "-1", "new_key", fake="2")

        self.assertErr(ret, "ERR_INVALID_COMMAND")
        self.assertRegex(out, self.FAIL_REGEX)

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_9(self):
        (ret, out, err) = self.client("substr", "bbb", "-2", "3", "new_key", fake="2")

        self.assertErr(ret, "ERR_INVALID_COMMAND")
        self.assertRegex(out, self.FAIL_REGEX)

    # ------------------------------------------------------------
    def test_E_handout_examples_substr_selfassign(self):
        self.assertSubstrEquals("bbb", 1, 4, "bbb", fake="2")

# ======================================================================
if __name__ == "__main__":
    unittest.main(verbosity=2)
