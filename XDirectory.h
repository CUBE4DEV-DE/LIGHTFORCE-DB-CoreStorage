#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QList>
#include <QFuture>
#include <QDir>
#include <QtConcurrent>

// The overall purpose of the file, is to provide a class called XDirectory with various
// methods that define copy, diff, merge, compress, decompress, encrypt, and remove
// operations. These operations are generally used to manipulate directories and
// their contents.
// The methods take source and destination directories, filtering options, and
// time ranges for selected files as parameters.Most of the operations are recursive,
// meaning they copy or diff all directories and files within the source directory,
// preserving their original file names and paths.There are also various means of
// filtering files in the operations, which could be useful in selecting particular
// file types or limiting the scope of operations.
// Overall, XDirectory offers a versatile set of tools for developers working
// with directories, making it easier to move, manipulate, and clean up
// directory structures.

namespace CoreStorage
{
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  XDirectory
// Description :
//	The purpose of these methods is to copy directories and their contents
//	to another location. They take various parameters such as the source
//	and destination directories, filtering options, and time ranges for
//	selected files. The methods use recursion to copy all directories and
//	files within the source directory. The copied contents will maintain
//	their original file names and paths.
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
