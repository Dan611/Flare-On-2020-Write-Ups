# Challenge 5 - TKApp

The `TKApp.tpk` file provided with this challenge is, according to the challenge description, some form of mobile app for a watch. Looking up the format online reveals `.tpk` files can be run on the `Tizen` mobile OS. Closer inspection of the file reveals that it can be extracted as a simple ZIP archive:

![3676d1b6575618d54726dd3b5544f2cd.png](../_resources/3b4d033758b54b258a45363a89a310b5.png)
![159277196829155a3862105de947bdfb.png](../_resources/898e45c24a1a44fc803ee31610b49e1b.png)

The binaries themselves for this challenge are located inside the `bin` folder:
![934a9f5b35f31f415d27f18ee40d2126.png](../_resources/0247d760dfe44c87907519ad5ea07bd2.png)

`TKApp.dll` is the only file with a modification date in this year, and the other files all appear to be helper libraries, so this is likely the primary point of execution. A quick look in IDA lets us know that the binary is a .NET assembly, and reminds us that we don't have the expensive IDA license required to view it!
![21895a10bc4539dc3da1205746c682c9.png](../_resources/12bdb53eceb24a8ea5486fe3a9ac460d.png)
![18c792eaab36e6514ab24995a380e107.png](../_resources/78537dfe7ef7475eb4d5a512e7df79be.png)

Fortunately [__dnSpy__](https://github.com/dnSpy/dnSpy/releases) is better equipped to deal with .NET, and loading in the binary gives us the following overview:
![1d85385546c221d132bb4871679adf86.png](../_resources/2b64387217cf4186a8f79061c24b3934.png)

The first snippits of code that stand out after looking through these C# classes are the following:
![19a5de48c1980badc29af79d2e444bc0.png](../_resources/e1b44ed09b7845c4800fda2fb3b44492.png)

The `Decode` function and `TKData.Password` variable are defined as follows:

![c6b083389f2115cfe01eec76aef4d777.png](../_resources/511acbd019564ed9b3c88102848b5538.png)
![0a74803b670269764c9595bc3413cb46.png](../_resources/95f727ab11b745369ac349c6280c0c70.png)

With a simple XOR loop we can get `UnlockPage`'s password, `"mullethat"`. Checking references to `IsPasswordCorrect` shows us that this password gets stored in `App.Password`:

![cd796aacd28252aa5f590670824697e4.png](../_resources/3bbcc565e41d4951a824882c56d2044f.png)

Tracing the references to `App.Password` leads us to the following `GetImage` function:
![1d19c7b65967e0363d4fe4c088a1a2d5.png](../_resources/9ed624292b9e4a9e82711350e81d1d54.png)

It looks like `App.Password`, `App.Note`, `App.Step`, and `App.Desc` are four components being used to craft some kind of string used to generate a `SHA256` key, so we'll need to find these other three components too.

For `App.Note` we check the references which brings us to the following code snippet:
![fb3832d0091f572b10f79b98bf8d3a08.png](../_resources/d2b7f884176e4c879f049efad5dcf081.png)

The value we want is set at the bottom, and working our way backwards we can see that items from the `todo` variable are only added to `list2` if the `Done` field is `false`. Hovering over `Todo` reveals the following structure, which shows us what we're working with:

![63eb27b680fffefad42a5a271ac91bdd.png](../_resources/ed0ff3586e20429e9a679d09f9f7a102.png)

This means `App.Note` will either be `"and enable GPS"` or `"keep steaks for dinner"`, but it seems likely to be the second one since this one only occurs if `this.isHome` is true, which sounds like some kind of security measure.

Next up is `App.Step`, which is set from the current application's `"its"` metadata field:
![4ca1a14966ffd323f97f970266fe3349.png](../_resources/d1c47a5ba4e844c0972926fe1bf52d41.png)

A quick grep search (in this case [The Silver Searcher](https://github.com/ggreer/the_silver_searcher))  let's us quickly find this metadata key's value, `"magic"`:
![4a97149830cd3e282f6c354b2926fa5a.png](../_resources/b1d58c3bc5a542018caf6fa35d57359a.png)

All that's left is `App.Desc`, which is apparently stored in the `ImageDescription` of `gallery\05.jpg`'s EXIF data:
![4dd337401c2b8273e7c77362208c040c.png](../_resources/893ab39936a94e91a993b8a69a0c3be1.png)

We can use exiftool to reveal that the image description is `"water"`:
![24bd7409d32e9a83e2499b12adb07787.png](../_resources/2b91fccf331641519ffa4668abdc157f.png)

All together we have the following, which we can combine in Python using the same method the app does to create the `text` variable:
```py
>>> Password = "mullethat"
>>> Note     = "keep steaks for dinner"
>>> Step     = "magic"
>>> Desc     = "water"
>>> text = Desc[2] + Password[6] + Password[4] + Note[4] + Note[0] + Note[17] + Note[18] + Note[16] + Note[11] + Note[13] + Note[12] + Note[15] + Step[4] + Password[6] + Desc[1] + Password[2] + Password[2] + Password[4] + Note[18] + Step[2] + Password[4] + Note[5] + Note[4] + Desc[0] + Desc[3] + Note[15] + Note[8] + Desc[4] + Desc[3] + Note[4] + Step[2] + Note[13] + Note[18] + Note[18] + Note[8] + Note[4] + Password[0] + Password[7] + Note[0] + Password[4] + Note[11] + Password[6] + Password[4] + Desc[4] + Desc[3]
>>> print(text)
the kind of challenges we are gonna make here
```

Looking back at the code that created the `text` variable, to complete the challenge we just need to get the `SHA256` of the `text` variable and pass it, along with the byte string `"NoSaltOfTheEarth"` into the following function, then decode the output from BASE64:
![8442cdb05cb00190ba5179cb3996c458.png](../_resources/6fc9b4bf0eec45a6921eca98a8472a4a.png)

Note that the `Runtime.Runtime_dll` file passed into this function as `byte[] cipherText` can be acquired from the app's `Resources` section:

![5b2b8c41ccd6e9d7313015eee043c271.png](../_resources/97b2a19e34114b5f96c029b3ebd1f4cc.png)

Re-implementing the above function (AES decryption) in Python yields the following:
```py
>>> from hashlib import sha256 
>>> from Crypto.Cipher import AES
>>> from base64 import b64decode
>>> key = sha256(text.encode("UTF-8")).digest()
>>> iv = "NoSaltOfTheEarth"
>>> cipherText = []
>>> with open("Runtime.dll", "rb") as file:
...     cipherText = bytearray(file.read())
... 
>>> rijndael = AES.new(key, AES.MODE_CBC, iv)
>>> flag_base64 = rijndael.decrypt(bytes(cipherText))
>>> flag = b64decode(flag_base64)
>>> with open("flag.jpg", "wb") as file:
...     file.write(flag)
... 
62532
```
Opening the output `flag.jpg` file gives us the flag:

![flag.jpg](../_resources/4725a4fc1b64450bbad682c5c1ad8bb6.jpg)

Flag: `n3ver_go1ng_to_recov3r@flare-on.com`