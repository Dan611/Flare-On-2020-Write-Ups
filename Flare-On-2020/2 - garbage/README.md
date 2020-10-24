# Challenge 2 - garbage

To start off this challenge, trying to run the provided garbage.exe yields:
![365719b5d92f286e77a7ce691374d99b.png](../_resources/8a7f3d33c7a24c96a36ac9293657d5ba.png)

Viewing its PE header in CFF Explorer we can see that it was compressed with UPX:
![c3611b4b43ee01b137a3ec4cd7c479cc.png](../_resources/9a9b268ba55d48f9ad1f33e19d1c830a.png)

However, running upx -d garbage.exe to decompress it yields the following error:
![cc22fdb6279faf87ae4b925f7baa697f.png](../_resources/cdedb6afa6674128be4293127009100b.png)

Looking back at the resource directory, the ```Raw Address + Raw Size = 0xA200```, however the actual .exe file is only ```0x9F24``` bytes. By extending the image ```0x2DC``` bytes to make it ```0xA200``` bytes large like so, UPX will work:
![f0c2c9cea1c1a8506d30424f0267ada2.png](../_resources/ec7a9639acde4ec293dfad581dce1de3.png)
![74fddc9dd32e9413b1fa87028b38389f.png](../_resources/9da652d6842147afb5b2e780c7810532.png)

However the image will still not run:

![43cda75109e84dbae76b63d8a501b58e.png](../_resources/cdfcb81e280043a6b8e162669c9d9b9a.png)

Looking at the side-by-side configuration at the bottom of the last hex dump, we can see that the XML file is incomplete. Replacing it with a complete version of the correct size (```0x17D``` bytes, retrieved below again from CFF Explorer) will remove the previous error:

![e232cc93474cd423acf7f66de12c1e50.png](../_resources/719b57741627436a9c4fa63c8b2a120a.png)

```xml
<?xml version='1.0' encoding='UTF-8' standalone='yes'?>
<assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>
   <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
     <security>
        <requestedPrivileges>
          <requestedExecutionLevel level='asInvoker' uiAccess='false' />
        </requestedPrivileges>
     </security>
   </trustInfo>
</assembly>
```

But now we have yet another new error:

![ea8fd8647820051908d367be4305fe80.png](../_resources/ad2121874788499eac9737f34ca9caf9.png)

Checking the import table, we can see that the .dll names are missing:
![8f1e63035f594a36956f89915255d63b.png](../_resources/2c61e371a2774088b267e3f322157f04.png)

Adding them in fixes the .exe and it will now run correctly:
![f721669f628ea431c6955a7d86dd548a.png](../_resources/4303df3f0be045e4b37bfc1fc6305856.png)


![9533c2a9fd248c152cf3c705f5306729.png](../_resources/9027604f3f054d7cb6b03a01cd8f640e.png)

Flag: `C0rruptGarbag3@flare-on.com`