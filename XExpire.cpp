#include "XExpire.h"
#include <QDir>

#include "XDirectory.h"
#include "XFile.h"
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  XExpire
//----------------------------------------------------------------------------- 

XExpire::XExpire()
{
	m_MaxMinutes = 60 * 24;
	QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(timer()));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  ~XExpire
//----------------------------------------------------------------------------- 

XExpire::~XExpire()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  setPath
// Params:  const QString& datapath
// Params:  const QString& backuppath
//----------------------------------------------------------------------------- 

void XExpire::setPath(const QString& datapath, qint32 maxminutes, int seconds)
{
	m_DataPath=datapath;
	m_MaxMinutes = maxminutes;
	if (seconds > 0)
	{
		m_Timer.setInterval(seconds * 1000);
		m_Timer.start();
	}
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  timer
//----------------------------------------------------------------------------- 
void XExpire::timer()
{
	flush();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  timestamp
// Params:  const QDateTime& datetime
//----------------------------------------------------------------------------- 

void XExpire::flush(const QDateTime& datetime, bool run)
{
	QDateTime fromdatetime(QDateTime::currentDateTime());
	QDateTime todatetime(QDateTime::currentDateTime());

	fromdatetime = fromdatetime.addYears(-100);
	todatetime=todatetime.addSecs(-60 * m_MaxMinutes);

	QStringList files=XDirectory::files(dataPath(), "*.*", fromdatetime, todatetime);
	XDirectory::remove(dataPath(),files, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  timestamp
//----------------------------------------------------------------------------- 

void XExpire::flush()
{
	QDateTime datetime(QDateTime::currentDateTime());
	flush(datetime, true);
	XFile::wait();
	XDirectory::wait();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XExpire
// Method:  wait
//----------------------------------------------------------------------------- 

void XExpire::wait()
{
	XFile::wait();
	XDirectory::wait();
}

