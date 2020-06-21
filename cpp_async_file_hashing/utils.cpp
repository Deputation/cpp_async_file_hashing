#include "common.hpp"

std::string utils::get_file_hash_md5(HANDLE handle)
{
	std::string hash = "";

	if (INVALID_HANDLE_VALUE == handle)
	{
		return "";
	}

	HCRYPTPROV handle_prov = 0;

	// Get handle to the crypto provider
	if (!CryptAcquireContext(&handle_prov,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		return "";
	}

	HCRYPTHASH handle_hash = 0;

	if (!CryptCreateHash(handle_prov, CALG_MD5, 0, 0, &handle_hash))
	{
		return "";
	}

	BYTE byte_hash[1024];
	DWORD cb_read = 0;

	auto read_result = false;

	while (read_result = ReadFile(handle, byte_hash, 1024, &cb_read, NULL))
	{
		if (0 == cb_read)
		{
			break;
		}

		if (!CryptHashData(handle_hash, byte_hash, cb_read, 0))
		{
			CryptReleaseContext(handle_prov, 0);
			CryptDestroyHash(handle_hash);
			return "";
		}
	}

	if (!read_result)
	{
		CryptReleaseContext(handle_prov, 0);
		CryptDestroyHash(handle_hash);
		return "";
	}

	BYTE rgb_hash[16];
	DWORD cb_hash = 0;
	CHAR hash_digits[] = "0123456789abcdef";

	cb_hash = 16;
	if (CryptGetHashParam(handle_hash, HP_HASHVAL, rgb_hash, &cb_hash, 0))
	{
		for (DWORD i = 0; i < cb_hash; i++)
		{
			hash.push_back(hash_digits[rgb_hash[i] >> 4]);
			hash.push_back(hash_digits[rgb_hash[i] & 0xf]);
		}
		hash.push_back('\x00');
	}
	else
	{
		CryptDestroyHash(handle_hash);
		CryptReleaseContext(handle_prov, 0);
		return "";
	}

	CryptDestroyHash(handle_hash);
	CryptReleaseContext(handle_prov, 0);

	return hash;
}

void utils::file_enumeration_callback(const std::function<void(HANDLE, std::string)> function, const std::string iteration_path)
{
	const std::string path = iteration_path;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{

		const auto wfull_path_buffer = std::wstring(entry.path());
		const auto full_path = std::string(wfull_path_buffer.begin(), wfull_path_buffer.end());
		const HANDLE handle = CreateFile(entry.path().c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		// the name can be derived from the handle but the function is
		// unnecessarily huge, let's process the string instead.
		const auto name_buffer = full_path;

		// function running in unvirtualized context
		function(handle, name_buffer);
	}
}