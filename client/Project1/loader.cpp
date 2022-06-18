#include "loader.h"

// https://github.com/woah1337/NSLoader/blob/master/InjectLib/InjectLib.cpp
// https://cryptopp.com/wiki/Advanced_Encryption_Standard
bool loader::inject() {
	std::string dll_string = xorstr_("https://example.com/api/dll.php?token=");
	dll_string += login_token;
	std::wstring ws_dll_string{ dll_string.begin(), dll_string.end() };
	std::string response = http::get_binary_data(ws_dll_string.c_str());

	if (response == xorstr_("invalid token")) {
		sub_status = response;
		return false;
	}
	
	std::string dll;

	CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption d;
	d.SetKeyWithIV((byte*)xorstr_("REDACTED"), 16, (byte*)xorstr_("REDACTED"));

	CryptoPP::StringSource s(response, true, new CryptoPP::HexDecoder(new CryptoPP::StreamTransformationFilter(d, new CryptoPP::StringSink(dll))));
	
	auto size = sizeof(dll) / sizeof(dll[0]);

	blackbone::Process proc;

	proc.Attach(xorstr_(L"csgo.exe"));

	proc.mmap().MapImage(size, (char*)dll.c_str(), false, blackbone::WipeHeader);

	proc.Detach();

	return true;
}

// https://github.com/KoMaR1911/dnSpyDetector/blob/master/dnSpyDetector/dnSpyDetector/Program.cs
bool loader::is_func_hooked(HMODULE Module, LPCSTR Function) {
	HANDLE FunctionHandle = LI_FN(GetProcAddress)(Module, Function);
	byte Data[5];

	memcpy(Data, FunctionHandle, 5);

	if (((((Data[0] == 0xE9 || Data[0] == 0x68 || Data[0] == 0xB8)
		|| (Data[1] == 0xE9 || Data[1] == 0x68 || Data[1] == 0xB8)
		|| (Data[2] == 0xE9 || Data[2] == 0x68 || Data[2] == 0xB8)
		|| (Data[3] == 0xE9 || Data[3] == 0x68 || Data[3] == 0xB8)
		|| (Data[4] == 0xE9 || Data[4] == 0x68 || Data[4] == 0xB8)))))
	{
		return true;
	}

	return false;
}

// https://github.com/LordNoteworthy/al-khaser/
#define LODWORD_(_qw)    ((DWORD)(_qw))
BOOL loader::vm_check() {
	ULONGLONG tsc1;
	ULONGLONG tsc2;
	ULONGLONG tsc3;
	DWORD i = 0;

	// Try this 10 times in case of small fluctuations
	for (i = 0; i < 10; i++)
	{
		tsc1 = __rdtsc();

		// Waste some cycles - should be faster than CloseHandle on bare metal
		GetProcessHeap();

		tsc2 = __rdtsc();

		// Waste some cycles - slightly longer than GetProcessHeap() on bare metal
		CloseHandle(0);

		tsc3 = __rdtsc();

		// Did it take at least 10 times more CPU cycles to perform CloseHandle than it took to perform GetProcessHeap()?
		if ((LODWORD_(tsc3) - LODWORD_(tsc2)) / (LODWORD_(tsc2) - LODWORD_(tsc1)) >= 10)
			return FALSE;
	}

	// We consistently saw a small ratio of difference between GetProcessHeap and CloseHandle execution times
	// so we're probably in a VM!
	return TRUE;
}

// https://stackoverflow.com/a/51230137
bool loader::check_testsigning() {
	typedef NTSTATUS(__stdcall* td_NtQuerySystemInformation)(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

	struct SYSTEM_CODEINTEGRITY_INFORMATION {
		ULONG Length;
		ULONG CodeIntegrityOptions;
	};

	static td_NtQuerySystemInformation NtQuerySystemInformation = (td_NtQuerySystemInformation)LI_FN(GetProcAddress)(GetModuleHandle(xorstr_(L"ntdll.dll")), xorstr_("NtQuerySystemInformation"));

	SYSTEM_CODEINTEGRITY_INFORMATION Integrity = { sizeof(SYSTEM_CODEINTEGRITY_INFORMATION), 0 };
	NTSTATUS status = NtQuerySystemInformation(103, &Integrity, sizeof(Integrity), NULL);

	return (NT_SUCCESS(status) && (Integrity.CodeIntegrityOptions & 0x2));
}
