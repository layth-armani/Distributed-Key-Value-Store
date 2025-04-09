import unittest
from lib import DKVSTests


class Week03(DKVSTests):
    # ======================================================================
    # cat
    # ------------------------------------------------------------
    # Tool function
    def assertCatEquals(self, *args, put_key, fake=None):
        (ret, out, err) = self.client("cat", *args, put_key, fake=fake)

        self.assertErr(ret, "ERR_NONE")
        new_map, oldmap = self.parse_fake_output(out, get_old_map=True)
        new_value = "".join(map(lambda k: oldmap[k], args))

        # former key-value pairs unchanged?
        for key, value in oldmap.items():
            self.assertEqual(new_map[key], value)

        # new key-value inserted?
        self.assertEqual(new_map[put_key], new_value)
        self.assertRegex(out, self.OK_REGEX)

    # ------------------------------------------------------------
    # Tests
    # ------------------------------------------------------------
    def test_R_handout_examples_cat_1(self):
        self.assertCatEquals("ab", "bbb", put_key="new_key", fake="2")

    # ------------------------------------------------------------
    def test_R_handout_examples_cat_2(self):
        self.assertCatEquals("bbb", "ab", "a", "ab", put_key="new_key", fake="2")

    # ------------------------------------------------------------
    def test_R_handout_examples_cat_3(self):
        (ret, out, err) = self.client("cat", "bbb", "ccc", "new_key", fake="2")

        self.assertErr(ret, "ERR_NOT_FOUND")
        self.assertRegex(out, self.FAIL_REGEX)

    # ------------------------------------------------------------
    def test_R_handout_examples_cat_4(self):
        (ret, out, err) = self.client("cat", "ab", "bbb", fake="2")

        table = self.parse_fake_output(out)

        self.assertErr(ret, "ERR_NONE")
        self.assertEqual(table["ab"], f"hello ab")
        self.assertEqual(table["bbb"], f"hello ab")
        self.assertRegex(out, self.OK_REGEX)

# ======================================================================
if __name__ == "__main__":
    unittest.main(verbosity=2)
