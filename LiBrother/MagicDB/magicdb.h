#ifndef __MAGIC_DB_H__
#define __MAGIC_DB_H__

/*
	MagicDB 数据库系统
	Designed for LiBrother
	Powered by ZK
*/

//接口定义文件

#include "common_interfaces.h"
#include "mdbutils.h"
#include "mdberror.h"

//记录集
interface IRecordset
{
	//获取记录集大小，错误返回-1
	virtual int getSize() = 0;

	//下一条记录
	virtual bool nextRecord() = 0;

	//上一条记录
	virtual bool prevRecord() = 0;

	/*
	获取除Binary类型以外的数据
	参数：	column	[in]	列名称或列序号
	*/
	virtual MVariant getData(const MVariant& column) = 0;

	//获取Binary类型数据的大小
	virtual int getBinarySize(const MVariant& column) = 0;

	/*
	读取Binary类型
	参数：	column		[in]	列名称或列序号
			buffer		[out]	保存数据的缓存区
			nOffset		[in]	从nOffset位置开始读取
			nDataSize	[in]	读取的数据大小
	*/
	virtual bool getBinary(const MVariant& column, char * buffer, size_t nOffset, size_t nDataSize) = 0;

	/*
	查找下一条记录
	参数：	column	[in]	需要满足条件的列
			cmp		[in]	判断条件(等于，大于，小于，不等于，大于等于，小于等于，相似)
			cmpVal	[in]	需要比较的数据
	*/
	virtual bool findNext(const MVariant& column, MCompare cmp, const MVariant& cmpVal) = 0;

	//查找上一条记录
	virtual bool findPrev(const MVariant& column, MCompare cmp, const MVariant& cmpVal) = 0;

	//设定数据
	virtual bool setData(const MVariant& column, const MVariant& val) = 0;

	//设定Binary数据的长度
	virtual bool setBinarySize(const MVariant& column, size_t nSize) = 0;

	//设定Binary类型的内容，多于设定长度将自动加大空间
	virtual bool setBinary(const MVariant& column, const char * buffer, size_t nOffset, size_t nDataSize) = 0;

	//新增加一行，仅当记录集是一个实际的数据表时可用
	virtual bool addNew() = 0;

	//删除当前行，仅当记录集是一个实际的数据表时可用
	virtual bool deleteThis() = 0;

	//更新数据库，数据修改完毕后调用
	virtual bool updateDatabase() = 0;
};

interface IDatabase
{
	/*
	获取一个数据表
	参数：	strTable	[in]	表的名称
			ppRecordset	[out]	指向IRecordset *的指针
	*/
	virtual bool getTable(const char * strTable, IRecordset ** ppRecordset) = 0;

	//执行一条SQL语句，返回查询结果的记录集
	virtual bool executeSQL(const char * strSQL, IRecordset ** ppRecordset) = 0;
};

interface IDBMS
{
	/*
	打开一个现有的数据库
	参数：	strDBFile	[in]	文件的路径
			ppDatabase	[out]	指向IDatabase *的指针
	*/
	virtual bool openDatabase(const char * strDBFile, IDatabase ** ppDatabase) = 0;

	//创建一个新的数据库
	virtual bool createDatabase(const char * strDBFile) = 0;
};

//取得IDBMS对象
bool getDBMS(IDBMS ** ppDBMS);

//获取数据库的版本
int getVersion();

#endif