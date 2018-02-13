# div N
**Category:** Reverse Engineering  
**Points:** 100

**Description**  

	$ cat foo.c
	long long div(long long x) {
		return x / N;
	}
	$ gcc -DN=$N -c -O2 foo.c
	$ objdump -d foo.o

	foo.o:     file format elf64-x86-64


	Disassembly of section .text:

	0000000000000000 
	:
	0:	48 89 f8             	mov    %rdi,%rax
	3:	48 ba 01 0d 1a 82 9a 	movabs $0x49ea309a821a0d01,%rdx
	a:	30 ea 49 
	d:	48 c1 ff 3f          	sar    $0x3f,%rdi
	11:	48 f7 ea             	imul   %rdx
	14:	48 c1 fa 30          	sar    $0x30,%rdx
	18:	48 89 d0             	mov    %rdx,%rax
	1b:	48 29 f8             	sub    %rdi,%rax
	1e:	c3                   	retq   
	$ echo “HarekazeCTF{$N}” > /dev/null


We are provided with a .c file, information on how it was compiled with GCC, and a dump of the compiled binary using objdump. Knowing that `-DN=$N` in the line `gcc -DN=$N -c -O2 foo.c` is the same as including `#define N $N` at the top of the .c file, and seeing `echo “HarekazeCTF{$N}” > /dev/null` at the end of the description, our goal is to find the predefined value `$N`.

Because actual division is much slower than equivalent mathematical statements using multiplication and shifts, GCC will compile `x / N` into such statements as can be seen in the description. The statement `1 / N` can be calculated and shifted to the left by GCC ahead of time, shifted enough times until it is a decimal of the form `0.XXXX...` to improve accuracy. Next it will convert this decimal into a binary fraction and store it as some hex value.

The hardcoded value in the dump `0x49ea309a821a0d01` when converted to binary is `0100 1001 1110 1010 0011 0000 1001 1010 1000 0010 0001 1010 0000 1101 0000 0001`, which can be thought of as the binary fraction `0.0100100111101010001100001001101010000010000110100000110100000001`. Converting this to a decimal value yields `0.2887297036378202037...`. Now we simply use the provided shift value on line 14: `0x30` for the equation `(1 / N) = Q / (2^0x30)`. Dividing by `2^0x30` represents 48 shifts to the right, and `Q` is the quotient decimal we just found. Flipping both fractions, this can be rewritten as `N = (2^0x30) /  0.2887297036378202037`, thus `N = 974873638438446`.

**Flag**  
`HarekazeCTF{974873638438446}`
