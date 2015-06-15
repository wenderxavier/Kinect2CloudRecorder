#include <WinBase.h>
#include <cstdlib>

inline void hresultToChar(HRESULT result, char *buffer, unsigned int bufferSize)
{
	HLOCAL pBuffer;
	FormatMessageW(  
	FORMAT_MESSAGE_ALLOCATE_BUFFER | // The function will allocate space for pBuffer.
	FORMAT_MESSAGE_FROM_SYSTEM | // System wide message.
	FORMAT_MESSAGE_IGNORE_INSERTS, // No inserts.
	NULL, // Message is not in a module.
	result, // Message identifier.
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language.
	(LPTSTR)&pBuffer, // Buffer to hold the text string.
	512, // The function will allocate at least this much for pBuffer.
	NULL // No inserts.
	);

	wcstombs(buffer,(const wchar_t*)pBuffer,bufferSize); // converts from wchar to char
	LocalFree(pBuffer);
}

template <typename Callable>
inline void logHRESULT(Callable loggerCallback, const char *filename, int line, HRESULT result)
{
	const unsigned int BufferSize = 512;

	char message[BufferSize];
	hresultToChar(result, message, BufferSize);

	loggerCallback(filename, line, message);
}
