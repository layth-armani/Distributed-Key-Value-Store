import unittest
from lib import DKVSTests


class Week02(DKVSTests):
    def test_fake_no_cmd_should_fail(self):
        (ret, out, err) = self.client(fake="1")
        self.assertErr(ret, "ERR_INVALID_COMMAND")

    def test_get_has_exactly_one_arg(self):
        (ret, out, err) = self.client("get", fake="1")
        self.assertErr(ret, "ERR_INVALID_COMMAND")

        (ret, out, err) = self.client("get", "abc", "def", fake="1")
        self.assertErr(ret, "ERR_INVALID_COMMAND")

    def test_get_on_empty_dkvs_should_fail(self):
        (ret, out, err) = self.client("get", "abc", fake="1")
        self.assertErr(ret, "ERR_NOT_FOUND")
        self.assertRegex(out, self.FAIL_REGEX)

    def test_get_value(self):
        self.assertGetEquals("ab", "hello ab", fake="2")

    def test_put_has_exactly_two_arg(self):
        (ret, out, err) = self.client("put", fake="1")
        self.assertErr(ret, "ERR_INVALID_COMMAND")

        (ret, out, err) = self.client("put", "abc", fake="1")
        self.assertErr(ret, "ERR_INVALID_COMMAND")

        (ret, out, err) = self.client("put", "abc", "def", "ghi", fake="1")
        self.assertErr(ret, "ERR_INVALID_COMMAND")

    def test_put_should_work(self):
        (ret, out, err) = self.client("put", "abc", "def", fake="2")

        self.assertDictEqual(
            self.parse_fake_output(out),
            {
                "a": "hello a",
                "bb": "hello bb",
                "ab": "hello ab",
                "bbb": "hello bbb",
                "abc": "def",
            },
        )

        self.assertErr(ret, "ERR_NONE")
        self.assertRegex(out, self.OK_REGEX)

    def test_put_replace_existing(self):
        (ret, out, err) = self.client("put", "a", "bcd", fake="2")

        self.assertDictEqual(
            self.parse_fake_output(out),
            {
                "a": "bcd",
                "bb": "hello bb",
                "ab": "hello ab",
                "bbb": "hello bbb",
            },
        )

        self.assertErr(ret, "ERR_NONE")
        self.assertRegex(out, self.OK_REGEX)

    def test_put_empty_key(self):
        (ret, out, err) = self.client("put", "", "empty", fake="1")

        self.assertDictEqual(
            self.parse_fake_output(out),
            {
                "": "empty",
            },
        )

        self.assertErr(ret, "ERR_NONE")
        self.assertRegex(out, self.OK_REGEX)

    def test_put_empty_value(self):
        (ret, out, err) = self.client("put", "empty", "", fake="1")

        self.assertDictEqual(
            self.parse_fake_output(out),
            {
                "empty": "",
            },
        )

        self.assertErr(ret, "ERR_NONE")
        self.assertRegex(out, self.OK_REGEX)

    def test_get_empty_key(self):
        (ret, out, err) = self.client("get", "", fake="3")
        self.assertErr(ret, "ERR_INVALID_ARGUMENT")

    def test_get_empty_value(self):
        self.assertGetEquals("empty", "", fake="3")

if __name__ == "__main__":
    unittest.main(verbosity=2)
