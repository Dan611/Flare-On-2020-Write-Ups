# Challenge 10 - break

This challenge simply provides a file called `break` and the following comforting message:
```txt
As a reward for making it this far in Flare-On, we've decided to give you a break. Welcome to the land of sunshine and rainbows!
```
Although that's not very reassuring, taking a peek at the file reveals it to be a 32-bit ELF Linux executable. Let's look at the program actually executing. We enter the password to the previous challenge, and are then told that `sorry i stole your input :)` is not the correct password:

![c427bdbe3ef7df7e5b93c88ca9a2240e.png](../_resources/748e16ffeb1d47198df2acf61c6143ad.png)

Hmm...  In IDA the function call that stands out the most is `ptrace`, which is called 57 times!

![484fa3d1bedcd6e7e9b0a29fe0535ff0.png](../_resources/09cab9e470024be1a800fd3eed348c6a.png)

It's important to realize just how versatile [ptrace](https://www.man7.org/linux/man-pages/man2/ptrace.2.html) can be, so here are some of the arguments to it we might expect to see:

```txt
PTRACE_ATTACH   - Attach to the target process
PTRACE_PEEKDATA - Read data from the target process
PTRACE_POKEDATA - Write data to the target process
PTRACE_GETREGS  - Get the target process registers
PTRACE_SETREGS  - Set the target process registers
PTRACE_CONT     - Continue target process execution after an interrupt
```
With just these arguments you could potentially write a fully-fledged debugger, which seems likely considering how many calls are occuring. The process has two main forks which then attach to their parent and act like debuggers, intercepting certain calls and signals. Here is the first `fork` call, and it is interesting to note that this is called from `init`, before the process's `main` function even has a chance to execute:

![6de1f23f07e879f56799bee718377586.png](../_resources/4f170fc43a92413cb5a392c0792230c8.png)

![4408e62449f827bc26f6164157870a9c.png](../_resources/9c352b7840964da79b81090ac6ef349c.png)

Now we have the original parent process, as well as a fork that has attached to it using `ptrace`. Also note the `_nanosleep_` call, which will give the forked child process some time to attach before the parent resumes executing after the fork call. Some similar code is called from this child fork, where a second fork is created that then attaches itself to the first fork:

![4324899e86dc76f1b97c357ac0ccfb13.png](../_resources/42e9f90fb9ba4bec8a60279bd6827ca4.png)

![e8591eddc49d5f6c90f99348c9f89f6f.png](../_resources/bc5f2fda970c4ea89c7ffe153ea43128.png)

![914939b9d8b6f7c0137385ecef8306fe.png](../_resources/7289a30f93154d1092684726e146ebea.png)

It's become a bit convoluted, but we now have our original parent process, a primary fork of the parent that has attached to the parent as a debugger, and a secondary fork of the primary fork that has attached to the primary fork as a debugger. Each of these debuggers can intercept certain calls or interrupt signals, then read and write to memory and registers.

Here's a snippit showing the primary fork load the parent process's registers onto the stack by passing `PTRACE_GETREGS`, followed by the register structure it's using:

![79dde262a4fbe5bdee1b718bdddf3284.png](../_resources/0533ca46a4fa46dc9aee7d6eceb17ef3.png)
```c
struct user_regs_struct
{
  long int ebx;
  long int ecx;
  long int edx;
  long int esi;
  long int edi;
  long int ebp;
  long int eax;
  long int xds;
  long int xes;
  long int xfs;
  long int xgs;
  long int orig_eax;
  long int eip;
  long int xcs;
  long int eflags;
  long int esp;
  long int xss;
};
```
Since the registers were loaded at `ebp-0xCC`, and `(0xCC - 0xA0) / 4 = 0xB = 11`, we can see it's referencing the 12th (offset 11 from the start) struct entry ```orig_eax```. It's important to note that if `ptrace` is used to intercept syscalls, `orig_eax` will contain the number of the syscall. We can see the program do some quick obfuscation to `orig_eax` then procede to compare it to several values to determine its path of execution:

![3c84679bc1c8b2401c670488f605a9b5.png](../_resources/f218f70d60964a63bb52b00558ab7eda.png)

The following bash script will generate the obfuscated codes for every Linux 32-bit syscall, which can be cross-checked in the Linux source code at [syscall_32.tbl](https://github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_32.tbl).
```sh
for i in {0..335}
do
	printf "%3d: %08X\n" \
		$i               \
		$(((($i ^ 0xDEADBEEF) * 0x1337CAFE) & 0xFFFFFFFF))
done
```
Here's some examples, with the syscall number on the left and the obfuscated value and its corresponding name appended on the right:
```txt
  2: B82D3C24 sys_exit
  4: 91BDA628 sys_read
 12: F7FF4E38 sys_execve
 16: AB202240 sys_chmod
 55: 2499954E sys_ioctl
 93: 4A51739A sys_truncate
 97: 9678E7E2 sys_getpriority
 98: 83411CE4 sys_setpriority
123: 09C7A9D6 sys_newuname
153: C93DE012 sys_mlockall
218: E8135594 sys_pivot_root
```
This means, for example, that the comparison at the bottom of the IDA snippit above, `cmp eax, 0E8135594h`, is checking to see if the syscall the parent process was attempting to execute before the fork debugger intercepted it was `sys_pivot_root`, or something else. In this way, the parent process can make seemingly random calls like `call _nice`, `call _execve`, or `call _truncate`, and the debugger can arbitrarily re-route execution. The parent process becomes more like a program operating in a virtual machine, where syscalls lead to VM handler functions. 

One example VM function is the following, which occurs when the parent attempts a `sys_read`. The parent's `main` function prints some prompts then attempts to read from `stdin` to get the user to input the flag. The flag is then checked against a string, and the process exits:
![11209b5c5e2f68d82180628bfb9138f0.png](../_resources/6295d5acedef446a99fb12819c807234.png)

![d8217aa733df4dede28b92717bfd2769.png](../_resources/5c50b9220ed646fbb0729ee49417a38e.png)

We know that the program doesn't actually seem to work so simply though, because somewhere along the way our input was overwritten. The marked `_read` operation above in the parent triggers the following VM handler function in the primary fork:

![44e34fd26ed3bfbd0d2fb7278218aebb.png](../_resources/e6644d8207274d80ab505fea6a0ab1ae.png)

What is happening? First, the fork calls `_fgets` to actually acquire the user input. Next, `0xB8` is passed into a `string_decryptor` function, which returns a decrypted string stored at a certain index in the binary. This string is then written into the parent process's memory using `write_mem_to_pid`, which is simply a wrapper for `PTRACE_POKEDATA`.

With the following quick bash script we can modify the `0xB8` byte to be anything between `0x00 - 0xFF`. It then executes the modified program, which will replace that `sorry i stole your input :)` string we saw earlier with some other decrypted string. We can use the program output to form a list of decrypted strings, which are placed below. 

```sh
#!/bin/bash
for i in {0..255}
do
    cp break broken
    printf \\$(printf "%03o" $i)\\$(printf "%03o" $(($i >> 8))) \
        | dd of=broken bs=1 seek=5024 conv=notrunc status=none
    printf "%02x:\n" $i
    ./broken <<< "" &
    pid=$!
    (sleep 0.2; pkill -SIGKILL broken) &
    killer=$!
    wait $pid
    kill $killer
    printf "\n"
done
```
```txt
a1: I hear Wolfram Alpha is good at doing things with big numbers.
a2: There are two hard things in computer science: caching, off by one errors, and comparing really big numbers.
a3: Like a phoenix, I rise from the ashes
a4: This string has no purpose and is merely here to waste your time.
a6: There are many lessons to be learned by studying the Bee Movie.
a7: This challenge is so easy it exploits itself.
a8: moc.eralf-on@gn1hs1n1f_n0_st4rgn0c
a9: elf_0n_a_sh3lf@on-flare.com
ab: TracerPid: %d
ac: fake_flag@no-flare.com
ad: sm0l_bin_b1g_h34rt@no-flare.com
ae: okay_1_sw34r_th1s_1s_th3_r34l_0ne@no-flare.com
af: not_a_fake_flag@no-flare.com
b2: /proc/%d/status
b8: sorry i stole your input :)
b9: OOPSIE WOOPSIE!! Uwu We made a mistaky wakey!!!!
ba: This string also has no purpose and again, is here merely to waste your time.
bb: This string is used to calculate the XXTEA key to decrypt the fourth part of the flag.
bc: I HAVE THE CONCH DON'T INTERRUPT ME
be: winners never quit
bf: TracerPid:
```
Aside from some fake `@no-flare.com` flags, there's nothing too interesting in there except for the comment about using XXTEA to decrypt part of the flag.

Something else to note about the second debugger (the second fork attached to the first fork), is that although it is small and handles significantly less VM functionality than the primary debugger, its existence means we can't debug the first fork at all. We can completely patch it out however, and with some cleanup making sure execution will be the same as before, we can now debug the first fork:

Before:

![251df299f55f5887475b9a502cb1f89e.png](../_resources/430ccafef2b14a7088e3002a1f718910.png)

After:

![f806896922c31ae02b6e13277b112f21.png](../_resources/23246300d4e141f8b8baba7c0f3902c2.png)

The parent's `main` function still wasn't doing anything that would lead us to the flag though, but note the following action the primary fork takes. The ASM bytes for a UD2 breakpoint are written to the beginning of the `flag_fhecker` function, so that the parent will trigger a `SIGILL` signal to the primary fork debugger:

![db2f6ad5b7a3fc05c0544599dea06ea7.png](../_resources/ff1aa64f2a624694ae0a7939c318699b.png)

At the bottom of the primary fork debugger handler, the program specifically checks for this signal. The first thing it does is write the input string it intercepted back into the parent process at a known location, then change the instruction pointer to an entirely new function:

![55d9b6fe28e7ac6e1330b264ba9e76b8.png](../_resources/a219b39e3ee9474faaf3821c228f42ec.png)

Afterwards, the parent process will now resume execution here:

![4e233f6ce71db20077a342f15bcccad5.png](../_resources/f7f9b29ca5434828bb74cabf49909d41.png)

The parent process then makes some VM calls and calls some decryptor functions, and finally compares our input string to what it decrypted. A trick here is that if we modify the binary to re-route `_memcmp` calls to `puts`, and NOP out the ```push [ebp+input_string_ptr]``` so that the decrypted data pointer is at the top of the stack, we can see what its trying to compare against:

![8524cf4a96617968afd9210ae16b668f.png](../_resources/3605e99ae07c4fa584b4c4767bd52bf8.png)

![2a69c640d96a534c3b9db71f2af2eb01.png](../_resources/faf29d4be9524cf0be1c0cb3860499c6.png)

![be3d69377279e568bb6a671c3826e951.png](../_resources/79f14d22a17147efac31cf95cf5b734b.png)

This appears to be the first part of the flag. Execution procedes to the following function call:

![904cec7f180a405170400f2b68d57a3e.png](../_resources/45238fe81f3947108afb36cfc1db48fb.png)

We can see it's looping over our input string, then using it somehow with a `_truncate` VM call:

![f3c10aa6793df8a1aa7e1a4d29a72580.png](../_resources/5fdb389b2d01455180038c9817634d37.png)

As was hinted to us in one of the decrypted strings, this part uses something similar to XXTEA encryption to encode our input bytes. It uses lots of syscalls which get handled in the primary fork debugger, some of which call NULL pointers themsleves, which then get handled by the second debugger!

Even for simply looping, the parent proces calls a NULL pointer, which sends a `SIGSEGV` signal to the primary fork debugger (`call eax` below):

![64dd2fc64268dc4883d06814aea4e14c.png](../_resources/4f85708574a545338834fbc56267e94f.png)

Here is where it's handled in the primary fork debugger, where the `i` loop count variable is incremented, and we loop if `i` is less than 16, othwersie we return from the call and exit the loop:

![ef48476061214fda6c60f4d790c90b85.png](../_resources/ae453e77fe164240af4da95dadc59198.png)

We can re-implement this ASM labyrinth in Python like so:
```py
def xxtea(data, key, encrypt):
    for z in range(0, len(data), 8):
        dword1 = int.from_bytes(data[z:z+4], "little")
        dword2 = int.from_bytes(data[z+4:z+8], "little")

        r = []
        if encrypt:
            r = range(16)
        else:
            r = range(15, -1, -1)
        for i in r:
            tmp = dword2
            dword2 += key[i*3]
            dword2 &= 0xFFFFFFFF
            dword2 = (dword2 >> (key[i*3+2] & 31)) | (dword2 << (-key[i*3+2] & 31))
            dword2 &= 0xFFFFFFFF
            dword2 ^= key[i*3+1]
            dword2 ^= dword1
            dword1 = tmp

        data[z+0:z+4] = dword2.to_bytes(4, "little")
        data[z+4:z+8] = dword1.to_bytes(4, "little")
```

Then we can plug in this encoded value that it is comparing our XXTEA encrytped input to:

![c514b8d934d183a3a7c7431a877645df.png](../_resources/d52e4a7ab4e5404b8edb3e773c4bff57.png)

![5709299f317860661ac093813ecc77c4.png](../_resources/ea776ff15b104ed8886e942685b2353c.png)

Running this data throuugh the decryption function yields the second part of the flag:
```py
>>> bin = []
>>> with open("break", "rb") as file:
...     bin = bytearray(file.read())
... 
>>> flag2 = bin[0x0015C100:0x0015C120]
>>> xxtea(flag2, key, False)
>>> print(flag2.decode("ascii"))
4nD_0f_De4th_4nd_d3strUct1oN_4nd
```

We still don't have the full flag, but if we look back at the loop that was comparing our XXTEA encrypted input to data in the binary, something that stands out is how long the loop continues on for, because the flag certanly isn't `39999 (0x9C3F)` bytes long:

![75c0eb3af5496f2b35a2a365c8cadc8b.png](../_resources/fc09e3c9ca244593a789e7b934eaa5a8.png)

In fact, this loop is much larger than the size allocated for the stack buffer, and if it goes too far, it will start corrupting the stack:

![56e27eb671f5959c4f06ef05bab8b6c7.png](../_resources/5cdae76b42784dc4891a8155978fbf62.png)

If we breakpoint at the end of the loop we can verify that EDX is now pointing at a variable well beyond the stack buffer:

![a03e2c9b6a8d7ab70772dd4fc23dd76b.png](../_resources/8054698214ac4e38867c9bb8e4bfe734.png)

This `0x28` offset belongs to the stack variable I've named `SIGSEGV`. We can see that usually it contains 0, and if the primary fork were to call this variable, it would trigger a `SIGSEGV` signal and be handled in the secondary fork debugger:

![51df06bd4ab1980ec9e28029bc66f323.png](../_resources/4cf078affcae478d91f1905a40c60fb2.png)

What is being written into this variable? Apparently, the offset `0x08053b70`:

![d66b93a0a5aad9047857117a548f4ff2.png](../_resources/a3b26b3002fc4401adf8a500c7ffebce.png)

Normally, if we were to look at this address we would see nothing of interest:

![0bc828748f72214912a01de7cbe34da8.png](../_resources/9107ce0591d348c5a6865eb2799391cd.png)

### . . .

![be9ecf8a5d219488f3004c3e4e1592cc.png](../_resources/7a27751ba1964111a24577effe368710.png)

However, when the XXTEA encryption was running madly through all that memory, the bytes at this address became actual code. More specifically, the bytes became a full transcript of the Bee Movie with some code inside of it... Note the `00` byte after the highlighted little-endian address, because the code stops copying data when it hits that zero, so it doesn't destroy too much of the stack:

![1501e27fba2daccbcc37622235faa760.png](../_resources/df99bed030f34800957043273008997c.png)

In the memory dump we can see the region where it transitions from the Bee Movie script into actual 32-bit ASM as well:

![048a5b6584e7356d6b7771b2a5acf101.png](../_resources/539ba9e5894b4bb99aefb7dbbcf63957.png)

Now the code procedes to call the value in the `SIGSEGV` variable, which used to be 0 and would have been triggered a `SIGSEGV` error signal to be intercepted by the secondary fork debugger, but is now pointing into the unencrypted data region, which contains hidden ASM:

![2cf8bb4fbac7389cde06a7ec9f763ebd.png](../_resources/f4d7d5255dbb4a4d9160220d6cd1bc00.png)

Inside this secret code, it first verifies that we have correctly matched the first 32 (0x20) characters of the flag:

![ab2f0ef46fb81a2564903295730dcb94.png](../_resources/bbceeae8592f41d1a135bca45ba6cbf7.png)

This last bit requires more ingenuity than brute force analysis; there are upwards of 20 function calls, most of which are different from each other and can go more than 5 nested function calls deep, or contain triple nested loops. The quickest way to figure these out is by debugging them and seeing their inputs and outputs, but remember it's impossible to debug this process without first stripping out the second fork so that we can attach to the first fork, which includes making sure all of its VM functionality is properly replicated.

Anyways, we can see the first three calls are zeroing out memory, whereas the next batch of calls takes in hex strings stored in the decrypted movie script as plain text and converts them into actual bytes on the stack:

![43a94b633c09fc32f332201ff13cc367.png](../_resources/3472cd67d80c4c3b9bc05a71560b180d.png)

Next it does some extremely large multiplication, division, and subtraction using these enormous numbers, along with the bytes from our input and some random numbers for obfuscation:

![dbff68fd50f67d7d9fe797b05c303da5.png](../_resources/c466230375264fc9a0d8c6ec7754b004.png)

Here's one of the math operations, so we can see just how messy the library it's using can look in ASM:

![51d02d7cac5e8cba1bb91a5cb7be3473.png](../_resources/f5a14ecb0bed4f89a7df31b00cdfa643.png)

![96eb0e25fc577fa455ab87929f39a8a2.png](../_resources/730171ce4c8b4aaf8aa692b8ed51747e.png)

We see lots of subcalls inside all of those nested loops, but if you look at the inputs and outputs of the function you can see that the final output is divisible by the inputs, so we can determine it was a multiplication function.

One of the inputs into these huge math operations was our input flag, and when all these math calls are finished, it compares it to what is likely the final part of the flag, before allowing the program to succeed:

![fad61b896ebfe282e7ac5f5bc620cdc7.png](../_resources/c409057100eb4c74a649641c1fd3b6b6.png)

If we unravel the math operations it does, the end result is essentially

```py
encrypted_input = (input_bytes * a) - ((input_bytes * a) // m) * m
```
where `input_bytes` are the bytes of the string we input into the program, `encrypted_input` are the bytes after the large math operations, `a` and `m` are some of those large hex strings from above, and `//` is the floor-rounding integer division operator. Since it compares this directly to `encrypted_flag`, we know that if we substitute in `encrypted_flag` for `encrypted_input` in the equation, then we can solve for `input_bytes` and get the flag!

The equation then becomes:

```py
encrypted_flag = (flag_bytes * a) - ((flag_bytes * a) // m) * m
encrypted_flag = (flag_bytes * a) %  m
flag_bytes * a ≡ encrypted_flag (mod m)
```
We can use Python 3.8's `pow()` to calculate the multiplicitive inverse of `a` for `m`, then solve for `flag_bytes`:
```py
i = pow(a, -1, m)
flag_bytes = (i * encrypted_flag % m)
```

Now we just plug in the values from the decrypted data section:
```py
>>> encrypted_flag = 0xd036c5d4e7eda23afceffbad4e087a48762840ebb18e3d51e4146f48c04697eb
>>> m = 0xd1cc3447d5a9e1e6adae92faaea8770db1fab16b1568ea13c3715f2aeba9d84f
>>> b = 0xc10357c7a53fa2f1ef4a5bf03a2d156039e7a57143000c8d8f45985aea41dd31
>>> flag_bytes = (pow(b, -1, m) * encrypted_flag % m)
>>> print(hex(flag_bytes))
0x6d6f632e6e6f2d6572616c6640733369707075705f306e5f
>>> print(flag_bytes.to_bytes(24, "little").decode("ascii"))
_n0_puppi3s@flare-on.com
```
Let's collect all the pieces of the flag we have now and put them together:
```txt
1: w3lc0mE_t0_Th3_l
2: 4nD_0f_De4th_4nd_d3strUct1oN_4nd
3: _n0_puppi3s@flare-on.com
```

Flag: `w3lc0mE_t0_Th3_l4nD_0f_De4th_4nd_d3strUct1oN_4nd_n0_puppi3s@flare-on.com`

&nbsp;

Click here for the Python 3.8+ script to generate the flag. Note that it requires the `break` binary, which it reads data from:
[rainbows.py](../_resources/rainbows.py)