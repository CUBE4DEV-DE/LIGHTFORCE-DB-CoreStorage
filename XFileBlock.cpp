#include "XFileBlock.h"


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFileBlock
// Method:  m_GlobalContext
//----------------------------------------------------------------------------- 

QString XFileBlock::m_Context("XFILE");

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileBlock
// Method:  XFileBlock
//----------------------------------------------------------------------------- 
XFileBlock::XFileBlock()
{
	m_SemaphoreWrite = Q_NULLPTR;
	m_SemaphoreRead = Q_NULLPTR;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XSystemBlock
// Method:  XSystemBlock
// Params:  const QString& keyname
//----------------------------------------------------------------------------- 

XFileBlock::XFileBlock(const QString& keyname, bool write)
{
	m_SemaphoreWrite = Q_NULLPTR;
	m_SemaphoreRead = Q_NULLPTR;

	lock(keyname, write);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XSystemBlock
// Method:  ~XSystemBlock
//----------------------------------------------------------------------------- 

XFileBlock::~XFileBlock()
{
	unLock();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFileBlock
// Method:  lock
// Params:  const QString& keyname
// Params:  bool write
//----------------------------------------------------------------------------- 

void XFileBlock::lock(const QString& keyname, bool write)
{
	if (write)
	{
		lockWrite(keyname);
	}
	else
	{
		lockRead(keyname);
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFileBlock
// Method:  lockWrite
// Params:  const QString& keyname
//----------------------------------------------------------------------------- 

void XFileBlock::lockWrite(const QString& keyname)
{
	// Wait Stop For Read
	m_SemaphoreRead = new QSystemSemaphore(m_Context + keyname + "R", 1, QSystemSemaphore::Open);
	m_SemaphoreRead->acquire();
	m_SemaphoreRead->release();
	delete m_SemaphoreRead;
	m_SemaphoreRead = Q_NULLPTR;

	// Start Write
	m_SemaphoreWrite = new QSystemSemaphore(m_Context + keyname + "W", 1, QSystemSemaphore::Open);
	m_SemaphoreWrite->acquire();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFileBlock
// Method:  lockRead
// Params:  const QString& keyname
//----------------------------------------------------------------------------- 

void XFileBlock::lockRead(const QString& keyname)
{
	// Wait Stop For Write
	m_SemaphoreWrite = new QSystemSemaphore(m_Context + keyname + "W", 1, QSystemSemaphore::Open);
	m_SemaphoreWrite->acquire();
	m_SemaphoreWrite->release();
	delete m_SemaphoreWrite;
	m_SemaphoreWrite = Q_NULLPTR;

	// Start Read
	m_SemaphoreRead = new QSystemSemaphore(m_Context + keyname + "R", 1, QSystemSemaphore::Open);
	m_SemaphoreRead->acquire();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFileBlock
// Method:  unLock
//----------------------------------------------------------------------------- 

void XFileBlock::unLock()
{
	// Stop Write
	if (m_SemaphoreWrite)
	{
		m_SemaphoreWrite->release();
		delete m_SemaphoreWrite;
		m_SemaphoreWrite = Q_NULLPTR;
	}

	// Stop Read
	if (m_SemaphoreRead)
	{
		m_SemaphoreRead->release();
		delete m_SemaphoreRead;
		m_SemaphoreRead = Q_NULLPTR;
	}

}
