import unittest
from lib import DKVSTests


class Week07(DKVSTests):
    def test_dump_simple(self):
        self.server("127.0.0.1", "1234", abc="def")
        self.server("127.0.0.1", "1235", abc="def")
        self.server("127.0.0.1", "1236", abc="def")

        nodes, servers = self.parse_dump(self.dump())

        self.assertListEqual(
            nodes,
            [
                ["93149f866bf3acc9710375cb46706bf09960a6ab", "127.0.0.1", "1236"],
                ["aa66f3e5a8d9cdc5c0bd49708bc59847e6915634", "127.0.0.1", "1234"],
                ["c484ea9b3b14d139b1456032a49990367b857fe6", "127.0.0.1", "1235"],
            ],
        )

        self.assertDictEqual(
            servers,
            {
                "127.0.0.1:1234": {"abc": "def"},
                "127.0.0.1:1235": {"abc": "def"},
                "127.0.0.1:1236": {"abc": "def"},
            },
        )

    def test_dump_non_responsive(self):
        self.server("127.0.0.1", "1234", abc="def")
        self.server("127.0.0.1", "1236", abc="def")

        nodes, servers = self.parse_dump(self.dump())

        self.assertListEqual(
            nodes,
            [
                ["93149f866bf3acc9710375cb46706bf09960a6ab", "127.0.0.1", "1236"],
                ["aa66f3e5a8d9cdc5c0bd49708bc59847e6915634", "127.0.0.1", "1234"],
                ["c484ea9b3b14d139b1456032a49990367b857fe6", "127.0.0.1", "1235"],
            ],
        )

        self.assertDictEqual(
            servers,
            {
                "127.0.0.1:1234": {"abc": "def"},
                "127.0.0.1:1235": {},
                "127.0.0.1:1236": {"abc": "def"},
            },
        )

    def test_dump_large_values(self):
        self.create_server_file(["127.0.0.1", "1234", "1"])

        value = "0" * 30000
        self.server("127.0.0.1", "1234", a=value, b=value, c=value, d=value, e=value)

        nodes, servers = self.parse_dump(self.dump())

        self.assertListEqual(
            nodes, [["aa66f3e5a8d9cdc5c0bd49708bc59847e6915634", "127.0.0.1", "1234"]]
        )

        self.assertDictEqual(
            servers,
            {
                "127.0.0.1:1234": {
                    "a": value,
                    "b": value,
                    "c": value,
                    "d": value,
                    "e": value,
                },
            },
        )

    def test_dump_handout_example_improved(self):
        self.create_server_file(
            ["127.0.0.1", "1234", "1"],
            ["127.0.0.1", "1235", "2"],
            ["127.0.0.1", "1236", "3"],
            ["127.0.0.1", "1237", "2"],
        )

        self.server("127.0.0.1", "1234")
        self.server("127.0.0.1", "1235")
        self.server("127.0.0.1", "1236")
        self.server("127.0.0.1", "1237")

        (ret, out, err) = self.client("put", "a", "b")
        self.assertErr(ret, "ERR_NONE")
        (ret, out, err) = self.client("put", "cc", "dd")
        self.assertErr(ret, "ERR_NONE")

        nodes, servers = self.parse_dump(self.dump())

        self.assertListEqual(
            nodes,
            [
                ["05f929071339f796a97abc59a095e0cb206abb7a", "127.0.0.1", "1237"],
                ["1bcd2db55b43d8c6b50583892f141a6bb3224c04", "127.0.0.1", "1235"],
                ['413518e3f53d2922b8df4dc02d0121f0fe781b0b', '127.0.0.1', '1237'],
                ["5f26268754fcf2a51fcfacaaa2aaf4f0d83f6d67", "127.0.0.1", "1236"],
                ["93149f866bf3acc9710375cb46706bf09960a6ab", "127.0.0.1", "1236"],
                ["aa66f3e5a8d9cdc5c0bd49708bc59847e6915634", "127.0.0.1", "1234"],
                ["c484ea9b3b14d139b1456032a49990367b857fe6", "127.0.0.1", "1235"],
                ["ee482fd6bcd8a1eb2a929a0d284b563404b64d19", "127.0.0.1", "1236"],
            ],
        )

        self.assertDictEqual(
            servers,
            {
                "127.0.0.1:1234": {"a": "b"},
                "127.0.0.1:1235": {"a": "b", "cc": "dd"},
                "127.0.0.1:1236": {"a": "b", "cc": "dd"},
                "127.0.0.1:1237": {"cc": "dd"},
            },
        )


if __name__ == "__main__":
    unittest.main(verbosity=2)
