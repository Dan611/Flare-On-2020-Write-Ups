# Unknown
**Category:** Reverse Engineering  
**Points:** 200

**Story**  
>Diggin through some old files we discovered this binary. Although despite our inspection we can't figure out what it does. Or what it wants...

Running the command `file unknown` we get:

	unknown: ELF 64-bit LSB executable, x86-64, ...

Running the program with `./unknown` we only get the output `Try again`. Loading the binary into a disassembler, we can see that there are three failure messages, `Try again.`, `Still nope.`, and `Nope.`.

The execution path that avoids the failure conditions first checks to make sure only one argument is being passed into the program, then the argument's length is compared with the preset value 0x38. If these conditions are met the program will begin a simple loop. The main function can be seen to operate with the following logic:
```c
int main(int argc, char **argv)
{
	if(argc == 2)
	{
		char *input = argv[1];
		int len = strlen(input);
		if(len == 0x38)
		{
			int fail = 0;
			for(int i = 0;i < len;i++)
			{
				if(obscurehash(input, i) != 0)
					fail = 1;
			}

			if(!fail)
				printf("Congraz the flag is: %s\n", input);
			else
				puts("Nope.");
		}
		else
			puts("Still nope.");
	}
	else
		puts("Try again.");
}
```
The unknown function `obscurehash()` contains a large amount of obtuse code used to hash a character at a given index in the input argument string. At the end it compares the newly hashed character to a hashed character stored inside the binary.

Since the variable it is printing for the flag is the same variable it used to store the input argument in, a likely possibility is that the unknown hashing function is comparing our input argument to a hashed version of the flag. This can be verified by knowing that previous flags have taken the form `TUCTF{ ... }`, and passing in the argument `TUCTF{aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa}`. The hashing function succeeds for the first 6 characters it hashes, then it sets the failure condition flag to true.

With these facts, one solution now available is to bruteforce the flag by continuously calling the hashing function, changing the character in the string at the current index being checked until the hashing function succeeds, then moving on to the next character. By loading the program into IDA and passing in any parameter, then executing the provided python script `unknown.py`, the flag is obtained and printed out.

**Flag**  
`TUCTF{w3lc0m3_70_7uc7f_4nd_7h4nk_y0u_f0r_p4r71c1p471n6!}`
