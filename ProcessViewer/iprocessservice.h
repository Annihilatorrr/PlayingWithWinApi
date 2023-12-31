#ifndef IPROCESSSERVICE_H
#define IPROCESSSERVICE_H

#include <map>
#include <processinfo.h>

class IProcessService
{
public:
    virtual std::map<unsigned int, ProcessInfo> getProcessTreeByCom() const = 0;
    virtual void kill(unsigned int processId) = 0;
};

#endif // IPROCESSSERVICE_H
