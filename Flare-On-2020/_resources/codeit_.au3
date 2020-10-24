#Region
	#AutoIt3Wrapper_UseUpx=y
#EndRegion

#OnAutoItStartRegister "AREIHNVAPWN"
Global $os
Global $global_input

Func _stringrepeat($sstring, $irepeatcount)
	$irepeatcount = Int($irepeatcount)
	If $irepeatcount = 0 Then Return ""
	If StringLen($sstring) < 1 OR $irepeatcount < 0 Then Return SetError(1, 0, "")
	Local $sresult = ""
	While $irepeatcount > 1
		If BitAND($irepeatcount, 1) Then $sresult &= $sstring
		$sstring &= $sstring
		$irepeatcount = BitShift($irepeatcount, 1)
	WEnd
	Return $sstring & $sresult
EndFunc

Func create_bmp($width, $height, $unused)
	Local $bmp_header[2]
	$bmp_header[0] = DllStructCreate("struct;uint bfSize;uint bfReserved;uint bfOffBits;uint biSize;int biWidth;int biHeight;ushort biPlanes;ushort biBitCount;uint biCompression;uint biSizeImage;int biXPelsPerMeter;int biYPelsPerMeter;uint biClrUsed;uint biClrImportant;endstruct;")
	DllStructSetData($bmp_header[0], "bfSize", (3 * $width + Mod($width, 4) * Abs($height)))
	DllStructSetData($bmp_header[0], "bfReserved", 0)
	DllStructSetData($bmp_header[0], "bfOffBits", 54)
	DllStructSetData($bmp_header[0], "biSize", 40)
	DllStructSetData($bmp_header[0], "biWidth", $width)
	DllStructSetData($bmp_header[0], "biHeight", $height)
	DllStructSetData($bmp_header[0], "biPlanes", 1)
	DllStructSetData($bmp_header[0], "biBitCount", 24)
	DllStructSetData($bmp_header[0], "biCompression", 0)
	DllStructSetData($bmp_header[0], "biSizeImage", 0)
	DllStructSetData($bmp_header[0], "biXPelsPerMeter", 0)
	DllStructSetData($bmp_header[0], "biYPelsPerMeter", 0)
	DllStructSetData($bmp_header[0], "biClrUsed", 0)
	DllStructSetData($bmp_header[0], "biClrImportant", 0)
	$bmp_header[1] = DllStructCreate("struct;" & _stringrepeat("byte[" & DllStructGetData($bmp_header[0], "biWidth") * 3 & "];", DllStructGetData($bmp_header[0], "biHeight")) & "endstruct")
	Return $bmp_header
EndFunc

Func rand_str($low, $high)
	Local $str = ""
	For $str_len = 0 To Random($low, $high, 1)
		$str &= Chr(Random(97, 122, 1))
	Next
	Return $str
EndFunc

Func export_internal_resource($rsrc_num)
	Local $name = rand_str(15, 20)
	Switch $rsrc_num
		Case 10 To 15
			$name &= ".bmp"
			FileInstall(".\sprite.bmp", @ScriptDir & "\\" & $name)
		Case 25 To 30
			$name &= ".dll"
			FileInstall(".\qr_encoder.dll", @ScriptDir & "\\" & $name)
	EndSwitch
	Return $name
EndFunc

Func get_comptuer_name()
	Local $binary_chars = -1
	Local $string = DllStructCreate("struct;dword;char[1024];endstruct")
	DllStructSetData($string, 1, 1024)
	Local $success = DllCall("kernel32.dll", "int", "GetComputerNameA", "ptr", DllStructGetPtr($string, 2), "ptr", DllStructGetPtr($string, 1))
	If $success[0] <> 0 Then
		$binary_chars = BinaryMid(DllStructGetData($string, 2), 1, DllStructGetData($string, 1))
	EndIf
	Return $binary_chars
EndFunc

GUICreate("CodeIt Plus!", 300, 375, -1, -1)

; obfuscate input bytes using bytes from the internal bmp file
; 7 bytes are read from the bmp per input byte
Func obfuscate_string(ByRef $bytes)
	Local $internal_bmp = export_internal_resource(14)
	Local $bmp_handle = open_file_read($internal_bmp)
	If $bmp_handle <> -1 Then
		Local $bmp_size = get_file_size($bmp_handle)
		If $bmp_size <> -1 AND DllStructGetSize($bytes) < $bmp_size - 54 Then
			Local $bmp_buffer = DllStructCreate("struct;byte[" & $bmp_size & "];endstruct")
			Local $success = read_file($bmp_handle, $bmp_buffer)
			If $success <> -1 Then
				Local $bmp_struct = DllStructCreate("struct;byte[54];byte[" & $bmp_size - 54 & "];endstruct", DllStructGetPtr($bmp_buffer))
				Local $bmp_data_index = 1
				Local $encrypted_string = ""
				For $byte_index = 1 To DllStructGetSize($bytes)
					Local $byte = Number(DllStructGetData($bytes, 1, $byte_index))
					For $shift_amount = 6 To 0 Step -1
						$byte += BitShift(BitAND(Number(DllStructGetData($bmp_struct, 2, $bmp_data_index)), 1), -1 * $shift_amount)
						$bmp_data_index += 1
					Next
					ConsoleWrite(BitShift($byte, 1) + BitShift(BitAND($byte, 1), -7) & ", ")
					$encrypted_string &= Chr(BitShift($byte, 1) + BitShift(BitAND($byte, 1), -7))
				Next
				DllStructSetData($bytes, 1, $encrypted_string)
				MsgBox(0, "title", BinaryToString($encrypted_string), 0)
			EndIf
		EndIf
		close_file($bmp_handle)
	EndIf
	delete($internal_bmp)
EndFunc

Func test_for_win_flag(ByRef $string_struct)

	Local $computer_name = $global_input
	; Local $computer_name = get_comptuer_name()

	If $computer_name <> -1 Then
		$computer_name = Binary(StringLower(BinaryToString($computer_name)))
		Local $computer_name_struct = DllStructCreate("struct;byte[" & BinaryLen($computer_name) & "];endstruct")
		DllStructSetData($computer_name_struct, 1, $computer_name)

		obfuscate_string($computer_name_struct)
		Local $crypto_service = DllStructCreate("struct;ptr;ptr;dword;byte[32];endstruct")
		DllStructSetData($crypto_service, 3, 32)
		Local $success = DllCall("advapi32.dll", "int", "CryptAcquireContextA", "ptr", DllStructGetPtr($crypto_service, 1), "ptr", 0, "ptr", 0, "dword", 24, "dword", 4026531840)

		; get the hash of the encrypted computer_name_struct
		If $success[0] <> 0 Then
			$success = DllCall("advapi32.dll", "int", "CryptCreateHash", "ptr", DllStructGetData($crypto_service, 1), "dword", 32780, "dword", 0, "dword", 0, "ptr", DllStructGetPtr($crypto_service, 2))
			If $success[0] <> 0 Then
				$success = DllCall("advapi32.dll", "int", "CryptHashData", "ptr", DllStructGetData($crypto_service, 2), "struct*", $computer_name_struct, "dword", DllStructGetSize($computer_name_struct), "dword", 0)
				If $success[0] <> 0 Then
					$success = DllCall("advapi32.dll", "int", "CryptGetHashParam", "ptr", DllStructGetData($crypto_service, 2), "dword", 2, "ptr", DllStructGetPtr($crypto_service, 4), "ptr", DllStructGetPtr($crypto_service, 3), "dword", 0)
					If $success[0] <> 0 Then
						; use the hash of the obfuscated computer_name_struct as the decryption key
						Local $decryption_key = Binary("0x080200001066000020000000") & DllStructGetData($crypto_service, 4)
						Local $encrypted_data = Binary("0xCD4B32C650CF21BDA184D8913E6F920A37A4F3963736C042C459EA07B79EA443FFD1898BAE49B115F6CB1E2A7C1AB3C4C25612A519035F18FB3B17528B3AECAF3D480E98BF8A635DAF974E0013535D231E4B75B2C38B804C7AE4D266A37B36F2C555BF3A9EA6A58BC8F906CC665EAE2CE60F2CDE38FD30269CC4CE5BB090472FF9BD26F9119B8C484FE69EB934F43FEEDEDCEBA791460819FB21F10F832B2A5D4D772DB12C3BED947F6F706AE4411A52")
						Local $crypt_struct = DllStructCreate("struct;ptr;ptr;dword;byte[8192];byte[" & BinaryLen($decryption_key) & "];dword;endstruct")
						DllStructSetData($crypt_struct, 3, BinaryLen($encrypted_data))
						DllStructSetData($crypt_struct, 4, $encrypted_data)
						DllStructSetData($crypt_struct, 5, $decryption_key)
						DllStructSetData($crypt_struct, 6, BinaryLen($decryption_key))
						Local $success = DllCall("advapi32.dll", "int", "CryptAcquireContextA", "ptr", DllStructGetPtr($crypt_struct, 1), "ptr", 0, "ptr", 0, "dword", 24, "dword", 4026531840)
						If $success[0] <> 0 Then
							$success = DllCall("advapi32.dll", "int", "CryptImportKey", "ptr", DllStructGetData($crypt_struct, 1), "ptr", DllStructGetPtr($crypt_struct, 5), "dword", DllStructGetData($crypt_struct, 6), "dword", 0, "dword", 0, "ptr", DllStructGetPtr($crypt_struct, 2))
							If $success[0] <> 0 Then
								$success = DllCall("advapi32.dll", "int", "CryptDecrypt", "ptr", DllStructGetData($crypt_struct, 2), "dword", 0, "dword", 1, "dword", 0, "ptr", DllStructGetPtr($crypt_struct, 4), "ptr", DllStructGetPtr($crypt_struct, 3))
								If $success[0] <> 0 Then
									Local $unencrypted_data = BinaryMid(DllStructGetData($crypt_struct, 4), 1, DllStructGetData($crypt_struct, 3))
									$FLARE = Binary("FLARE")
									$ERALF = Binary("ERALF")
									$unencrypted_header = BinaryMid($unencrypted_data, 1, BinaryLen($FLARE))
									$unencrypted_footer = BinaryMid($unencrypted_data, BinaryLen($unencrypted_data) - BinaryLen($ERALF) + 1, BinaryLen($ERALF))
									If $FLARE = $unencrypted_header AND $ERALF = $unencrypted_footer Then

										MsgBox(0, "", "SUCCESS: " & $global_input, 0)

										DllStructSetData($string_struct, 1, BinaryMid($unencrypted_data, 6, 4))
										DllStructSetData($string_struct, 2, BinaryMid($unencrypted_data, 10, 4))
										DllStructSetData($string_struct, 3, BinaryMid($unencrypted_data, 14, BinaryLen($unencrypted_data) - 18))
									EndIf
								EndIf
								DllCall("advapi32.dll", "int", "CryptDestroyKey", "ptr", DllStructGetData($crypt_struct, 2))
							EndIf
							DllCall("advapi32.dll", "int", "CryptReleaseContext", "ptr", DllStructGetData($crypt_struct, 1), "dword", 0)
						EndIf
					EndIf
				EndIf
				DllCall("advapi32.dll", "int", "CryptDestroyHash", "ptr", DllStructGetData($crypto_service, 2))
			EndIf
			DllCall("advapi32.dll", "int", "CryptReleaseContext", "ptr", DllStructGetData($crypto_service, 1), "dword", 0)
		EndIf
	EndIf
EndFunc

Func areaqwbmtiz(ByRef $flkhfbuyon)
	Local $fluupfrkdz = -1
	Local $flqbsfzezk = DllStructCreate("struct;ptr;ptr;dword;byte[16];endstruct")
	DllStructSetData($flqbsfzezk, 3, 16)
	Local $fltrtsuryd = DllCall("advapi32.dll", "int", "CryptAcquireContextA", "ptr", DllStructGetPtr($flqbsfzezk, 1), "ptr", 0, "ptr", 0, "dword", 24, "dword", 4026531840)
	If $fltrtsuryd[0] <> 0 Then
		$fltrtsuryd = DllCall("advapi32.dll", "int", "CryptCreateHash", "ptr", DllStructGetData($flqbsfzezk, 1), "dword", 32771, "dword", 0, "dword", 0, "ptr", DllStructGetPtr($flqbsfzezk, 2))
		If $fltrtsuryd[0] <> 0 Then
			$fltrtsuryd = DllCall("advapi32.dll", "int", "CryptHashData", "ptr", DllStructGetData($flqbsfzezk, 2), "struct*", $flkhfbuyon, "dword", DllStructGetSize($flkhfbuyon), "dword", 0)
			If $fltrtsuryd[0] <> 0 Then
				$fltrtsuryd = DllCall("advapi32.dll", "int", "CryptGetHashParam", "ptr", DllStructGetData($flqbsfzezk, 2), "dword", 2, "ptr", DllStructGetPtr($flqbsfzezk, 4), "ptr", DllStructGetPtr($flqbsfzezk, 3), "dword", 0)
				If $fltrtsuryd[0] <> 0 Then
					$fluupfrkdz = DllStructGetData($flqbsfzezk, 4)
				EndIf
			EndIf
			DllCall("advapi32.dll", "int", "CryptDestroyHash", "ptr", DllStructGetData($flqbsfzezk, 2))
		EndIf
		DllCall("advapi32.dll", "int", "CryptReleaseContext", "ptr", DllStructGetData($flqbsfzezk, 1), "dword", 0)
	EndIf
	Return $fluupfrkdz
EndFunc

Func arepfnkwypw()
	Local $flgqbtjbmi = -1
	Local $fltpvjccvq = DllStructCreate("struct;dword;dword;dword;dword;dword;byte[128];endstruct")
	DllStructSetData($fltpvjccvq, 1, DllStructGetSize($fltpvjccvq))
	Local $flaghdvgyv = DllCall("kernel32.dll", "int", "GetVersionExA", "struct*", $fltpvjccvq)
	If $flaghdvgyv[0] <> 0 Then
		If DllStructGetData($fltpvjccvq, 2) = 6 Then
			If DllStructGetData($fltpvjccvq, 3) = 1 Then
				$flgqbtjbmi = 0
			EndIf
		EndIf
	EndIf
	Return $flgqbtjbmi
EndFunc

Func main()
	Local $input = GUICtrlCreateInput("Enter text to encode", -1, 5, 300)
	Local $generate_button = GUICtrlCreateButton("Can haz code?", -1, 30, 300)
	Local $displayed_bmp = GUICtrlCreatePic("", -1, 55, 300, 300)
	Local $help = GUICtrlCreateMenu("Help")
	Local $help_item = GUICtrlCreateMenuItem("About CodeIt Plus!", $help)
	Local $internal_bmp = export_internal_resource(13)
	GUICtrlSetImage($displayed_bmp, $internal_bmp)
	delete($internal_bmp)
	GUISetState(@SW_SHOW)
	While 1
		Switch GUIGetMsg()
			Case $generate_button
				Local $input_text = GUICtrlRead($input)
				$global_input = $input_text
				If $input_text Then
					Local $qr_dll = export_internal_resource(26)
					Local $string_struct = DllStructCreate("struct;dword;dword;byte[3918];endstruct")
					Local $qr_symbol = DllCall($qr_dll, "int:cdecl", "justGenerateQRSymbol", "struct*", $string_struct, "str", $input_text)
					If $qr_symbol[0] <> 0 Then
						test_for_win_flag($string_struct)
						Local $qr_bmp = create_bmp((DllStructGetData($string_struct, 1) * DllStructGetData($string_struct, 2)), (DllStructGetData($string_struct, 1) * DllStructGetData($string_struct, 2)), 1024)
						$qr_pixels = DllCall($qr_dll, "int:cdecl", "justConvertQRSymbolToBitmapPixels", "struct*", $string_struct, "struct*", $qr_bmp[1])
						If $qr_pixels[0] <> 0 Then
							$internal_bmp = rand_str(25, 30) & ".bmp"
							arelassehha($qr_bmp, $internal_bmp)
						EndIf
					EndIf
					delete($qr_dll)
				Else
					$internal_bmp = export_internal_resource(11)
				EndIf
				GUICtrlSetImage($displayed_bmp, $internal_bmp)
				delete($internal_bmp)
			Case $help_item
				Local $help_text = "This program generates QR codes using QR Code Generator (https://www.nayuki.io/page/qr-code-generator-library) developed by Nayuki. "
				$help_text &= "QR Code Generator is available on GitHub (https://github.com/nayuki/QR-Code-generator) and open-sourced under the following permissive MIT License (https://github.com/nayuki/QR-Code-generator#license):"
				$help_text &= @CRLF
				$help_text &= @CRLF
				$help_text &= "Copyright Â© 2020 Project Nayuki. (MIT License)"
				$help_text &= @CRLF
				$help_text &= "https://www.nayuki.io/page/qr-code-generator-library"
				$help_text &= @CRLF
				$help_text &= @CRLF
				$help_text &= "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the Software), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:"
				$help_text &= @CRLF
				$help_text &= @CRLF
				$help_text &= "1. The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software."
				$help_text &= @CRLF
				$help_text &= "2. The Software is provided as is, without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the Software or the use or other dealings in the Software."
				MsgBox(4096, "About CodeIt Plus!", $help_text)
			Case -3
				ExitLoop
		EndSwitch
	WEnd
EndFunc

Func arepqqkaeto($flmwacufre, $fljxaivjld)
	Local $fljiyeluhx = -1
	Local $flmwacufreheadermagic = DllStructCreate("struct;ushort;endstruct")
	DllStructSetData($flmwacufreheadermagic, 1, 19778)
	Local $flivpiogmf = open_file_write($fljxaivjld, False)
	If $flivpiogmf <> -1 Then
		Local $flchlkbend = aremfkxlayv($flivpiogmf, DllStructGetPtr($flmwacufreheadermagic), DllStructGetSize($flmwacufreheadermagic))
		If $flchlkbend <> -1 Then
			$flchlkbend = aremfkxlayv($flivpiogmf, DllStructGetPtr($flmwacufre[0]), DllStructGetSize($flmwacufre[0]))
			If $flchlkbend <> -1 Then
				$fljiyeluhx = 0
			EndIf
		EndIf
		close_file($flivpiogmf)
	EndIf
	Return $fljiyeluhx
EndFunc

main()

Func arelassehha($flbaqvujsl, $flkelsuuiy)
	Local $flefoubdxt = -1
	Local $flamtlcncx = arepqqkaeto($flbaqvujsl, $flkelsuuiy)
	If $flamtlcncx <> -1 Then
		Local $flvikmhxwu = open_file_write($flkelsuuiy, True)
		If $flvikmhxwu <> -1 Then
			Local $flwldjlwrq = Abs(DllStructGetData($flbaqvujsl[0], "biHeight"))
			Local $flumnoetuu = DllStructGetData($flbaqvujsl[0], "biHeight") > 0 ? $flwldjlwrq - 1 : 0
			Local $flqphcjgtp = DllStructCreate("struct;byte;byte;byte;endstruct")
			For $fllrcvawmx = 0 To $flwldjlwrq - 1
				$flamtlcncx = aremfkxlayv($flvikmhxwu, DllStructGetPtr($flbaqvujsl[1], Abs($flumnoetuu - $fllrcvawmx) + 1), DllStructGetData($flbaqvujsl[0], "biWidth") * 3)
				If $flamtlcncx = -1 Then ExitLoop
				$flamtlcncx = aremfkxlayv($flvikmhxwu, DllStructGetPtr($flqphcjgtp), Mod(DllStructGetData($flbaqvujsl[0], "biWidth"), 4))
				If $flamtlcncx = -1 Then ExitLoop
			Next
			If $flamtlcncx <> -1 Then
				$flefoubdxt = 0
			EndIf
			close_file($flvikmhxwu)
		EndIf
	EndIf
	Return $flefoubdxt
EndFunc

Func open_file_read($filename)
	; 2147483648 = GENERIC_READ
	Local $file = DllCall("kernel32.dll", "ptr", "CreateFile", "str", @ScriptDir & "\\" & $filename, "uint", 2147483648, "uint", 0, "ptr", 0, "uint", 3, "uint", 128, "ptr", 0)
	Return $file[0]
EndFunc

Func open_file_write($filename, $open_existing = True)
	; 1073741824 = GENERIC_WRITE
	Local $file = DllCall("kernel32.dll", "ptr", "CreateFile", "str", @ScriptDir & "\\" & $filename, "uint", 1073741824, "uint", 0, "ptr", 0, "uint", $open_existing ? 3 : 2, "uint", 128, "ptr", 0)
	Return $file[0]
EndFunc

GUIDelete()

Func aremfkxlayv($fllsczdyhr, $flbfzgxbcy, $flutgabjfj)
	If $fllsczdyhr <> -1 Then
		Local $flvfnkosuf = DllCall("kernel32.dll", "uint", "SetFilePointer", "ptr", $fllsczdyhr, "long", 0, "ptr", 0, "uint", 2)
		If $flvfnkosuf[0] <> -1 Then
			Local $flwzfbbkto = DllStructCreate("uint")
			$flvfnkosuf = DllCall("kernel32.dll", "ptr", "WriteFile", "ptr", $fllsczdyhr, "ptr", $flbfzgxbcy, "uint", $flutgabjfj, "ptr", DllStructGetPtr($flwzfbbkto), "ptr", 0)
			If $flvfnkosuf[0] <> 0 AND DllStructGetData($flwzfbbkto, 1) = $flutgabjfj Then
				Return 0
			EndIf
		EndIf
	EndIf
	Return -1
EndFunc

Func read_file($file, ByRef $buffer)
	Local $num_bytes_read = DllStructCreate("struct;dword;endstruct")
	Local $success = DllCall("kernel32.dll", "int", "ReadFile", "ptr", $file, "struct*", $buffer, "dword", DllStructGetSize($buffer), "struct*", $num_bytes_read, "ptr", 0)
	Return $success[0]
EndFunc

Func close_file($fldiapcptm)
	Local $flhvhgvtxm = DllCall("kernel32.dll", "int", "CloseHandle", "ptr", $fldiapcptm)
	Return $flhvhgvtxm[0]
EndFunc

Func delete($file)
	Local $retval = DllCall("kernel32.dll", "int", "DeleteFileA", "str", $file)
	Return $retval[0]
EndFunc

Func get_file_size($file)
	Local $file_size = -1
	Local $file_size_high = DllStructCreate("struct;dword;endstruct")
	Local $file_size_low = DllCall("kernel32.dll", "dword", "GetFileSize", "ptr", $file, "struct*", $file_size_high)
	If $file_size_low <> -1 Then
		$file_size = $file_size_low[0] + Number(DllStructGetData($file_size_high, 1))
	EndIf
	Return $file_size
EndFunc
