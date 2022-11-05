# ELEC3227-NetworkArchitecture

This is a project template for the Il Matto development board.

### Project Folder Structure
    .
    ├── build           # compiled files
    ├── include         # dependency files
    ├── src             # source files
    ├── test            # test files
    ├── <target>.c
    └── makefile

### Makefile Usage

Compiles all files, viewing its size and disassemble.
```
make all
make size
make disasm
```

Compile test files in test/ which can then be run in build.
```
make test
```

Test the programmer connection, flash program to memory and flash fuses.
```
make test-flash
make flash
make fuse
```

Removes all compiled files.
```
make clean
```

Provides a help dialog.
```
make help
```
