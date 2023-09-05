#pragma once


#include "CoreStorageGlobal.h"

#include <QString>
#include <QFuture>
#include <QList>
#include <QtConcurrent>
#include <QRegularExpression>

namespace CoreStorage
{
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  XMetaFile
//----------------------------------------------------------------------------- 

class CORESTORAGE_EXPORT XMetaFile  : public QObject
{
	Q_OBJECT
public:

	// NoAdd '' Full Add '+' WriteOnly  Add '-' ReadOnly 
	static bool access(const QString& filename, const QString& user=QString(""), const QString& group= QString(), const QString& role=QString(), const QString& password= QString());
	static bool access(const QString& filename, const QStringList& users=QStringList(), const QStringList& groups=QStringList(), const QStringList& roles=QStringList(), const QStringList& passwords=QStringList());

	static bool isAccess(const QString& filename, const QString& user=QString(""), const QString& group= QString(), const QString& role=QString(), const QString& password=QString());
	static bool isAccessRead(const QString& filename, const QString& user = QString(""), const QString& group = QString(), const QString& role = QString(), const QString& password = QString());
	static bool isAccessWrite(const QString& filename, const QString& user = QString(""), const QString& group = QString(), const QString& role = QString(), const QString& password = QString());

	static bool accessUser(const QString& filename, const QString& user,bool readonly=false);
	static bool accessGroup(const QString& filename, const QString& group, bool readonly = false);
	static bool accessRole(const QString& filename, const QString& role, bool readonly = false);
	static bool accessPassword(const QString& filename, const QString& password, bool readonly = false);
	
	static bool isAccessUser(const QString& filename, const QString& user, bool readonly = false);
	static bool isAccessGroup(const QString& filename, const QString& group, bool readonly = false);
	static bool isAccessRole(const QString& filename, const QString& role, bool readonly = false);
	static bool isAccessPassword(const QString& filename, const QString& password, bool readonly = false);

	static bool touchRead(const QString& filename, const QString& user=QString(""), const QString& group=QString(), const QString& role=QString(), const QDateTime& timestamp=QDateTime::currentDateTime());
	static bool touchWrite(const QString& filename, const QString& user=QString(""), const QString& group=QString(), const QString& role=QString(), const QDateTime& timestamp=QDateTime::currentDateTime());
	static bool touchCreate(const QString& filename, const QString& user=QString(""), const QString& group=QString(), const QString& role=QString(), const QDateTime& timestamp=QDateTime::currentDateTime());
	static bool touchCopy(const QString& filename, const QString& user=QString(""), const QString& group=QString(), const QString& role=QString(), const QDateTime& timestamp=QDateTime::currentDateTime());
	static bool touchLog(const QString& filename, const QString& info, const QString& user=QString(""), const QString& group=QString(), const QString& role=QString(), const QDateTime& timestamp=QDateTime::currentDateTime());

	static bool remove(const QString& filename);
	static bool removeLog(const QString& filename);
	static bool removeAccess(const QString& filename);

	static bool touch(const QString& filename, const QString& info, const QString& user=QString(""), const QString& group=QString(), const QString& role=QString(), const QDateTime& timestamp=QDateTime::currentDateTime());

	static QString fileNameAccess(const QString& filename) { return filename + ".acc.meta"; }
	static QString fileNameLog(const QString& filename) { return filename + ".log.meta"; }

	static void setMaxTouch(qint32 max = 1024) { m_MaxTouch = max; }
private:

	static bool accessProp(const QString& filename, const QString& prop, const QString& value);

	static qint32 m_MaxTouch;
};


};
using namespace CoreStorage;

