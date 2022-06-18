#include "http.h"

// https://gist.github.com/aaaddress1/5831798e3e3c6c738db470eae6168377
std::string http::get_binary_data(const wchar_t url[]) {
    std::vector<char>* binaryData = new std::vector<char>();
    WCHAR sz_hostName[MAX_PATH], sz_reqPath[MAX_PATH]; int port = 0;

    // begin with domain?
    if (!wcsstr(url, xorstr_(L"//"))) {
        if (swscanf(url, xorstr_(L"%[^:]:%d%s"), sz_hostName, &port, sz_reqPath) == 3);
        else if (swscanf(url, xorstr_(L"%[^/]%s"), sz_hostName, sz_reqPath) == 2)
            port = INTERNET_DEFAULT_HTTP_PORT;
    }

    // begin with http:// or https:// ?
    else if (swscanf(wcsstr(url, xorstr_(L"//")) + 2, xorstr_(L"%[^:]:%d%s"), sz_hostName, &port, sz_reqPath) == 3);
    else if (swscanf(wcsstr(url, xorstr_(L"//")) + 2, xorstr_(L"%[^/]%s"), sz_hostName, sz_reqPath) == 2)
        port = wcsstr(url, xorstr_(L"https")) ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
    else return xorstr_("error 1");

    // launch a http request.
    HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;
    hSession = WinHttpOpen(xorstr_(L"WinHTTP/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    hConnect = LI_FN(WinHttpConnect)(hSession, sz_hostName, port, 0);
    hRequest = WinHttpOpenRequest(hConnect, xorstr_(L"GET"), sz_reqPath, NULL, WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE);
    if (!hRequest) return xorstr_("error 2");

    if (!WinHttpSendRequest(hRequest, xorstr_(L"x-api-key: REDACTED\r\n"), -1L, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) or !WinHttpReceiveResponse(hRequest, NULL))
        return xorstr_("error 3");

    // recv binary data.
    char byteCache[4096] = { 0 };
    for (DWORD dwRead(sizeof(byteCache)); dwRead == sizeof(byteCache); ) {
        if (!LI_FN(WinHttpReadData)(hRequest, byteCache, sizeof(byteCache), &dwRead)) return xorstr_("error 4");
        for (size_t x = 0; x < dwRead; x++) binaryData->push_back(byteCache[x]);
    }

    // clean up.
    if (hRequest) LI_FN(WinHttpCloseHandle)(hRequest);
    if (hConnect) LI_FN(WinHttpCloseHandle)(hConnect);
    if (hSession) LI_FN(WinHttpCloseHandle)(hSession);

    std::string binary{ binaryData->begin(), binaryData->end() };
    return binary;
}

// https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpreaddata#examples
std::string http::get_api_data(const wchar_t url[]) {
    std::string response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL bResults = FALSE;
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(xorstr_(L"WinHTTP/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTP server.
    if (hSession)
        hConnect = LI_FN(WinHttpConnect)(hSession, xorstr_(L"example.com"), INTERNET_DEFAULT_HTTPS_PORT, 0);

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, xorstr_(L"GET"), url, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest, xorstr_(L"x-api-key: REDACTED\r\n"), -1L, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);

    // Keep checking for data until there is nothing left.
    if (bResults) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!LI_FN(WinHttpQueryDataAvailable)(hRequest, &dwSize)) {
                printf(xorstr_("Error %u in WinHttpQueryDataAvailable.\n"), LI_FN(GetLastError)());
                break;
            }

            // No more available data.
            if (!dwSize)
                break;

            // Allocate space for the buffer.
            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer) {
                printf(xorstr_("Out of memory\n"));
                break;
            }

            // Read the Data.
            ZeroMemory(pszOutBuffer, dwSize + 1);

            if (!LI_FN(WinHttpReadData)(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                printf(xorstr_("Error %u in WinHttpReadData.\n"), LI_FN(GetLastError)());
            }
            else {
                response.append(pszOutBuffer);
            }

            // Free the memory allocated to the buffer.
            delete[] pszOutBuffer;

            // This condition should never be reached since WinHttpQueryDataAvailable
            // reported that there are bits to read.
            if (!dwDownloaded)
                break;

        } while (dwSize > 0);
    }
    else {
        // Report any errors.
        printf(xorstr_("Error %d has occurred.\n"), LI_FN(GetLastError)());
    }

    // Close any open handles.
    if (hRequest) LI_FN(WinHttpCloseHandle)(hRequest);
    if (hConnect) LI_FN(WinHttpCloseHandle)(hConnect);
    if (hSession) LI_FN(WinHttpCloseHandle)(hSession);

    return response;
}