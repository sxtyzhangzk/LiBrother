#ifndef __MAGIC_DB_H__
#define __MAGIC_DB_H__

/*
	MagicDB ���ݿ�ϵͳ
	Designed for LiBrother
	Powered by ZK
*/

//�ӿڶ����ļ�

#include "common_interfaces.h"
#include "mdbutils.h"
#include "mdberror.h"

//��¼��
interface IRecordset
{
	//��ȡ��¼����С�����󷵻�-1
	virtual int getSize() = 0;

	//��һ����¼
	virtual bool nextRecord() = 0;

	//��һ����¼
	virtual bool prevRecord() = 0;

	/*
	��ȡ��Binary�������������
	������	column	[in]	�����ƻ������
	*/
	virtual MVariant getData(const MVariant& column) = 0;

	//��ȡBinary�������ݵĴ�С
	virtual int getBinarySize(const MVariant& column) = 0;

	/*
	��ȡBinary����
	������	column		[in]	�����ƻ������
			buffer		[out]	�������ݵĻ�����
			nOffset		[in]	��nOffsetλ�ÿ�ʼ��ȡ
			nDataSize	[in]	��ȡ�����ݴ�С
	*/
	virtual bool getBinary(const MVariant& column, char * buffer, size_t nOffset, size_t nDataSize) = 0;

	/*
	������һ����¼
	������	column	[in]	��Ҫ������������
			cmp		[in]	�ж�����(���ڣ����ڣ�С�ڣ������ڣ����ڵ��ڣ�С�ڵ��ڣ�����)
			cmpVal	[in]	��Ҫ�Ƚϵ�����
	*/
	virtual bool findNext(const MVariant& column, MCompare cmp, const MVariant& cmpVal) = 0;

	//������һ����¼
	virtual bool findPrev(const MVariant& column, MCompare cmp, const MVariant& cmpVal) = 0;

	//�趨����
	virtual bool setData(const MVariant& column, const MVariant& val) = 0;

	//�趨Binary���ݵĳ���
	virtual bool setBinarySize(const MVariant& column, size_t nSize) = 0;

	//�趨Binary���͵����ݣ������趨���Ƚ��Զ��Ӵ�ռ�
	virtual bool setBinary(const MVariant& column, const char * buffer, size_t nOffset, size_t nDataSize) = 0;

	//������һ�У�������¼����һ��ʵ�ʵ����ݱ�ʱ����
	virtual bool addNew() = 0;

	//ɾ����ǰ�У�������¼����һ��ʵ�ʵ����ݱ�ʱ����
	virtual bool deleteThis() = 0;

	//�������ݿ⣬�����޸���Ϻ����
	virtual bool updateDatabase() = 0;
};

interface IDatabase
{
	/*
	��ȡһ�����ݱ�
	������	strTable	[in]	�������
			ppRecordset	[out]	ָ��IRecordset *��ָ��
	*/
	virtual bool getTable(const char * strTable, IRecordset ** ppRecordset) = 0;

	//ִ��һ��SQL��䣬���ز�ѯ����ļ�¼��
	virtual bool executeSQL(const char * strSQL, IRecordset ** ppRecordset) = 0;
};

interface IDBMS
{
	/*
	��һ�����е����ݿ�
	������	strDBFile	[in]	�ļ���·��
			ppDatabase	[out]	ָ��IDatabase *��ָ��
	*/
	virtual bool openDatabase(const char * strDBFile, IDatabase ** ppDatabase) = 0;

	//����һ���µ����ݿ�
	virtual bool createDatabase(const char * strDBFile) = 0;
};

//ȡ��IDBMS����
bool getDBMS(IDBMS ** ppDBMS);

//��ȡ���ݿ�İ汾
int getVersion();

#endif