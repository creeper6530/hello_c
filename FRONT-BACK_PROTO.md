# Frontend-backend protocol

Only file descriptor that can be closed is the **write** end of Front-to-Back; that signals "please exit now" to the backend


## Message type
| Byte | Meaning | Notes |
| ---- | ------- | ----- |
| 0x00 | NOOP | Does nothing |
| 0x01 | ECHO | Echoes back the entire message |


# Return code of worker
| Code | Meaning |
| ---- | ------- |
| 0 | Success |
| 1 | Read error |
| 2 | Write error |
| 127 | Invalid message |

# Misc

## GDB debugging
To debug, open two terminals: in first one run `tty` to get its TTY device,
then (IMPORTANT!!!) run `sleep 999999999` to force Bash to yield control over the TTY
so that you don't combat and have issues.

In the other window, run `gdb out/app`.
There, run `(gdb) tty /path/to/device` to attach to the other TTY,
run `(gdb) break FILE:XX`, where FILE is filename and XX line number,
run `(gdb) run` to start the program.

To print an expression once, run `(gdb) print EXPR`.
To print bytes the easiest, just cast something to `unsigned char [XX]`, where XX is a sufficiently high number.
To print an expression in a certain format, run `(gdb) print/FMT EXPR`, where FMT is either:
- `x` for hex
- `t` for binary
- `c` for character
- `f` for float
- `s` for string

To automatically print an expression on every breakpoint, run `(gdb) display EXPR`.
To break whenever an expression changes, run `(gdb) watch EXPR`.

To resume executing after a breakpoint, run `(gdb) cont` or `(gdb) continue`.
To execute until a function ends, run `(gdb) finish`.
To execute next line of code in current file, run `(gdb) next`.
To execute next line of code (stepping inside functions), run `(gdb) step`.
