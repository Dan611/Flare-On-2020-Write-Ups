#  import wmi

# takes in a string of hex characters, e.g. '440345034603',
# subtracts each hex pair, i.e '44' - '03', and appends them as a string
# for the example, this would become 'ABC'
def rigmarole(es):
    furphy = ''
    for i in range(0, len(es), 4):
        cc = int(es[i:i+2], 16) - int(es[i+2:i+4], 16)
        furphy += chr(cc)
    return furphy

# XOR encode / decode panjandrum with the key bibble
def canoodle(panjandrum, ardylo, s, bibble):
    kerfuffle = bytearray(s)
    quean = 0
    for cattywampus in range(0, len(panjandrum), 4):
        kerfuffle[quean] = int(panjandrum[cattywampus+ardylo : cattywampus+ardylo+2], 16) ^ bibble[quean % len(bibble)]
        quean += 1
        if quean == len(kerfuffle):
            break
    return kerfuffle

def folderol():
    #  vba script macro, produces 'stomp.mp3' which is not the solution

    #  # decode a list of 12 strings from F.L
    #  onzo = ''
    #  with open('F_L.txt', 'r') as F_L:
    #      onzo = F_L.read().replace('\n', '').split('.')
    #  
    #  #  fudgle = rigmarole(onzo[7])
    #  
    #  #  # check for vbox
    #  #  c = wmi.WMI()
    #  #  twattling = c.query(rigmarole(onzo[8]))
    #  #  
    #  #  for p in twattling:
    #  #      pos = p.Name.lower().find('vmw') + p.Name.lower().find('vmt') + p.Name.lower().find(rigmarole(onzo[9]))
    #  #      if pos > 0:
    #  #          print(rigmarole(6)+': '+rigmarole(4))
    #  
    #  xertz = [0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE]
    #  
    #  wabbit = []
    #  with open('F_T.txt', 'r') as F_T:
    #      wabbit = canoodle(F_T.read().replace('\n', ''), 0, 168667, xertz)
    #  
    #  mf = open('stomp.mp3', 'wb')
    #  mf.write(wabbit)

    # decompiled p-code, produces 'v.png' which is the solution
    buff = bytearray(8)

    # decode a list of 12 strings from F.L
    onzo = ''
    with open('F_L.txt', 'r') as F_L:
        onzo = F_L.read().replace('\n', '').split('.')
    for p in onzo:
        print(rigmarole(p))

    # reverse 'FLARE-ON' into bytearray(b'NO-ERALF')
    firkin = rigmarole(onzo[3])
    n = len(firkin)
    for i in range(0, n):
        buff[n - i - 1] = ord(firkin[i:i+1])

    # use the 8 bit b'NO-ERALF' key to decode the secret data
    wabbit = []
    with open('F_T.txt', 'r') as F_T:
        wabbit = canoodle(F_T.read().replace('\n', ''), 2, 306605, buff)

    # script uses this path
    #  print(rigmarole(onzo[0]) + rigmarole(onzo[11]))
    #  ->
    # 'AppData\Microsoft\v.png'

    mf = open('v.png', 'wb')
    mf.write(wabbit)

folderol()
