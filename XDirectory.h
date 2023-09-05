#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QList>
#include <QFuture>
#include <QDir>
#include <QtConcurrent>

namespace CoreStorage
{
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  XDirectory
//----------------------------------------------------------------------------- 
	
class CORESTORAGE_EXPORT XDirectory
{
public:
	static bool copy(const QString& srcdirectory, const QString& dstdirectory,bool run=false);
	static bool copy(const QString& srcdirectory, const QString& dstdirectory, const QDateTime& fromdatetime, const QDateTime& todatetime, bool run = false);
	static bool copy(const QString& srcdirectory, const QString& dstdirectory, const QStringList& filter, bool include, bool run = false);
	static bool copy(const QString& srcdirectory, const QString& dstdirectory, const QString& filter, bool include, bool run = false);
	static bool copy(const QString& srcdirectory, const QString& dstdirectory, const QStringList& filter, bool include, const QDateTime& fromdatetime, const QDateTime& todatetime, bool run = false);
	static bool copy(const QString& srcdirectory, const QString& dstdirectory, const QString& filter, bool include, const QDateTime& fromdatetime, const QDateTime& todatetime, bool run = false);

	static bool diff(const QString& srcdirectory, const QString& diffdirectory, const QString& dstdirectory, bool run = false);
	static bool diff(const QString& srcdirectory, const QStringList& diffdirectory, const QString& dstdirectory, bool run = false);
	static bool diff(const QString& srcdirectory, const QString& diffdirectory, const QString& dstdirectory, const QStringList& filter, bool run = false);
	static bool diff(const QString& srcdirectory, const QStringList& diffdirectory, const QString& dstdirectory, const QStringList& filter, bool run = false);
	static bool diff(const QString& srcdirectory, const QString& diffdirectory, const QString& dstdirectory, const QString& filter, bool run = false);
	static bool diff(const QString& srcdirectory, const QStringList& diffdirectory, const QString& dstdirectory, const QString& filter, bool run = false);

	static bool merge(const QStringList& mergedirectory, const QString& dstdirectory, bool run = false);
	
	static bool compress(const QString& srcdirectory, const QString& dstdirectory, bool run = false);
	static bool unCompress(const QString& srcdirectory, const QString& dstdirectory, bool run = false);

	static bool crypt(const QString& srcdirectory, const QString& dstdirectory, const QString& key, bool run = false);
	static bool unCrypt(const QString& srcdirectory, const QString& dstdirectory, const QString& key, bool run = false);

	static bool remove(const QString& directory, bool run = false);
	static bool remove(const QString& directory,const QStringList& filenames, bool run = false);

	static bool exists(const QString& directory);
	static bool equal(const QString& directory, const QString& directory2);

	static bool clean(const QString& directory, const QString& filter, bool run = false);
	static bool clean(const QString& directory, const QStringList& filter, bool run = false);

	static QStringList files(const QString& directory, const QString& filter = "*.*", bool include=true);
	static QStringList files(const QString& directory, const QStringList& filter, bool include=true);
	static QStringList files(const QString& directory, const QString& filter, const QDateTime& fromdatetime, const QDateTime& todatetime, bool include = true);
	static QStringList files(const QString& directory, const QStringList& filter, const QDateTime& fromdatetime, const QDateTime& todatetime, bool include = true);

	static QStringList tree(const QString& directory, const QString& filter = "*.*", bool include=true);
	static QStringList tree(const QString& directory, const QStringList& filter, bool include=true);
	
	static QStringList dirs(const QString& directory, const QString& filter="*.*");
	static QStringList dirs(const QString& directory, const QStringList& filter);

	static QStringList dirsTree(const QString& directory);

	static bool mkPath(const QString& directory);
	
	static void wait();
private:
	static QJsonObject DirectoryJSON(const QString& directory);
	static QJsonObject DirectoriesJSON(const QString& srcdirectory, const QString& dstdirectory);


	static QStringList tree(const QString& directory, const QString& path,const QStringList& filter,bool include=true);
	static QStringList dirsTree(const QString& directory, const QString& path);

	static QList<QFuture<void> > m_Futures;

	template <class Function, class ...Args>
	static auto concurrent(Function&& f, Args &&...args)
	{
		append(QtConcurrent::run(std::forward<Function>(f), std::forward<Args>(args)...));
	}

	static void append(QFuture<void> future);
};


};
using namespace CoreStorage;
