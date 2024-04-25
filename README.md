<p align="center">
    <img src="logo.png">
</p>

# ptrwatch

### OUT:

Ptrwatch is a Linux CLI tool for observing pointer chains in real time. 

[Generate pointer chains](https://github.com/vykt/ptrscan)

---

### DEPENDENES:

Ptrwatch dynamically links [libpwu](https://github.com/vykt/libpwu). Release +0.1.4 is required. Visit the page and follow installation instructions.

Ptrwatch also links libncurses. To build ptrwatch, you'll additionally need the ncurses development library. Consult your distro's wiki for details.

---

### INSTALLAON:

Fetch the repo:
```
$ git clone https://github.com/vykt/ptrwatch
```

Generate build files:
```
$ cd ptrscan && ./buildgen.sh
```

Build the release:
```
$ cd build && make watch
```

Check the install script & install:
```
$ cd .. && sudo ./install.sh
```

---

### CONFIGURATION FILE:

To provide ptrwatch with pointer chains you must specify them in a config file. An example config _ptrwatch.cfg_ is included in the root of the repo.

The config file can contain 3 types of lines.lank lines are allowed:

#### Comments

__Comments__ begin with a '#' and span the entire line.

config:
```
# this is a comment
```

output:
```
<none>
```

#### Titles

__Titles__ begin with a '>' and span the entire line. __Titles__ have a red colour and should be used to provide structure to the output of ptrwatch.

config:
```
>this is a title
```

output:
```
this is a title
```

#### Entries

__Entries__ display the value at the end of a pointer chain. An entry takes the following format:

```
<description>:<type>:<start_backing_file> <offsets>
```

_description_ is a string displayed besides the value read from the pointer chain.

_type_ can be any of:

- uint8
- int8
- uint16
- int16
- uint32
- int32
- uint64
- int64
- float
- double
- string:[len]

Note that the string _type_ takes an additional length parameter [len].

_start_backing_file_ can either be the string representation of the backing file (e.g. 'libc.so.6') or a starting address in written in hexadecimal with a '0x' prefix.

_offsets_ are a space delimited list of offsets. The final offset is dereferenced as _type_ and displayed.

See the provided _ptrwatch.cfg_ for examples.

---

### EXAMPLES

Watch pointer chains of process _example_proc_ using the default _ptrwatch.cfg_ config:

```
ptrwatch example_proc
```

Watch pointer chains of process with PID _1234_, using _new_config.cfg_ as the config file:
```
ptrwatch -c new_config.cfg 1234
```
