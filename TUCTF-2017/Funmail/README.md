# Funmail
**Category:** Reverse Engineering  
**Points:** 25

**Story**  
>One of our employees has locked himself out of his account. can you help 'john galt' recover his password? And no snooping around his emails you hear.

Running the command `file funmail` we get:

	funmail: ELF 32-bit LSB shared object, Intel 80386, ...

Running the program with `./funmail`, we are asked for a username and password. The program recognizes the username `john galt`, but we still need a password to log in. A quick scan through the binary in a hex editor will reveal the following string:

	00002080: 74 68 69 73 2D 70 61 73 73 77 6F 72 64 2D 69 73    this-password-is
	00002090: 2D 61 2D 73 65 63 72 65 74 2D 74 6F 2D 65 76 65    -a-secret-to-eve
	000020a0: 72 79 6F 6E 65 21 00 00 2D 2D 2D 2D 2D 2D 2D 2D    ryone!..--------

We can then log in and view user `john galt`'s email, which contains the flag.

**Flag**  
`TUCTF{d0n7_h4rdc0d3_p455w0rd5}`
