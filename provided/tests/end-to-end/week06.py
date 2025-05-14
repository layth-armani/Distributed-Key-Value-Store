import unittest
from lib import DKVSTests


class Week06(DKVSTests):
    def test_get_no_server(self):
        (res, out, err) = self.client("get", "abc")
        self.assertErr(res, ["ERR_NOT_FOUND", "ERR_NETWORK"])

    def test_put_no_server(self):
        (res, out, err) = self.client("put", "abc", "def")
        self.assertErrNotEquals(res, "ERR_NONE")

    def test_network_get_empty(self):
        self.server("127.0.0.1", "1234")
        self.server("127.0.0.1", "1235")
        self.server("127.0.0.1", "1236")

        (res, out, err) = self.client("get", "abc")
        self.assertErr(res, "ERR_NOT_FOUND")

    def test_network_get(self):
        self.server("127.0.0.1", "1234", abc="def")
        self.server("127.0.0.1", "1235", abc="def")
        self.server("127.0.0.1", "1236", abc="def")

        (res, out, err) = self.client("get", "abc")
        self.assertErr(res, "ERR_NONE")
        self.assertEqual(out, "OK def")

    def test_network_get_single_server(self):
        self.create_server_file(["127.0.0.1", "1234", "1"])

        self.server("127.0.0.1", "1234", abc="def")

        (res, out, err) = self.client("get", "abc", r=1, n=1)
        self.assertErr(res, "ERR_NONE")
        self.assertEqual(out, "OK def")

    def test_network_get_single_up(self):
        self.server("127.0.0.1", "1236", abc="def")

        (res, out, err) = self.client("get", "abc", r=1, n=3)
        self.assertErr(res, "ERR_NONE")
        self.assertEqual(out, "OK def")

    def test_network_get_quorum_failure(self):
        self.server("127.0.0.1", "1236", abc="def")

        (res, out, err) = self.client("get", "abc", r=2, n=3)
        self.assertErr(res, ["ERR_NOT_FOUND", "ERR_NETWORK"])
        self.assertRegex(out, self.FAIL_REGEX)

    def test_network_put_then_get(self):
        self.server("127.0.0.1", "1234", "LOG1.txt")
        self.server("127.0.0.1", "1235", "LOG2.txt")
        self.server("127.0.0.1", "1236", "LOG3.txt")

        (res, out, err) = self.client("put", "abc", "def")
        self.assertErr(res, "ERR_NONE")
        self.assertEqual(out, "OK")

        (res, out, err) = self.client("get", "abc")
        self.assertErr(res, "ERR_NONE")
        self.assertEqual(out, "OK def")

    def test_network_put_then_get_suffix(self):
        self.server("127.0.0.1", "1234")
        self.server("127.0.0.1", "1235")
        self.server("127.0.0.1", "1236")

        (res, out, err) = self.client("put", "abc", "def")
        self.assertErr(res, "ERR_NONE")
        self.assertEqual(out, "OK")

        (res, out, err) = self.client("get", "abcdef")
        self.assertErr(res, "ERR_NOT_FOUND")
        self.assertRegex(out, self.FAIL_REGEX)

        (res, out, err) = self.client("get", "a")
        self.assertErr(res, "ERR_NOT_FOUND")
        self.assertRegex(out, self.FAIL_REGEX)


if __name__ == "__main__":
    unittest.main(verbosity=2)
