#include "XBackup.h"
#include <QDir>

#include "XDirectory.h"
#include "XFile.h"
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  XBackup
//----------------------------------------------------------------------------- 

XBackup::XBackup()
{
	QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(timer()));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  ~XBackup
//----------------------------------------------------------------------------- 

XBackup::~XBackup()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  setPath
// Params:  const QString& datapath
// Params:  const QString& backuppath
//----------------------------------------------------------------------------- 

void XBackup::setPath(const QString& datapath, const QString& backuppath, int seconds)
{
	setBackupPath(backuppath);
	setDataPath(datapath);

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
// Class:   XBackup
// Method:  setDataPath
// Params:  const QString& path
//----------------------------------------------------------------------------- 
void XBackup::setDataPath(const QString& path)
{
	if (m_DataPath != path)
	{
		m_DataPath = path;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  timer
//----------------------------------------------------------------------------- 
void XBackup::timer()
{
	timestamp();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  timestamp
// Params:  const QDateTime& datetime
//----------------------------------------------------------------------------- 

void XBackup::timestamp(const QDateTime& datetime, bool run)
{
	QString dailybackup = QString("Bak[%1]").arg(datetime.toString("yyyyMMdd"));
	if (!QDir(backupPath()).exists(dailybackup))
	{
		XDirectory::copy(dataPath(), backupPath() + QDir::separator() + dailybackup + QDir::separator() + "Full",run);
	}

	// TimeBased Inc
	/*QString incbackup = dailybackup + QDir::separator() + QString("Inc[%1]").arg(datetime.toString("HHmmss"));
	if (!QDir(backupPath()).exists(incbackup))
	{
		QDateTime todate(datetime);
		
		QDateTime fromdate;
		fromdate.setDate(datetime.date());
		fromdate.setTime(QTime(0, 0, 0));
		
		XDirectory::copy(dataPath(), backupPath() + QDir::separator() + incbackup,
			fromdate, todate, run);
	}*/

	// FileBased Diff
	QString diffbackup = dailybackup + QDir::separator() + QString("Inc[%1]").arg(datetime.toString("HHmmss"));
	if (!QDir(backupPath()).exists(diffbackup))
	{
		QStringList backuppaths;
		for(const QString dirname: XDirectory::dirs(backupPath() + QDir::separator() + dailybackup))
		{
			if (dirname.startsWith("Full")|| dirname.startsWith("Inc"))
				backuppaths << backupPath() + QDir::separator() + dailybackup + QDir::separator() + dirname;
		}
		XDirectory::diff(dataPath(), backuppaths, backupPath() + QDir::separator() + diffbackup, run);
	}

}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  timestamp
//----------------------------------------------------------------------------- 

void XBackup::timestamp()
{
	QDateTime datetime(QDateTime::currentDateTime());
	timestamp(datetime,true);
	XFile::wait();
	XDirectory::wait();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  wait
//----------------------------------------------------------------------------- 

void XBackup::wait()
{
	XFile::wait();
	XDirectory::wait();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  rollback
// Params:  QDateTime rollbackdatetime
//----------------------------------------------------------------------------- 

bool XBackup::rollback(const QDateTime& rollbackdatetime)
{
	QString fullrollbackup = QString("Bak[%1]").arg(rollbackdatetime.toString("yyyyMMdd"));
	if (!QDir(backupPath()).exists(fullrollbackup))
		return false;

	XDirectory::remove(dataPath(), false);

	QStringList backuppaths;
	for (const QString dirname : XDirectory::dirs(backupPath() + QDir::separator() + fullrollbackup))
	{
		if (dirname.startsWith("Full") || dirname.startsWith("Inc"))
			backuppaths << backupPath() + QDir::separator() + fullrollbackup + QDir::separator() + dirname;
	}
	XDirectory::merge(backuppaths, dataPath(), false);

	return true;
}

