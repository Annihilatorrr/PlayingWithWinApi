#pragma once

#include <string>

namespace WA
{
    class ErrorOutput
    {
    public:
        static std::wstring getLastError();
    };
}
