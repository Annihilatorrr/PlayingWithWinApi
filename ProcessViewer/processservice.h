#ifndef PROCESSSERVICE_H
#define PROCESSSERVICE_H

#include "iprocessservice.h"

class ProcessService: public IProcessService
{
public:
    std::map<unsigned int, ProcessInfo> getProcessTreeByCom() const
    {
        WA::Process pr;
        std::map<unsigned int, ProcessInfo> processes = pr.getProcessTreeByCom();
        return processes;
    }
};

#endif // PROCESSSERVICE_H
