nush
======

Usage:
- Run as an interactive shell with `make;./nush` or pass optional script input
 with `make;./nush [SCRIPT]`
- exit with `^D` or `exit`

nush supports seven operators:
- Redirect input: `nush$ sort < foo.txt`
- Redirect output: `nush$ sort foo.txt > output.txt`
- Pipe: `sort foo.txt | uniq`
- Background: `sleep 10 &`
- And: `true && echo one`
- Or: `true || echo one`
- Semicolon: `echo one; echo two`
