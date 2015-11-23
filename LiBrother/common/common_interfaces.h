#ifndef __COMMON_INTERFACES_H__
#define __COMMON_INTERFACES_H__

#include <cstring>
#include <vector>

#ifndef interface
#define interface struct
#endif

const int ErrorDescriptionSize = 200;

//错误类型
enum ErrType
{
	None,
	InvalidParam, PermissionDenied,
	UnsupportedMethod, NetworkError, DatabaseError,
	Other
};

//描述一个错误
struct TErrInfo
{
	ErrType type;		//类型
	int code;			//错误代码
	char description[ErrorDescriptionSize];	//错误的描述[不使用时置为nullptr]
	TErrInfo(ErrType type = None, int code = 0, const char * description = nullptr) : type(type), code(code)
	{
		if (!description)
		{
			this->description[0] = '\0';
			return;
		}
#if _MSC_VER >= 1500
		strncpy_s(this->description, description, ErrorDescriptionSize - 1);
#else
		strncpy(this->description, description, ErrorDescriptionSize - 1);
#endif
	}
};

/*	用于实现一个接口，在定义class时加入
	示例：
	class CUser : public IUser
	{
		IMPLEMENT_INTERFACE
	public:
		virtual bool deleteBook();
		//接口的相关实现
	protected:
		//成员变量
		//其他成员函数
	};
*/
#define IMPLEMENT_INTERFACE \
	private:\
		int m_nRefCount = 0;\
		TErrInfo m_errLast = {ErrType::None, 0, nullptr}; \
		void setError(ErrType type = None, int code = 0, const char * strDescription = nullptr) \
		{ \
			m_errLast = TErrInfo(type, code, strDescription); \
		} \
		void transferError(IAbstract *pIface) \
		{ \
			m_errLast = pIface->GetError(); \
		} \
	public:\
		virtual int AddRef() { return ++m_nRefCount; } \
		virtual int Release() \
		{ \
			m_nRefCount--; \
			if (m_nRefCount <= 0) \
				delete this; \
			return m_nRefCount; \
		} \
		virtual TErrInfo GetError() \
		{ \
			return m_errLast; \
		}

//所有接口的基类
interface IAbstract
{
	//增加引用
	virtual int AddRef() = 0;

	//释放接口，使用完毕后调用此方法
	virtual int Release() = 0;

	//获取最近的一次错误信息
	virtual TErrInfo GetError() = 0;
};

//一个专用于接口的数组
class IFvector
{
public:
	~IFvector()
	{
		for (std::vector<IAbstract *>::iterator i = m_container.begin(); i != m_container.end(); ++i)
			(*i)->Release();
	}
	//插入一个接口
	void push_back(IAbstract * pInterface)
	{
		pInterface->AddRef();
		m_container.push_back(pInterface);
	}
	//删除最后一个接口
	void pop_back()
	{
		m_container[m_container.size() - 1]->Release();
		m_container.pop_back();
	}
	//获取下标为pos的接口，注意使用完毕后调用Release方法
	IAbstract * at(size_t pos)
	{
		IAbstract * ret = m_container.at(pos);
		ret->AddRef();
		return ret;
	}
	//获取下标为pos的接口，仅供临时访问用，使用完毕后不需要Release
	IAbstract * operator[](size_t pos)
	{
		return m_container[pos];
	}
	//获取数组中接口的个数
	size_t size() const
	{
		return m_container.size();
	}
protected:
	std::vector<IAbstract *> m_container;
};

template<typename iface>
struct auto_iface
{
	auto_iface()
	{
		pIface = nullptr;
	}
	~auto_iface()
	{
		if (pIface)
			((IAbstract*)pIface)->Release();
	}
	iface ** operator &()
	{
		return &pIface;
	}
	iface * operator->()
	{
		return pIface;
	}
	operator iface*()
	{
		return pIface;
	}
	IAbstract * abs_iface()
	{
		return (IAbstract*)pIface;
	}
	iface *pIface;
};

#endif