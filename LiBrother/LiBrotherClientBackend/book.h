#ifndef __BOOK_H__
#define __BOOK_H__

#include "function_interfaces.h"
#include "common_types.h"
#include<string>
class CBook : public IBook
{
	IMPLEMENT_INTERFACE
public:
	CBook();
	CBook(TBookBasicInfo *Info);
	~CBook();
	virtual bool getBasicInfo(TBookBasicInfo& info) override;
	virtual bool getDescription(std::string& description) override;
	virtual bool setBasicInfo(const TBookBasicInfo& info) override;
	virtual bool setDescription(const char * description) override;
	virtual bool deleteBook(int number) override;
	virtual bool getBorrowInfo(std::vector<TBorrowInfo> &binfo) override;
	bool insert();
	bool check(TBookBasicInfo info_to_check);
protected:
	TBookBasicInfo * m_CBBI;
	std::string m_Description;
	int m_Id;
};

#endif