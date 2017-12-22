# Future
**Category:** Reverse Engineering  
**Points:** 250

**Story**  
>Future me gave me this and told me to add it to TUCTF. I dunno, he sounded crazy. Anyway, Let's see what's so special about it.

Inside the file `future.zip` are the two files `future` and `future.c`. Running the command `file future` we get:

	future: ELF 32-bit LSB shared object, Intel 80386,

Running the program with `./future`, we are asked for the flag and unable to provide it. Opening `future.c` we see a 5x5 2D array generator function `genMatrix()`, a function `genAuthString()` that takes values from the array and puts them into an `auth` string variable, and the `main()` function which calls these functions and compares the `auth` string against a pre-defined variable:

`char pass[19] = "\x8b\xce\xb0\x89\x7b\xb0\xb0\xee\xbf\x92\x65\x9d\x9a\x99\x99\x94\xad\xe4\x00";`

We see that the program takes in a 26 length zero-terminated string called `flag`, and uses the values of chars in the strings to generate the 5x5 matrix. Thus, there exist two strings `flag` and `pass`, where we provide the string we are looking for, `flag`, and the string it must be transformed into, `pass`.

To start, we need to know how the char values being put into the matrix are used. Using the same logic provided in future.c, using Python we can generate a 5x5 matrix where each value in the matrix is the index of the character future.c would be putting into the matrix from the input argument string:
```python
mat = [[0] * 5 for x in range(5)]
for i in range(25):
	m = (i * 2) % 25
	f = (i * 7) % 25
	mat[m/5][m%5] = f

for row in mat:
	print row
```
This outputs the following 2D array matrix:

	[0, 16, 7, 23, 14]
	[5, 21, 12, 3, 19]
	[10, 1, 17, 8, 24]
	[15, 6, 22, 13, 4]
	[20, 11, 2, 18, 9]

We can determine from this output, for example, that `mat[0][0]` in future.c would contain the value of the first (index 0) char in the input string, and `mat[4][4]` would contain the tenth (index 9) char in the input string. Thus the first line in `genAuthString()`, `auth[0] = mat[0][0] + mat[4][4];`, would sum the values of the first and tenth chars in the input string and store them in `auth[0]`.

To solve this problem we can use the fact that previous flags have used the form `TUCTF{ ... }` to determine the rest of the values in the string. For example, we know `mat[0][0]` would contain the char `'T'`, and we know `auth[0]` is pre-defined as `'\x8b'`. Thus, we can rewrite the first line mathematically as `auth[0] = mat[0][0] + mat[4][4]` -> `mat[4][4] = auth[0] - mat[0][0]` -> `'\x8b' - 'T' = '\x55' = '7'`. So `mat[4][4]` contains the value `0x55`, and the tenth character in the flag string is `'7'`.

The included Python script `future.py` follows this exact logic to automate the process and calculate the flag. Taking the generated flag and passing it into the program confirms that it is correct.

**Flag**  
`TUCTF{5y573m5_0f_4_d0wn!}`