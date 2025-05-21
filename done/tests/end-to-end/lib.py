import unittest
import subprocess
import os
from error import *
from shutil import copyfile
import glob
import time
import signal

# https://imzye.com/Python/python-func-timeout/
class Timeout:
    def __init__(self, seconds=1, error_message="Timeout"):
        self.seconds = seconds
        self.error_message = error_message

    def handle_timeout(self, signum, frame):
        raise TimeoutError(self.error_message)

    def __enter__(self):
        signal.signal(signal.SIGALRM, self.handle_timeout)
        signal.alarm(self.seconds)

    def __exit__(self, type, value, traceback):
        signal.alarm(0)


SRC_DIR = os.getenv("SRC_DIR", "../../done")
EXE = f"./dkvs-client"
SERVER_EXE = f"./dkvs-server"
DUMP_EXE = f"./dkvs-dump-ring"
FAKE_EXE = f"./fake-dkvs-client-"
TMP_DIR = f"/tmp/cs202"


class DKVSTests(unittest.TestCase):
    FAIL_REGEX = r"FAIL"
    OK_REGEX = r"OK"

    def setUp(self):
        self.commands = []
        self.running_servers = []
        self.server_file = ""

        os.makedirs(TMP_DIR, exist_ok=True)

        if os.path.exists(f"{SRC_DIR}/{SERVER_EXE}"):
            copyfile(f"{SRC_DIR}/{SERVER_EXE}", f"{TMP_DIR}/{SERVER_EXE}")
            os.chmod(f"{TMP_DIR}/{SERVER_EXE}", 0o777)

        if os.path.exists(f"{SRC_DIR}/{EXE}"):
            copyfile(f"{SRC_DIR}/{EXE}", f"{TMP_DIR}/{EXE}")
            os.chmod(f"{TMP_DIR}/{EXE}", 0o777)

        if os.path.exists(f"{SRC_DIR}/{DUMP_EXE}"):
            copyfile(f"{SRC_DIR}/{DUMP_EXE}", f"{TMP_DIR}/{DUMP_EXE}")
            os.chmod(f"{TMP_DIR}/{DUMP_EXE}", 0o777)

        for file in glob.glob(f"{FAKE_EXE}*"):
            copyfile(file, f"{TMP_DIR}/{file}")
            os.chmod(f"{TMP_DIR}/{file}", 0o777)

        self.create_server_file(
            ["127.0.0.1", "1234", "1"],
            ["127.0.0.1", "1235", "1"],
            ["127.0.0.1", "1236", "1"],
        )

    def tearDown(self):
        if len(self.running_servers) > 0:
            self.commands.append(["killall", "dkvs-server"])

        for serv in self.running_servers:
            serv.send_signal(2)
            serv.wait()

    def nice_format(self, command):
        def remove_tmpdir_prefix(word: str):
            return word.removeprefix(TMP_DIR).lstrip("/")

        def add_quotes(word: str):
            return '"' + word + '"' if (not word or " " in word) else word

        return " ".join([add_quotes(remove_tmpdir_prefix(word)) for word in command])

    def run(self, result=None):
        failure_count = len(result.failures) if result else 0

        with Timeout(seconds=20):
            res = unittest.TestCase.run(self, result)  # call superclass run method

        if failure_count != len(res.failures):
            sf_print = ""
            if self.server_file != "":
                sf_print = "\n WITH SERVER FILE:\n\n" + self.server_file

            res.failures[-1] = (
                res.failures[-1][0],
                res.failures[-1][1]
                + "\n ---------------- TO REPRODUCE, RUN ---------------\n"
                + "\n "
                + "\n ".join([f"{self.nice_format(cmd)}" for cmd in self.commands])
                + "\n "
                + sf_print
                + "\n --------------------------------------------------\n",
            )

        return res

    def assertErr(self, actual, name, context=None):
        if isinstance(name, list):
            expected = [error_code(n) for n in name]
            message = ", ".join(name)
            message = f"expected one of [{message}], got {error_name(actual)}"

            if context:
                message = message + context

            self.assertIn(actual, expected, msg=message)
        else:
            expected = error_code(name)
            message = f"expected {name}, got {error_name(actual)}"

            if context:
                message = message + context

            self.assertEqual(actual, expected, msg=message)

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
        (
            self.assertRegex(out, f"\\nOK\\s+{value}$")
            if value
            else self.assertRegex(out, f"\\nOK\\s*$")
        )

    def create_server_file(self, *servers):
        self.server_file = ""
        with open(f"{TMP_DIR}/servers.txt", "w") as file:
            for s in servers:
                line = " ".join(s) + "\n"
                file.write(line)
                self.server_file += line

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

    def server(self, ip, port, logfile=None, **initial_values):
        cmd = [SERVER_EXE, ip, port]

        for k, v in initial_values.items():
            cmd += [k, v]

        if logfile:
            cmd += [f">{logfile}", "2>&1"]
        self.commands.append(cmd + ["&"])
        self.running_servers.append(subprocess.Popen(cmd, cwd=TMP_DIR))

    def parse_fake_output(self, out: str, get_old_map=False):
        lines = out.splitlines()
        oldmap = {}
        newmap = {}

        while len(lines) > 0 and not "end of fake network init" in lines[0]:
            kv = lines[0].split(": ", 1)
            if len(kv) > 1:
                oldmap[kv[0]] = kv[1]
            lines = lines[1:]
        lines = lines[1:]

        while len(lines) > 0 and lines[0] != "--------------------------------":
            kv = lines[0].split(": ", 1)
            newmap[kv[0]] = kv[1]
            lines = lines[1:]

        if get_old_map:
            return newmap, oldmap
        else:
            return newmap

    def dump(self):
        cmd = [f"{TMP_DIR}/{DUMP_EXE}"]
        self.commands.append(cmd)

        res = subprocess.run(
            cmd, check=False, capture_output=True, text=True, cwd=TMP_DIR
        )

        return res.stdout.strip()

    def parse_dump(self, dump: str):
        nodes = []
        servers = {}

        lines = [s.strip() for s in dump.splitlines() if s.strip()]

        self.assertEqual(lines[0], "Ring nodes:")
        lines = lines[1:]

        nodeRe = r"^([a-fA-F\d]{40})\s+\((\d{,3}(?:\.\d{,3}){,3})\s+(\d+)\)$"

        while re.match(nodeRe, lines[0]):
            m = re.match(nodeRe, lines[0])
            nodes.append([m.group(1), m.group(2), m.group(3)])
            lines = lines[1:]

        while not len(lines) == 0:
            nodeHeaderRe = r"^(\d{,3}(?:\.\d{,3}){,3}:\d+):(.*)$"
            keyCountRe = r"^storing (\d+) key-value pairs?:$"

            m = re.match(nodeHeaderRe, lines[0])
            if m is None or m.group(2):
                lines = lines[1:]
                continue

            node = m.group(1)
            servers[node] = {}

            lines = lines[1:]
            if len(lines) == 0:
                continue

            m = re.match(keyCountRe, lines[0])

            count = int(m.group(1))
            lines = lines[1:]

            while len(lines) != 0 and len(lines[0].split(" --> ")) == 2:
                s = lines[0].split(" --> ")
                servers[node][s[0]] = s[1]
                lines = lines[1:]

            self.assertEqual(len(servers[node]), count)

        return nodes, servers
