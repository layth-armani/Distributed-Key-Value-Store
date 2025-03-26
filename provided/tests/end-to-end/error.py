import re
import os

SRC_DIR = os.getenv('SRC_DIR', "../../done")

errors = {}

# Return code signedness is os-dependent, so we convert to unsigned to avoid any issues.
def error_code_to_unsigned(code):
    if code < 0:
        return 256 + code
    return code

def load_errors():
    with open(f"{SRC_DIR}/error.h") as f:
        line = f.readline().strip()
        while not line.startswith("enum") and "error_codes" in line:
            line = f.readline().strip()
            pass
        
        i = -128
        while not "}" in line:
            line = f.readline().strip()
            match = re.match(r"\s*([A-Z_]+)", line)
            if match:
                errors[error_code_to_unsigned(i)] = match.groups()[0]
                i += 1
    
    errors[0] = "ERR_NONE"
    errors[1] = "ASAN_ERROR"

def error_name(code):
    return errors[error_code_to_unsigned(code)]

def error_code(name):
    for code, err in errors.items():
        if err == name:
            return code
    return None
            
load_errors()    
