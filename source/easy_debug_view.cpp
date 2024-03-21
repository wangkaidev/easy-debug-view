/*
MIT License

Copyright (c) 2024 Kai Wang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdexcept>
#include <iostream>

constexpr auto DBWIN_BUFFER_NAME = TEXT("DBWIN_BUFFER");
constexpr auto DBWIN_BUFFER_READY_NAME = TEXT("DBWIN_BUFFER_READY");
constexpr auto DBWIN_DATA_READY_NAME = TEXT("DBWIN_DATA_READY");
constexpr auto DBWIN_MUTEX_NAME = TEXT("DBWinMutex");

/// Exception to make win32 error handling easier.
class Win32Exception : public std::runtime_error
{
public:
    Win32Exception() = delete;

    Win32Exception(DWORD last_error, const std::string& msg)
        : std::runtime_error(msg), last_error_(last_error)
    {
    }

    auto GetLastError() const { return last_error_; }

    static Win32Exception FromLastWin32Error(const std::string& msg)
    {
        return Win32Exception(::GetLastError(), msg);
    }

private:
    DWORD last_error_;
};

/// Wrapper for exception handling.
inline void ThrowIfFalse(bool condition, const std::string& msg)
{
    if (!condition)
    {
        throw Win32Exception::FromLastWin32Error(msg);
    }
}

/// Wrapper for windows handle.
class WinHandle
{
public:
    WinHandle(HANDLE h = nullptr)
        : handle_(h)
    {
    }

    ~WinHandle()
    {
        Release();
    }

    void Release()
    {
        if (*this)
        {
            ::CloseHandle(handle_);
        }

        handle_ = nullptr;
    }

    operator bool() const
    {
        return handle_ != INVALID_HANDLE_VALUE && handle_ != nullptr;
    }

    operator HANDLE() const
    {
        return handle_;
    }

    WinHandle& operator=(const HANDLE& handle)
    {
        Release();
        handle_ = handle;
        return *this;
    }

    WinHandle& operator=(WinHandle& handle)
    {
        if (this != &handle)
        {
            HANDLE temp = handle_;
            handle_ = handle.handle_;
            handle.handle_ = temp;
            handle.Release();
        }

        return *this;
    }

private:
    HANDLE handle_;
};

/// DBWin buffer data struct.
#pragma pack(push, 1)
struct DBWinBufferData
{
    DWORD proc_id;
    BYTE data[4096 - sizeof(DWORD)];
};
#pragma pack(pop)

bool g_running_ = true;

BOOL console_ctrl_handler(DWORD ctrl)
{
    switch (ctrl)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        g_running_ = false;
        return TRUE;
    }
    return FALSE;
}

void listen_sys_debug_output()
{
    ThrowIfFalse(SetConsoleCtrlHandler((PHANDLER_ROUTINE)(console_ctrl_handler), TRUE) == TRUE, "Failed to set console control handler.");

    WinHandle mutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, DBWIN_MUTEX_NAME);
    if (!mutex)
    {
        ThrowIfFalse(GetLastError() == ERROR_FILE_NOT_FOUND, "Failed to open mutex due to file not found error. Please try to use Admin to run the program.");
        ThrowIfFalse(mutex = ::CreateMutex(nullptr, FALSE, DBWIN_MUTEX_NAME), "Failed to open mutex.");
    }

    WinHandle buffer_ready_event = ::OpenEvent(EVENT_MODIFY_STATE, FALSE, DBWIN_BUFFER_READY_NAME);
    if (!buffer_ready_event)
    {
        ThrowIfFalse(GetLastError() == ERROR_FILE_NOT_FOUND, "Failed to open event of buffer ready due to file not found error.");
        ThrowIfFalse(buffer_ready_event = ::CreateEvent(nullptr, FALSE, TRUE, DBWIN_BUFFER_READY_NAME), "Failed to create create event for buffer ready.");
    }

    WinHandle data_ready_event = ::OpenEvent(EVENT_MODIFY_STATE, FALSE, DBWIN_DATA_READY_NAME);
    if (!data_ready_event)
    {
        ThrowIfFalse(GetLastError() == ERROR_FILE_NOT_FOUND, "Failed to open data ready event due to file not found error.");
        ThrowIfFalse(data_ready_event = ::CreateEvent(nullptr, FALSE, FALSE, DBWIN_DATA_READY_NAME), "Failed to create data ready event.");
    }

    WinHandle buffer_file_mapping = ::OpenFileMapping(FILE_MAP_READ, FALSE, DBWIN_BUFFER_NAME);
    if (!buffer_file_mapping)
    {
        ThrowIfFalse(GetLastError() == ERROR_FILE_NOT_FOUND, "Failed to open file mapping due to file not found error.");
        ThrowIfFalse(buffer_file_mapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(DBWinBufferData), DBWIN_BUFFER_NAME),
            "Failed to create file mapping.");
    }

    DBWinBufferData* dbwin_buffer = (DBWinBufferData*)(::MapViewOfFile(
        buffer_file_mapping, SECTION_MAP_READ, 0, 0, 0));
    ThrowIfFalse(dbwin_buffer, "Failed to do file mapping for the data buffer.");

    while (g_running_)
    {
        constexpr DWORD WAIT_TIME_MS = 100;

        if (::WaitForSingleObject(data_ready_event, WAIT_TIME_MS) == WAIT_OBJECT_0)
        {
            std::cout << "[PID " << dbwin_buffer->proc_id << "] " << dbwin_buffer->data;
            ::SetEvent(buffer_ready_event);
        }
    }

    ThrowIfFalse(::UnmapViewOfFile(dbwin_buffer), "Failed to unmap file mapping for the buffer.");
}

int main()
{
    try
    {
        listen_sys_debug_output();
    }
    catch (const Win32Exception& e)
    {
        std::cerr << "Win32 Exception happened: " << e.what()
                  << "\nLast Error Code: " << e.GetLastError() << std::endl;

        return e.GetLastError();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception happened: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception happened." << std::endl;
        return 1;
    }

    return 0;
}
