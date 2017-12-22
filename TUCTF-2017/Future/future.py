# password from future.c
passcode = "\x8b\xce\xb0\x89\x7b\xb0\xb0\xee\xbf\x92\x65\x9d\x9a\x99\x99\x94\xad\xe4\x00"
# flag containing known characters and placeholder characters
flag = list("TUCTF{\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00}")

# create a matrix that maps positions in the matrix used in future.c's genMatrix() to indices in the flag string
mat = [[0] * 5 for x in range(5)]
for i in range(25):
    m = (i * 2) % 25
    f = (i * 7) % 25
    mat[m/5][m%5] = f

#for row in mat:
#    print row

# create tuples to logically link different matrix positions together based on future.c's genAuthflag() logic
tuple = [None]*18
tuple[0] = ((0, 0), (4, 4))
tuple[1] = ((2, 1), (0, 2))
tuple[2] = ((4, 2), (4, 1))
tuple[3] = ((1, 3), (3, 1))
tuple[4] = ((3, 4), (1, 2))
tuple[5] = ((1, 0), (2, 3))
tuple[6] = ((2, 4), (2, 0))
tuple[7] = ((3, 3), (3, 2), (0, 3))
tuple[8] = ((0, 4), (4, 0), (0, 1))
tuple[9] = ((3, 3), (2, 0))
tuple[10] = ((4, 0), (1, 2))
tuple[11] = ((0, 4), (4, 1))
tuple[12] = ((0, 3), (0, 2))
tuple[13] = ((3, 0), (2, 0))
tuple[14] = ((1, 4), (1, 2))
tuple[15] = ((4, 3), (2, 3))
tuple[16] = ((2, 2), (0, 2))
tuple[17] = ((1, 1), (4, 1))

# for each character in the passcode, check if the corresponding flag character is defined; if this is the case,
# we can subtract the value of the flag character from the passcode character to find another flag character
for i in range(7) + range(9, 18):
    # these tuples have 2 flag characters mapped to them, so we need to check both
    for n in range(2):
        n1 = (n + 1) % 2
        if flag[mat[tuple[i][n][0]][tuple[i][n][1]]] != "\x00":
            flag[mat[tuple[i][n1][0]][tuple[i][n1][1]]] = chr(ord(passcode[i]) - ord(flag[mat[tuple[i][n][0]][tuple[i][n][1]]]))

for i in range(7, 9):
    # these tuples have 3 flag characters mapped to them, so we need to check all 3
    for n in range(3):
        n1 = (n + 1) % 3
        n2 = (n + 2) % 3
        if flag[mat[tuple[i][n1][0]][tuple[i][n1][1]]] != "\x00" and flag[mat[tuple[i][n2][0]][tuple[i][n2][1]]] != "\x00":
            flag[mat[tuple[i][n][0]][tuple[i][n][1]]] = chr(ord(passcode[i]) - ord(flag[mat[tuple[i][n1][0]][tuple[i][n1][1]]]) - ord(flag[mat[tuple[i][n2][0]][tuple[i][n2][1]]]))

print "".join(flag)
