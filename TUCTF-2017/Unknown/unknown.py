# the program must be passed an arbitrary argument for this script to work

# length of string to bruteforce
stringLen = 0x38
string = ""
# range of characters to bruteforce
charStart = 0x20
charEnd = 0x7E
# current character bruteforce attempt
char = charStart
# current string  index
index = 0
# whether or not the character brutforce attempt was correct
fail = 1

# run into the hashing function loop
idc.RunTo(0x401C15)
GetDebuggerEvent(WFNE_SUSP, -1)
idc.SetRegValue(1, 'ZF')
idc.RunTo(0x401C49)
GetDebuggerEvent(WFNE_SUSP, -1)
idc.SetRegValue(1, 'ZF')
idc.RunTo(0x401C71)
GetDebuggerEvent(WFNE_SUSP, -1)

# [rbp+var_8] contains the pointer to the input string, which the program tests against
stringPtr = idc.Qword(idc.GetRegValue('RBP') - 0x8)

while index <= stringLen:
    # [rpb+var_C] is used to determine which character the program hashes
    idc.PatchByte(idc.GetRegValue('RBP') - 0xC, index)
    while fail and char <= charEnd:
        # move to the start of the hashing function call
        idc.SetRegValue(0x401C71, 'EIP')
        # patch the input string for the next bruteforce attempt
        PatchByte(stringPtr + index, char)

        # run the hashing function
        idc.RunTo(0x401C82)
        GetDebuggerEvent(WFNE_SUSP, -1)

        # check whether or not the bruteforce worked
        fail = idc.GetRegValue('EAX')

        if fail:
            char += 1
        else:
            string += chr(char)

    char = charStart
    index += 1
    fail = 1

print string