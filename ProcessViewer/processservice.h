#ifndef PROCESSSERVICE_H
#define PROCESSSERVICE_H

#include "iprocessservice.h"
#include "winprocessservice.h"
class ProcessService: public IProcessService
{
public:
    std::map<unsigned int, ProcessInfo> getProcessTreeByCom() const
    {
        WA::WinProcessService pr;
        std::map<unsigned int, ProcessInfo> processes = pr.getProcessTreeByCom();
        return processes;
    }
};

#endif // PROCESSSERVICE_H
