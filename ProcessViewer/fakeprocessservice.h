#ifndef FAKEPROCESSSERVICE_H
#define FAKEPROCESSSERVICE_H

#include <chrono>
//#include <format>
#include <string>
#include "processserviceexception.h"
#include "iprocessservice.h"

class FakeProcessService: public IProcessService
{
public:
    std::map<unsigned int, ProcessInfo> getProcessTreeByCom() const
    {
        try
        {
            static int i = 0;
            i++;
            std::wstring s1;// = std::format(L"{}_{:%F %T}", L"Pr", std::chrono::system_clock::now());
            std::map<unsigned int, ProcessInfo> processes;
            //processes.insert({0, ProcessInfo(0, std::format(L"{}_{:%F %T}", L"Pr00_", std::chrono::system_clock::now()), -1, 0)});
            //processes.insert({10, ProcessInfo(10, std::format(L"{}_{:%F %T}", L"Pr10_", std::chrono::system_clock::now()), -1, 0)});
//            processes.insert({20, ProcessInfo(20, std::format(L"{}_{:%F %T}", L"Pr20_", std::chrono::system_clock::now()), -1, 0)});
//            processes.insert({30, ProcessInfo(30, std::format(L"{}_{:%F %T}", L"Pr30_", std::chrono::system_clock::now()), -1, 0)});
//            processes.insert({40, ProcessInfo(40, std::format(L"{}_{:%F %T}", L"Pr40_", std::chrono::system_clock::now()), -1, 0)});

//            processes.insert({1, ProcessInfo(1, std::format(L"{}_{:%F %T}", L"Pr1_", std::chrono::system_clock::now()), 0, 0)});
//            processes.insert({2, ProcessInfo(2, std::format(L"{}_{:%F %T}", L"Pr2_", std::chrono::system_clock::now()), 0, 0)});
//            processes.insert({3, ProcessInfo(3, std::format(L"{}_{:%F %T}", L"Pr3_", std::chrono::system_clock::now()), 0, 0)});

            if (i < 3)
            {
                //processes.insert({11, ProcessInfo(11, std::format(L"{}_{:%F %T}", L"Pr11_", std::chrono::system_clock::now()), 10, 0)});
                //processes.insert({12, ProcessInfo(12, std::format(L"{}_{:%F %T}", L"Pr12_", std::chrono::system_clock::now()), 10, 0)});
            }

            if (i > 15 && i < 20)
            {
                //processes.insert({13, ProcessInfo(13, std::format(L"{}_{:%F %T}", L"Pr13_", std::chrono::system_clock::now()), 10, 0)});
            }
            //throw ProcessServiceException();
            return processes;
        }
        catch(...)
        {
            throw;
        }
    }
};

#endif // FAKEPROCESSSERVICE_H
