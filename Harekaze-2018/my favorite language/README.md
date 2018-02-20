# my favorite language
**Category:** Reverse Engineering + Misc  
**Points:** 200

**Description**  

[foo.lazy](foo.lazy)


We are provided with the file `foo.lazy`, inside of which are the characters `` ` k s i `` repeated over and over. Knowing nothing about the .lazy file format, a quick search reveals that this structure is called __Lazy K__. Running `foo.lazy` with a [Lazy K interpreter](https://github.com/msullivan/LazyK) and then providing input gives the following:

	$ ./lazy2 foo.lazy
	abc
	wrong flag

We know flags have the format HarekazeCTF{ . . . }, but nothing else. Looking inside the Lazy K interpreter source file `lazy2.c` we can see the line

	#define DEBUG_COUNTERS 0 

Setting this to `1` then running some tests reveals the following:

	$ echo -n HarekazeCTF{00000 | ./lazy2 foo.lazy
	wrong flag
		gcs: 0
		news: 1438697
	primapps: 2032299
	partapps: 1277

	$ echo -n HarekazeCTF{10000 | ./lazy2 foo.lazy
	wrong flag
		gcs: 0
		news: 1438511
	primapps: 2032020
	partapps: 1277

	$ echo -n HarekazeCTF{01000 | ./lazy2 foo.lazy
	wrong flag
		gcs: 0
		news: 1438697
	primapps: 2032299
	partapps: 1277

	$ echo -n HarekazeCTF{00100 | ./lazy2 foo.lazy
	wrong flag
		gcs: 0
		news: 1438697
	primapps: 2032299
	partapps: 1277

	$ echo -n HarekazeCTF{11000 | ./lazy2 foo.lazy
	wrong flag
		gcs: 0
		news: 1438511
	primapps: 2032020
	partapps: 1277

	$ echo -n HarekazeCTF{10100 | ./lazy2 foo.lazy
	wrong flag
		gcs: 0
		news: 1438511
	primapps: 2032020
	partapps: 1277

The __news__ and __primapps__ fields appear to be changing based upon the input field, but they are the same across some of the tests. The __news__ field in particular is increased every time `alloc_expr()` is called in the Lazy K implementation, and we can see that all tests that start with `HarekazeCTF{0` have the __news__ value `1438697`, while all tests starting with `HarekazeCTF{1` have the __news__ value `1438511`.

Thinking about how the original source code may have been laid out, one possible conclusion is that the code checks each character in the input flag, and if it sees it is wrong, it will cease execution and print `wrong flag`. For the Lazy K interpreter, this means that the more correct flag characters that are present in the input string, the more code it will execute and the bigger the __news__ field will be.

Changes are made to the lazy2.c source file main() function loop like so and it is compiled as lazy2_mod:

		for (;;) {
			check(1);
			int ch = church2int(car(*toplevel_root));
			if (ch >= 256) {
				printf("%d", news);
				return ch-256;
			}
			//putchar(ch);

Like this, the only thing the Lazy K interpreter will print out is the __news__ field at the end of execution. Next, we continuously pass flag values until we find the character that produces the highest value __news__ output, then move on to the next character:

	$ echo -n HarekazeCTF{0 | ./lazy2 foo.lazy
	$ echo -n HarekazeCTF{1 | ./lazy2 foo.lazy
	$ echo -n HarekazeCTF{2 | ./lazy2 foo.lazy
	. . .

We will eventually have the highest news value possible, meaning the program read the entire flag string. Automating this process with [lazy.c](lazy.c) gives the output `HarekazeCTF{4AD8AA3A3571EA912A6EC5EA5FDCC93C0000000000000000000000000000000`, which appears to have worked up until the repeating 0s, likely indicating the end of the flag. Testing this flag with the Lazy K interpreter and shows that the flag is correct

	$ echo -n HarekazeCTF{4AD8AA3A3571EA912A6EC5EA5FDCC93C} | ./lazy2 foo.lazy
	correct flag

**Flag**  
`HarekazeCTF{4AD8AA3A3571EA912A6EC5EA5FDCC93C}`
