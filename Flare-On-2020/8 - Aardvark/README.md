8 - Aardvark

# Challenge 8 - Aardvark

This challenge provides us with the binary `ttt2.exe`, but it fails to execute:
![d0ecb38d0491035e1d644becfca97a08.png](../_resources/289abfbd003a436a82715aace8d0fb39.png)

Looking around in it we find the following string, which suggests this binary might require the Windows Subsystem for Linux (WSL):
![04c6fa138cd2b95a3cfc085635600a81.png](../_resources/de9842cbed8a4bb595bc22958a433e5c.png)

Installing WSL and running the executable finally allows a tic-tac-toe game to start up:
![b453bf7605870592b5b071fc327aa3aa.png](../_resources/04f3c498f22147828ae52b5491ad2f8d.png)

The game is pretty straight forward; since the AI takes the first move by placing an `X` in the center, the game is unwinnable and always results in a loss. A view of Process Monitor shows that the `ttt2.exe` binary creates a socket file at the location below. It also creates a `.tmp` file, which is a Linux ELF binary that is executed using WSL. The two binaries communicate via the socket file, and the AI that we play tic-tac-toe against is running on this Linux binary:
![9ce1f1a20748ebaff40073d32614b21e.png](../_resources/b3efa8ff1db0436290401cd3157d17c5.png)

At the bottom of the Linux binary's `main` function is this `o-eralf@` string, suggesting that reaching the end of the game correctly might produce the flag:
![19a563ed4801529024c56f7a037ef14b.png](../_resources/3f11a2db65ff4413931b2c17b7dee65e.png)

![220709a13b1521dd414abfff3aaedeb9.png](../_resources/37b089921743478b814eec40dede80b1.png)

The Linux binary also reads 9 characters from `/proc/version_signature` in a loop using ```getc```:
![2b8fc633e2376c2cd725b97a4ffd18f5.png](../_resources/aab18dfc7c7f4727ad8a0e2dc90d0bf4.png)

Checking the `/proc/version_signature` ourselves on our WSL install gives the following:
```sh
dan@DESKTOP-POSV4II:~$ cat /proc/version_signature
Microsoft 4.4.0-18362.1-Microsoft 4.4.35
```
`Microsoft` is 9 characters long; if we were on Ubuntu it would return `Ubuntu ...`, so this might also suggest that running WSL and having the signature `Microsoft` is the correct path.

The tic-tac-toe game is set up to be impossible to win, but let's try patching the Linux binary so that it sends an `O` instead of an `X`, guaranteeing a win:

The original assembly which sends `X`:
![8e02f284e6bd5653ee4f9285f3c5b3e8.png](../_resources/ea2e0305cdef4601b538a5bed1ed968c.png)
Patched to send ```O```:
![d74a322d32f5e6d57b816361b8c27f99.png](../_resources/f5a398885c27457688c95bc12c247768.png)

Now let's start the tic-tac-toe server manually by running our modified `.tmp` Linux binary, and connect to it via the socket file using the `socat` command:
![e1f7eaa64143f9253fc16ad7b787516b.png](../_resources/bead6e5743754d80ae76a0145f3a8354.png)
Bingo! Or was it tic-tac-toe.

Flag: `c1ArF/P2CjiDXQIZ@flare-on.com`