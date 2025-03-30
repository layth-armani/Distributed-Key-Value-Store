import unittest
import subprocess
import os
from error import *
from shutil import copyfile
import glob

SRC_DIR = os.getenv('SRC_DIR', "../../done")
EXE = f"./dkvs-client"
SERVER_EXE = f"./dkvs-server"
FAKE_EXE = f"./fake-dkvs-client-"
TMP_DIR = f"/tmp/cs202"


class DKVSTests(unittest.TestCase):
    def setUp(self):
        self.commands = []
        self.running_servers = []

        os.makedirs(TMP_DIR, exist_ok=True)

        if os.path.exists(f"{SRC_DIR}/{SERVER_EXE}"):
            copyfile(f"{SRC_DIR}/{SERVER_EXE}", f"{TMP_DIR}/{SERVER_EXE}")
            os.chmod(f"{SRC_DIR}/{SERVER_EXE}", 0o777)

        for file in glob.glob(f"{FAKE_EXE}*"):
            copyfile(file, f"{TMP_DIR}/{file}")
            os.chmod(f"{TMP_DIR}/{file}", 0o777)

        self.create_server_file(
            ["127.0.0.1", "1234"], ["127.0.0.1" "1235"], ["127.0.0.1", "1236"]
        )

    def tearDown(self):
        result = self._outcome.result
        ok = all(test != self for test, text in result.errors + result.failures)
        if not ok:
            failure = self._outcome.result.failures[-1]
            self._outcome.result.failures[-1] = (
                self,
                failure[1]
                + "\n === TO REPRODUCE, RUN ===\n"
                + "\n".join([f"{' '.join(cmd)}" for cmd in self.commands])
                + "\n",
            )

        for serv in self.running_servers:
            serv.send_signal(2)
            serv.wait()

    def assertErr(
        self,
        actual,
        name,
    ):
        expected = error_code(name)
        self.assertEqual(
            actual, expected, msg=f"expected {name}, got {error_name(actual)}"
        )

    def assertErrNotEquals(
        self,
        actual,
        name,
    ):
        expected = error_code(name)
        self.assertNotEqual(
            actual, expected, msg=f"expected error different than {name}"
        )
        
    def assertGetEquals(self, key, value, fake=None):
        (ret, out, err) = self.client("get", key, fake=fake)

        self.assertErr(ret, "ERR_NONE")
        self.assertRegex(out, f"OK\\s*{value}$")

    def create_server_file(self, *servers):
        with open(f"{TMP_DIR}/server.txt", "w") as file:
            for s in servers:
                file.write(" ".join(s) + "\n")

    def client(self, *args, w=None, r=None, n=None, fake=None):
        cmd = [f"{TMP_DIR}/{FAKE_EXE}{fake}" if fake else f"{TMP_DIR}/{EXE}"]
        if len(args) > 0:
            cmd.append(args[0])

        if r is not None:
            cmd.append("-r")
            cmd.append(str(r))
        if w is not None:
            cmd.append("-w")
            cmd.append(str(w))
        if n is not None:
            cmd.append("-n")
            cmd.append(str(n))

        if len(args[1:]) > 0:
            cmd.append("--")
            cmd.extend(args[1:])

        self.commands.append(cmd)
        res = subprocess.run(
            cmd, check=False, capture_output=True, text=True, cwd=TMP_DIR
        )

        return (res.returncode, res.stdout.strip(), res.stderr.strip())

    def server(self, ip, port):
        cmd = [SERVER_EXE, ip, port]
        self.commands.append(cmd + ["&"])
        self.running_servers.append(subprocess.Popen(cmd, cwd=TMP_DIR))

    def parse_fake_output(self, out: str):
        lines = out.splitlines()
        hashmap = {}

        while not "end of fake network init" in lines[0]:
            lines = lines[1:]
        lines = lines[1:]

        while lines[0] != "--------------------------------":
            kv = lines[0].split(": ", 1)
            hashmap[kv[0]] = kv[1]
            lines = lines[1:]

        return hashmap
