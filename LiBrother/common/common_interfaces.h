#ifndef __COMMON_INTERFACES_H__
#define __COMMON_INTERFACES_H__

#include <vector>

#ifndef interface
#define interface struct
#endif

//��������
enum ErrType
{
	None,
	InvalidParam, PermissionDenied,
	UnsupportedMethod, NetworkError, DatabaseError,
	Other
};

//����һ������
struct TErrInfo
{
	ErrType type;		//����
	int code;			//�������
	const char * description;	//���������[��ʹ��ʱ��Ϊnullptr]
	TErrInfo(ErrType type = None, int code = 0, const char * description = nullptr) : type(type), code(code), description(description) {}
};

/*	����ʵ��һ���ӿڣ��ڶ���classʱ����
	ʾ����
	class CUser : public IUser
	{
		IMPLEMENT_INTERFACE
	public:
		virtual bool deleteBook();
		//�ӿڵ����ʵ��
	protected:
		//��Ա����
		//������Ա����
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

//���нӿڵĻ���
interface IAbstract
{
	//��������
	virtual int AddRef() = 0;

	//�ͷŽӿڣ�ʹ����Ϻ���ô˷���
	virtual int Release() = 0;

	//��ȡ�����һ�δ�����Ϣ
	virtual TErrInfo GetError() = 0;
};

//һ��ר���ڽӿڵ�����
class IFvector
{
public:
	~IFvector()
	{
		for (std::vector<IAbstract *>::iterator i = m_container.begin(); i != m_container.end(); ++i)
			(*i)->Release();
	}
	//����һ���ӿ�
	void push_back(IAbstract * pInterface)
	{
		pInterface->AddRef();
		m_container.push_back(pInterface);
	}
	//ɾ�����һ���ӿ�
	void pop_back()
	{
		m_container[m_container.size() - 1]->Release();
		m_container.pop_back();
	}
	//��ȡ�±�Ϊpos�Ľӿڣ�ע��ʹ����Ϻ����Release����
	IAbstract * at(size_t pos)
	{
		IAbstract * ret = m_container.at(pos);
		ret->AddRef();
		return ret;
	}
	//��ȡ�±�Ϊpos�Ľӿڣ�������ʱ�����ã�ʹ����Ϻ���ҪRelease
	IAbstract * operator[](size_t pos)
	{
		return m_container[pos];
	}
	//��ȡ�����нӿڵĸ���
	size_t size() const
	{
		return m_container.size();
	}
protected:
	std::vector<IAbstract *> m_container;
};

#endif