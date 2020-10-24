# Challenge 9 - crackstaller

This challenge can get a bit messy, so here is a general overview of its components to refer back to:
### Components
* __crackstaller.exe__ - Provided executable that extracts the remaining files
* __credHelper.dll__ - COM server
* __cfs.dll__ - Capcom's exploitable Street Fighter V rootkit
* __driver.sys__ - Filter driver scanning for registry changes


Immediately when we extract `crackstaller.exe` to begin the challenge, Windows Defender alerts us:
![76dcd899309074e56ca8b81895699a93.png](../_resources/8e76ccfdda7f4441a1fa8d0fd9295b77.png)

A quick search of this virus signature name reveals that Capcom's anti-cheat rootkit was detected inside the binary. If we follow the process execution in ProcMon, we can note the following:

![d2708587683caeb24660eee70d77dd89.png](../_resources/b5206464065d47ea9d098163abb844eb.png)

![4c5e5210ab5b993a43d510c47064cca7.png](../_resources/fe85c281ef574c22af9034390975f299.png)

`crackstaller.exe` is extracting a file called `credHelper.dll`, then using that .dll to make some interesting registry additions by registering it as a COM server. Next up `crackstaller.exe` is creating a new kernel module `C:\Windows\System32\cfs.dll`. This could be the rootkit detected earlier. Nothing else interesting seemed to happen, and the process closed.

![0fcc6f01dfbfa0b64b33019ef31537bf.png](../_resources/e747e0dcf465422aa1f3c76f1d4edecb.png)

![d01cae707d67b5cdddbd63601e516be7.png](../_resources/a0861ea9b4bf4e56977dd86bbb637503.png)


If we check the SHA-256 of `cfs.dll` on VirusTotal, we can confirm this is Capcom's rootkit:
![f38558b585b4d9e14ccee0a2bf60b6fe.png](../_resources/fd50aa6061af41f08f8d476cd1196d42.png)

The `DeviceIoControl` import in `crackstaller.exe` is used to communicate with the rootkit:
![70a01f6e22614edd72fc98c1315d795c.png](../_resources/44f5a501aee246018ba7154f06b32501.png)

It allocates a buffer containing some ASM necessary to jump back into `crackstaller.exe` from the rootkit, but with the priviledged execution of kernel mode. It jumps to `crackstaller.exe` at `sub_140002A10`, which I have renamed `called_from_kernel` below, where it uses byte signature scanning to find NT functions:

![7fa7e252124df94dccce9ee442f53845.png](../_resources/1b6290909fed4148b2363712989c2ed0.png)

Next `crackstaller.exe` allocates a kernel pool, then copies `driver.sys` into it. After that, it scans for some specific bytes and patches them:

![3519a16e34fa56bd53b0c10061827acc.png](../_resources/fcf150e665d243b088eacca2a71d8381.png)
![9490e081a2d8a97408368f4c1f75bfc3.png](../_resources/a53b146029f947d6a7fa399a6351cde5.png)

If this patch doesn't occur, later string decryption fails, so this obfuscation is important to take note of. Finally, it creates a system thread at `driver.sys:DriverBootstrap`, where the filter driver loads itself into memory and begins execution:
![7680814d4d175930bfaeb73f6bcd21d7.png](../_resources/7bb9c93009e841399716bdcf1b38a116.png)

Once `driver.sys` is able to call its `DriverEntry`, it creates a device driver to register a `RegistryCallback` routine through a call to `CmRegisterCallbackEx`, which will scan for registry changes:
![b493be03af8b6b7ddb36b6ffa70482da.png](../_resources/76bb0dd182ae43649016b3876e089caa.png)

Inside the callback, the filter driver scans for a specific registry key using `wcsstr`:
![5d08b3c4714a75f9d478844913896ec8.png](../_resources/928152cdbe484d33b6a82b65630cf8c7.png)

If the string is a match, `driver.sys` does some decryption to retrieve the password `H@n $h0t FiRst!`. Note the `BBACABA` byte string it uses in the decryption. This was set earlier in `crackstaller.exe` before calling `driver.sys`:
![bf18f9e330c9c8118292f7694689b4f3.png](../_resources/539a42174252404994756cc7e9c513d2.png)

The password is expanded into a `wchar_t`, then passed as the `Class` parameter to `ZwCreateKey`:
![7d16a8dc7b52524f396564bfa3296d63.png](../_resources/bec6f63172bf41bca086b4cfa38c4e4a.png)

The driver then stops monitoring for registry changes by calling `CmUnRegisterCallback`. So we have some kind of password, apparently the "class" of the registry key `Computer\HKEY_CURRENT_USER\Software\Classes\CLSID\{CEEACC6E-CCB2-4C4F-BCF6-D2176037A9A7}\Config`. We need to see if we can interract with the COM server that was set up. Taking a look inside `credHelper.dll`, note the following two functions that interact with the registry keys found earlier:

![f92f2eaf7b87c5d08c608fec1d2322d5.png](../_resources/e308a33f1e2a46f5859998a23909c560.png)
![911e404859527c8796cfc7523661d1ef.png](../_resources/52aa8e5d542545b18140e881227bff01.png)

Inside the binary we can also find the bytes for the CLSID being used earlier, as well as an antirely new one:

![70041e5fd39683b9633e9331e4504f95.png](../_resources/7e72b6aa95c64035a3953aa2e311331f.png)
```sh
CLSIDs:
{CEEACC6E-CCB2-4C4F-BCF6-D2176037A9A7}
{E27297B0-1E98-4033-B389-24ECA246002A}
```

Using some C code to interface with the COM server through `CoCreateInstance()` and the second CLSID, we are returned a pointer to a C++ vtable:

![d3a7ea214a7067cb33eb6fe3ad86eb09.png](../_resources/032ed076ecee44e18ea5d09b5c1d1ebb.png)
Comparing the last 4 digits of the function pointers (`14b4`, `1508`, ...) to the offsets in IDA in the image above confirms they are the same functions. Now, passing a buffer to the two functions, first to the one that reads the `Password` registry key, then next to the one that sets the `Flag` registry key, the code is able to decrypt the flag using the password and save it to the registry:

![ed7f3b32fd76d562cffcc5e30b121878.png](../_resources/09ef67308b214ff8bdfbd2de0a1511c8.png)

Flag: `S0_m@ny_cl@sse$_in_th3_Reg1stry@flare-on.com`

&nbsp;

Click here for the COM server interfacing C code:
[com.c](../_resources/com.c)