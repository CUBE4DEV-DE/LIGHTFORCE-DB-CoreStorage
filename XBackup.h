#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QStringList>
#include <QObject>
#include <QTimer>

namespace CoreStorage
{
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XBackup
// Method:  XBackup
//----------------------------------------------------------------------------- 
class CORESTORAGE_EXPORT XBackup:public QObject
{
	Q_OBJECT
public:
	XBackup();
	virtual ~XBackup();

	void setPath(const QString& datapath,const QString& backuppath,int seconds=60*5);
	void setBackupPath(const QString& path) { m_BackupPath = path; }
	void setDataPath(const QString& path);

private slots:
	void timer();
public slots:
	void timestamp();
	void wait();
	void timestamp(const QDateTime& datetime,bool run=false);
	bool rollback(const QDateTime& rollbackdatetime);
private:
	
	const QString& backupPath() { return m_BackupPath; }
	const QString& dataPath() { return m_DataPath; }

	QString m_BackupPath;
	QString m_DataPath;
	QTimer m_Timer;
};


};
using namespace CoreStorage;