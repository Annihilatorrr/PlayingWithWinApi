#ifndef PROCESSSERVICEEXCEPTION_H
#define PROCESSSERVICEEXCEPTION_H

#include <QException>

class ProcessServiceException : public QException
{
public:
    void raise() const override { throw *this; }
    ProcessServiceException *clone() const override { return new ProcessServiceException(*this); }
};
#endif // PROCESSSERVICEEXCEPTION_H
