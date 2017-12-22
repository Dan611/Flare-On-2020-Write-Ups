# Funmail2.0
**Category:** Reverse Engineering  
**Points:** 50

**Story**  
>john galt is having some problems with his email again. But this time it's not his fault. Can you help him?

Running the command `file funmail2.0` we get:

	funmail2.0: ELF 32-bit LSB shared object, Intel 80386, ...

Running the program with `./funmail2.0`, we are asked for a username and password. The program recognizes the username `john galt`, but we still need a password to log in. A quick scan through the binary in a hex editor will reveal the following string:

	00002040: 2D 2D 2D 2D 2D 2D 2D 2D 20 00 00 00 6D 6F 72 65    --------....more
	00002050: 2D 73 65 63 75 72 65 2D 70 61 73 73 77 6F 72 64    -secure-password

However, when we attempt to log in, the program terminates with the error:

	ERROR! Program failed to load emails.
	Terminating

Opening the binary in a disassembler and looking through the strings, we see the following entries relating to emails:

	.rodata:00001313 aYouHave1Unread db 'You have 1 unread email.',0 ; DATA XREF: showEmails+15
	.rodata:0000132C a1ReadEmail     db '1) Read Email',0   	 ; DATA XREF: showEmails+27
	.rodata:0000133A a2Quit          db '2) Quit',0          	 ; DATA XREF: showEmails+39
	.rodata:00001342 asc_1342        db '>> ',0              	 ; DATA XREF: showEmails+4B
	.rodata:00001346 aInputIsTooLong db 'Input is too long',0 	 ; DATA XREF: showEmails+72


If we debug the program and set the instruction pointer to the start of `showEmails`, we check the emails just fine and get the flag.

**Flag**  
`TUCTF{l0c4l_<_r3m073_3x3cu710n}`