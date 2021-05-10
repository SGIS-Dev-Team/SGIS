#ifndef SOBJECTFACTORY_H
#define SOBJECTFACTORY_H

#include "modules/paint/sobject.h"

class SObjectFactory : public QObject
{
    Q_OBJECT
private:
    SObjectFactory();

public:
    static void releaseObject(SObject *&pObj)
    {
        if(pObj)
            delete pObj;
        pObj = nullptr;
    }
};



#endif // SOBJECTFACTORY_H
