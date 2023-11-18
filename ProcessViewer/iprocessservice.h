#ifndef IPROCESSSERVICE_H
#define IPROCESSSERVICE_H

#include <process.h>

class IProcessService
{
public:
    virtual std::map<unsigned int, ProcessInfo> getProcessTreeByCom() const = 0;
};

#endif // IPROCESSSERVICE_H
