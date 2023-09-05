#pragma once
#include <QSystemSemaphore>

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XBlock
// Method:  XBlock
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

