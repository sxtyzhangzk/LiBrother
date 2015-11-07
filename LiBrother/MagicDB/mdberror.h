#ifndef __MDB_ERROR_H__
#define __MDB_ERROR_H__

//Êý¾Ý¿âµÄ´íÎó
enum MError
{
	MErr_Unknown = 1,
	MErr_FileNotExist = 2,
	MErr_FileExist = 3,
	MErr_InvalidFile = 4,
	MErr_FileSysError = 5,
	MErr_TableNotExist = 6,
	MErr_SQLCompileFailed = 7,
	MErr_SQLExecuteFailed = 8,
	MErr_BOF = 9,
	MErr_EOF = 10,
	MErr_ColumnNotExist = 11,
	MErr_BinaryColumn = 12,
	MErr_NotBinaryColumn = 13,
	MErr_NullData = 14,
	MErr_RecordsetNotTable = 15,
	MErr_SizeOutOfLimit = 16,
	MErr_InternalError = 17
};

#endif