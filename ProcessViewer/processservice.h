#ifndef PROCESSSERVICE_H
#define PROCESSSERVICE_H

#include "iprocessservice.h"

#include "iwinprocessservice.h"
class ProcessService: public IProcessService
{
    WA::IWinProcessService* m_processService;
public:

    ProcessService(WA::IWinProcessService* processService):m_processService(processService)
    {

    }
    std::map<unsigned int, ProcessInfo> getProcessTreeByCom() const
    {   
        std::map<unsigned int, ProcessInfo> processes = m_processService->getProcessTreeByCom();
        return processes;
    }

    void kill(unsigned int processId)
    {
        m_processService->kill(processId);
    }
};

#endif // PROCESSSERVICE_H
