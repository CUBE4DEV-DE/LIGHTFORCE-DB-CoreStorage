#pragma once
#include <QSystemSemaphore>

namespace CoreStorage
{

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XBlock
// Method:  XBlock
// Description :
//	This class has several public methods.The lock method acquires a
//	lock on a file identified by its key name.The read_write flag
//	is used to specify whether the lock is for reading or writing.
//	The lockRead and lockWrite methods acquire a read - only or write -
//	only lock respectively.The unlock method releases the lock on a file.
// 	The class also has two private attributes : m_SemaphoreRead and
// 	m_SemaphoreWrite.They are of type QSystemSemaphore and are used as
// 	read and write locks.The static method setContext allows setting
// 	the context attribute of the class.
//----------------------------------------------------------------------------- 

class XFileBlock
{
	public:
	XFileBlock();
	XFileBlock(const QString& keyname,bool write=false);
	virtual ~XFileBlock();

	static void setContext(const QString& context)
	{
		m_Context = context;
	}

	void lock(const QString& keyname, bool write = false);
	void lockWrite(const QString& keyname);
	void lockRead(const QString& keyname);
	void unLock();

	private:
	QSystemSemaphore* m_SemaphoreRead;
	QSystemSemaphore* m_SemaphoreWrite;

	static QString m_Context;
};


};
using namespace CoreStorage;

