#ifndef __COMMON_INTERFACES_H__
#define __COMMON_INTERFACES_H__

#ifndef interface
#define interface struct
#endif

interface IAbstract
{
protected:
	int nRefCount;
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

#endif