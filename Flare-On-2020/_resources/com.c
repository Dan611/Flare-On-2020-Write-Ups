#pragma comment(lib, "ole32")

#include <windows.h>
#include <objbase.h>
#include <combaseapi.h>
#include <stdio.h>

// credHelper.dll server CLSID
wchar_t CREDHELPER_CLSID[] = L"{CEEACC6E-CCB2-4C4F-BCF6-D2176037A9A7}";

// flag decryption COM interface CLSID
wchar_t FLAG_CLSID[] = L"{E27297B0-1E98-4033-B389-24ECA246002A}";

int
main()
{
	HRESULT result;

	CLSID credhelper_id;
	CLSID flag_id;
	void *flag_interface;

	void **com_vtable;
	void (*read_password)(void *, char *);
	void (*set_flag)(void *, char *);

	char flag_data[256];

	CoInitialize(NULL);

	CLSIDFromString(FLAG_CLSID, &flag_id);
	CLSIDFromString(CREDHELPER_CLSID, &credhelper_id);

	if((result = CoCreateInstance(
		&credhelper_id,
		NULL,
		CLSCTX_INPROC_SERVER,
		&flag_id,
		&flag_interface)) != S_OK)
	{
		printf("CoCreateInstance failed with: %x\n", result);
		ExitProcess(-1);
	}

	// calling the vtable directly instead of using c++ classes
	com_vtable = *(void **) flag_interface;
	read_password = com_vtable[3];
	set_flag = com_vtable[4];

	printf("flag_interface: %p\n", flag_interface);
	printf("read_password: %p\n", read_password);
	printf("set_flag: %p\n", set_flag);

	read_password(flag_interface, flag_data);
	set_flag(flag_interface, flag_data);

	CoUninitialize();
}
