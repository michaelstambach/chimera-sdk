import subprocess

INPUT_A = "/workspaces/chimera-sdk/build/picolibc-build/librv32im/ilp32/libc.a"
TARGET_OBJ = "libc_ctype_tolower.c.o"


def main():
    # where are symbols provided
    # key: symbol, val: file
    provides = {}
    # which file requires what
    # key: file, val: symbol list
    requires = {}

    nm_out = subprocess.run(['nm', INPUT_A], stdout=subprocess.PIPE)
    nm_lines = nm_out.stdout.decode('utf-8').splitlines()

    cur_file = ""
    cur_requires = []
    for line in nm_lines:
        if line.endswith(':'):
            requires[cur_file] = cur_requires
            cur_file = line[:-1]
            cur_requires = []
        else:
            parts = line.strip().split(' ')
            if parts[0] == 'U':
                cur_requires.append(parts[1])
            elif len(parts) == 3:
                provides[parts[2]] = cur_file
    
    requires[cur_file] = cur_requires

    deps_found = set()
    deps_todo = set([TARGET_OBJ])

    while len(deps_todo) > 0:
        obj = deps_todo.pop()
        if obj in deps_found:
            continue
        deps_found.add(obj)
        reqs = requires[obj]
        for req in reqs:
            if req in provides.keys():
                provider = provides[req]
                deps_todo.add(provider)
            else:
                print(f"{req} not provided anywhere!")
    
    print(deps_found)

if __name__ == '__main__':
    main()
