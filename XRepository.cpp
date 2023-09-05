#include "XRepository.h"
#include <QDir>

#include "XDirectory.h"
#include "XFile.h"
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  XRepository
//----------------------------------------------------------------------------- 

XRepository::XRepository()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  ~XRepository
//----------------------------------------------------------------------------- 

XRepository::~XRepository()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  setPath
// Params:  const QString& datapath
// Params:  const QString& backuppath
//----------------------------------------------------------------------------- 

void XRepository::setPath(const QString& datapath, const QString& repositorypath)
{
	setRepositoryPath(repositorypath);
	setDataPath(datapath);

}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  setBackupPath
// Params:  const QString& path
//----------------------------------------------------------------------------- 

void XRepository::setRepositoryPath(const QString& path)
{
	m_RepositoryPath = path;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  setDataPath
// Params:  const QString& path
//----------------------------------------------------------------------------- 
void XRepository::setDataPath(const QString& path)
{
	if (m_DataPath != path)
	{
		m_DataPath = path;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  version
// Params:  const QString& rootversion
// Params:  const QString& subversion
// Params:  bool run
//----------------------------------------------------------------------------- 

void XRepository::version(const QString& rootversion, const QString& subversion, bool run)
{
	
	QString dailybackup = QString("Version[%1]").arg(rootversion);
	if (!QDir(repositoryPath()).exists(dailybackup))
	{
		XDirectory::copy(dataPath(), repositoryPath() + QDir::separator() + dailybackup + QDir::separator() + "Full", m_IncludeFilter,true, run);
	}

	// FileBased Diff
	QString diffbackup = dailybackup + QDir::separator() + QString("Sub[%1]").arg(subversion);
	if (!QDir(repositoryPath()).exists(diffbackup))
	{
		QStringList backuppaths;
		for (const QString dirname : XDirectory::dirs(repositoryPath() + QDir::separator() + dailybackup))
		{
			if (dirname.startsWith("Full") || dirname.startsWith("Sub"))
				backuppaths << repositoryPath() + QDir::separator() + dailybackup + QDir::separator() + dirname;
		}
		XDirectory::diff(dataPath(), backuppaths, repositoryPath() + QDir::separator() + diffbackup,m_IncludeFilter, run);
	}

}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  wait
//----------------------------------------------------------------------------- 

void XRepository::wait()
{
	XFile::wait();
	XDirectory::wait();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XRepository
// Method:  rollback
// Params:  QDateTime rollbackdatetime
//----------------------------------------------------------------------------- 

bool XRepository::rollback(const QString& rootversion, const QString& subversion)
{
	QString fullrollbackup = QString("Version[%1]").arg(rootversion);
	if (!QDir(repositoryPath()).exists(fullrollbackup))
		return false;

	XDirectory::remove(dataPath(), false);

	QStringList backuppaths;
	for (const QString dirname : XDirectory::dirs(repositoryPath() + QDir::separator() + fullrollbackup))
	{
		if (dirname.startsWith("Full") || dirname.startsWith("Sub"))
			backuppaths << repositoryPath() + QDir::separator() + fullrollbackup + QDir::separator() + dirname;
	}
	XDirectory::merge(backuppaths, dataPath(), false);

	return true;
}

