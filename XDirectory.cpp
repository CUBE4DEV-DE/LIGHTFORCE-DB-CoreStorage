#include "XDirectory.h"

#include <QFileInfo>
#include "XFile.h"
#include "XLogInject.h"

#include "../CoreBase/XDebug.h"

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  m_Futures
//----------------------------------------------------------------------------- 

QList<QFuture<void> > XDirectory::m_Futures;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copyDirectory
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
//----------------------------------------------------------------------------- 

bool XDirectory::copy(const QString& srcdirectory, const QString& dstdirectory, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "copy", "Trace", "Ok", DirectoriesJSON(srcdirectory,dstdirectory));
	}

	return copy(srcdirectory, dstdirectory, QStringList(),  true,run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copyDirectory
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QDateTime& fromdatetime
// Params:  const QDateTime& todatetime
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XDirectory::copy(const QString& srcdirectory, const QString& dstdirectory, const QDateTime& fromdatetime,const QDateTime& todatetime, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		QJsonObject obj = DirectoriesJSON(srcdirectory, dstdirectory);
		obj["FromDateTime"] = fromdatetime.toString(Qt::ISODate);
		obj["ToDateTime"] = todatetime.toString(Qt::ISODate);
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "copy FromTo", "Trace", "Ok", obj);
	}

	return copy(srcdirectory, dstdirectory, QStringList(),true, fromdatetime, todatetime, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copy
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QStringList& filter
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::copy(const QString& srcdirectory, const QString& dstdirectory, const QStringList& filter, bool include, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		QJsonObject obj = DirectoriesJSON(srcdirectory, dstdirectory);
		obj["Filter"] = filter.join(' ');
		obj["Include"] = include;
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "copy Filter", "Trace", "Ok", obj);
	}

	for (const QString& dirname : dirs(srcdirectory))
	{
		QDir(dstdirectory).mkpath(dirname);
		copy(srcdirectory + QDir::separator() + dirname, dstdirectory + QDir::separator() + dirname,filter,include, run);
	}

	mkPath(dstdirectory);

	if (filter.isEmpty())
	{
		for (const QString& filename : files(srcdirectory))
		{
			XFile::copy(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, run);
		}
	}
	else
	{
		for (const QString& filename : files(srcdirectory, filter,include))
		{
			XFile::copy(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, run);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copy
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QStringList& filter
// Params:  const QDateTime& fromdatetime
// Params:  const QDateTime& todatetime
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::copy(const QString& srcdirectory, const QString& dstdirectory, const QStringList& filter, bool include, const QDateTime& fromdatetime, const QDateTime& todatetime, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		QJsonObject obj = DirectoriesJSON(srcdirectory, dstdirectory);
		obj["Filter"] = filter.join(' ');
		obj["Include"] = include;
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "copy Filter", "Trace", "Ok", obj);
	}

	for (const QString& dirname : dirs(srcdirectory))
	{
		//QDir(dstdirectory).mkpath(dirname);
		copy(srcdirectory + QDir::separator() + dirname, dstdirectory + QDir::separator() + dirname,filter, include,fromdatetime, todatetime,run);
	}

	QStringList foundfiles;

	if (filter.isEmpty())
		foundfiles=files(srcdirectory, "*.*", fromdatetime, todatetime);
	else
		foundfiles=files(srcdirectory, filter, fromdatetime, todatetime,include);

	if (!foundfiles.isEmpty())
		mkPath(dstdirectory);

	for (const QString& filename : foundfiles)
	{
		XFile::copy(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, run);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copy
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QString& filter
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::copy(const QString& srcdirectory, const QString& dstdirectory, const QString& filter, bool include, bool run)
{
	return copy(srcdirectory, dstdirectory, QStringList() << filter,include, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copy
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QString& filter
// Params:  const QDateTime& fromdatetime
// Params:  const QDateTime& todatetime
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::copy(const QString& srcdirectory, const QString& dstdirectory, const QString& filter, bool include, const QDateTime& fromdatetime, const QDateTime& todatetime, bool run)
{
	return copy(srcdirectory, dstdirectory, QStringList() << filter, include,fromdatetime, todatetime,run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copyDirectoryDiff
// Params:  const QString& srcdirectory
// Params:  const QString& diffdirectory
// Params:  const QString& dstdirectory
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XDirectory::diff(const QString& srcdirectory, const QString& diffdirectory,const QString& dstdirectory, bool run)
{
	return diff(srcdirectory, QStringList() << diffdirectory, dstdirectory, QStringList(), run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  copyDirectoryDiff
// Params:  const QString& srcdirectory
// Params:  const QStringList& diffdirectory
// Params:  const QString& dstdirectory
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::diff(const QString& srcdirectory, const QStringList& diffdirectory,const QString& dstdirectory, bool run)
{
	return diff(srcdirectory, diffdirectory, dstdirectory, QStringList(), run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  diff
// Params:  const QString& srcdirectory
// Params:  const QString& diffdirectory
// Params:  const QString& dstdirectory
// Params:  const QStringList& filter
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::diff(const QString& srcdirectory, const QString& diffdirectory, const QString& dstdirectory,const QStringList& filter, bool run)
{
	return diff(srcdirectory, QStringList() << diffdirectory, dstdirectory, filter, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  diff
// Params:  const QString& srcdirectory
// Params:  const QStringList& diffdirectory
// Params:  const QString& dstdirectory
// Params:  const QStringList& filter
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XDirectory::diff(const QString& srcdirectory, const QStringList& diffdirectory, const QString& dstdirectory,const QStringList& filter, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		QJsonObject obj = DirectoriesJSON(srcdirectory, dstdirectory);
		obj["Filter"] = filter.join(' ');

		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "diff", "Trace", "Ok", obj);
	}

	//O_STACK(XDirectory, diff);
//O_PARAM(srcdirectory);
//O_PARAM(dstdirectory);


	bool filecopy = false;

	for (const QString& dirname : dirs(srcdirectory))
	{
		QStringList diffdirs;
		for (const QString& diffpath : diffdirectory)
			diffdirs << diffpath + QDir::separator() + dirname;

		if (diff(srcdirectory + QDir::separator() + dirname, diffdirs, dstdirectory + QDir::separator() + dirname,filter, run))
			filecopy = true;
	}

	QStringList foundfiles;

	if (!filter.isEmpty())
		foundfiles = files(srcdirectory, filter);
	else
		foundfiles = files(srcdirectory, "*.*");

	for (const QString& filename : foundfiles)
	{
		//O_PARAM(filename);

		bool newfile = true;

		QFileInfo srcinfo(srcdirectory + QDir::separator() + filename);
		srcinfo.refresh();

		//O_PARAM(srcinfo.lastModified().toString());
		//O_PARAM(srcinfo.birthTime().toString());
		//O_PARAM(srcinfo.size());

		for (const QString& diffpath : diffdirectory)
		{
			if (XFile::exists(diffpath + QDir::separator() + filename))
			{
				QFileInfo dstinfo(diffpath + QDir::separator() + filename);
				dstinfo.refresh();

				//O_PARAM(dstinfo.lastModified().toString());
				//O_PARAM(dstinfo.birthTime().toString());
				//O_PARAM(dstinfo.size());

				if (srcinfo.lastModified() == dstinfo.lastModified() &&
					//srcinfo.birthTime() == dstinfo.birthTime() &&
					srcinfo.size() == dstinfo.size())
					newfile = false;
			}
		}

		if (newfile)
			XFile::copy(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, run);
	}

	return true;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  diff
// Params:  const QString& srcdirectory
// Params:  const QString& diffdirectory
// Params:  const QString& dstdirectory
// Params:  const QString& filter
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::diff(const QString& srcdirectory, const QString& diffdirectory, const QString& dstdirectory,const QString& filter, bool run)
{
	return diff(srcdirectory, diffdirectory, dstdirectory, QStringList() << filter, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  diff
// Params:  const QString& srcdirectory
// Params:  const QStringList& diffdirectory
// Params:  const QString& dstdirectory
// Params:  const QString& filter
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::diff(const QString& srcdirectory, const QStringList& diffdirectory, const QString& dstdirectory,const QString& filter, bool run)
{
	return diff(srcdirectory, diffdirectory, dstdirectory, QStringList() << filter, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  merge
// Params:  const QString& srcdirectory
// Params:  const QStringList& mergedirectory
// Params:  const QString& dstdirectory
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::merge(const QStringList& mergedirectory, const QString& dstdirectory,bool run)
{
	bool filecopy = false;

	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "merge", "Trace", "Ok", DirectoryJSON(dstdirectory));
	}

	{
		// Sub Merges

		// All Sub dirs
		QSet<QString>subdirs;
		for (const QString& pathname : mergedirectory)
		{
			for (const QString& dirname : dirs(pathname))
				subdirs.insert(dirname);
		}

		// Merge activ directories
		for (const QString& dirname : subdirs.values())
		{
			QStringList mergeddirs;
			for (const QString& pathname : mergedirectory)
			{
				if (exists(pathname+QDir::separator()+dirname))
					mergeddirs << pathname + QDir::separator() + dirname;
			}

			if (!mergeddirs.isEmpty())
				merge(mergeddirs, dstdirectory + QDir::separator() + dirname, run);
		}
	}

	{
		// Sub Merges

		// All Files
		QSet<QString>allfiles;
		for (const QString& pathname : mergedirectory)
		{
			for (const QString& dirname : files(pathname,"*.*"))
				allfiles.insert(dirname);
		}

		for (const QString& filename : allfiles.values())
		{
			QString pathfile;
			QFileInfo pathinfo;

			for (const QString& pathname : mergedirectory)
			{
				if (XFile::exists(pathname + QDir::separator() + filename))
				{
					QFileInfo srcinfo(pathname + QDir::separator() + filename);
					srcinfo.refresh();

					if (pathfile.isEmpty()|| srcinfo.lastModified() > pathinfo.lastModified())
					{
						pathfile = pathname + QDir::separator() + filename;
						pathinfo.setFile(pathfile);
						pathinfo.refresh();
					}
				}
			}

			// Copy newest File
			if (!pathfile.isEmpty())
				XFile::copy(pathfile, dstdirectory + QDir::separator() + filename, run);
		}

		
	}

	return true;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  compressDirectory
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::compress(const QString& srcdirectory, const QString& dstdirectory, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "compress", "Trace", "Ok", DirectoriesJSON(srcdirectory, dstdirectory));
	}

	for (const QString& dirname : dirs(srcdirectory))
	{
		QDir(dstdirectory).mkpath(dirname);

		compress(srcdirectory + QDir::separator() + dirname, dstdirectory + QDir::separator() + dirname, run);
	}

	mkPath(dstdirectory);

	for (const QString& filename : files(srcdirectory))
	{
		XFile::compress(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, run);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  unCompressDirectory
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::unCompress(const QString& srcdirectory, const QString& dstdirectory, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "unCompress", "Trace", "Ok", DirectoriesJSON(srcdirectory, dstdirectory));
	}

	for (const QString& dirname : dirs(srcdirectory))
	{
		QDir(dstdirectory).mkpath(dirname);

		compress(srcdirectory + QDir::separator() + dirname, dstdirectory + QDir::separator() + dirname, run);
	}

	mkPath(dstdirectory);

	for (const QString& filename : files(srcdirectory))
	{
		XFile::unCompress(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, run);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  crypt
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QString& key
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XDirectory::crypt(const QString& srcdirectory, const QString& dstdirectory, const QString& key, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "crypt", "Trace", "Ok", DirectoriesJSON(srcdirectory, dstdirectory));
	}

	for (const QString& dirname : dirs(srcdirectory))
	{
		QDir(dstdirectory).mkpath(dirname);

		crypt(srcdirectory + QDir::separator() + dirname, dstdirectory + QDir::separator() + dirname, key, run);
	}

	mkPath(dstdirectory);

	for (const QString& filename : files(srcdirectory))
	{
		XFile::crypt(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename, key, run);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  unCrypt
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
// Params:  const QString& key
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::unCrypt(const QString& srcdirectory, const QString& dstdirectory, const QString& key, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "unCrypt", "Trace", "Ok", DirectoriesJSON(srcdirectory, dstdirectory));
	}

	for (const QString& dirname : dirs(srcdirectory))
	{
		QDir(dstdirectory).mkpath(dirname);

		unCrypt(srcdirectory + QDir::separator() + dirname, dstdirectory + QDir::separator() + dirname,key, run);
	}

	mkPath(dstdirectory);

	for (const QString& filename : files(srcdirectory))
	{
		XFile::unCrypt(srcdirectory + QDir::separator() + filename, dstdirectory + QDir::separator() + filename,key, run);
	}
	return true;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  removeDirectory
// Params:  const QString& directory
// Params:  const QStringList& filter
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XDirectory::remove(const QString& directory,  bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "remove", "Trace", "Ok", DirectoryJSON(directory));
	}

	if (run)
	{
		concurrent([directory,run]
			{
				for (const QString& dirname : dirs(directory))
				{
					remove(directory + QDir::separator() + dirname, run);
				}

				for (const QString& filename : files(directory))
				{
					XFile::remove(directory + QDir::separator() + filename, run);
				}

				for (const QString& dirname : dirs(directory))
				{
					QDir(directory).rmdir(dirname);
				}

			});
	}
	else
	{
		for (const QString& dirname : dirs(directory))
		{
			remove(directory + QDir::separator() + dirname, run);
		}

		for (const QString& filename : files(directory))
		{
			XFile::remove(directory + QDir::separator() + filename, run);
		}

		for (const QString& dirname : dirs(directory))
		{
			QDir(directory).rmdir(dirname);
		}

		QDir(directory).rmdir(".");
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  remove
// Params:  const QString& directory
// Params:  const QStringList& filenames
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

bool XDirectory::remove(const QString& directory, const QStringList& filenames, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "remove", "Trace", "Ok", DirectoryJSON(directory));
	}

	bool ret=true;
	for (const QString& filename : filenames)
		if (!XFile::remove(directory + QDir::separator() + filename, run))
			ret = false;
	return ret;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  exists
// Params:  const QString& directory
// Params:  
//----------------------------------------------------------------------------- 

bool XDirectory::exists(const QString& directory)
{
	return QDir(directory).exists();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  equal
// Params:  const QString& directory
// Params:  const QString& directory2
//----------------------------------------------------------------------------- 

bool XDirectory::equal(const QString& directory, const QString& directory2)
{
	//O_STACK(XDirectory, equal)
	//O_PARAM(directory);
	//O_PARAM(directory2);

	QStringList files = tree(directory);
	QStringList files2 = tree(directory2);

	if (files.count() != files2.count())
	{
		O_PARAM(files.count());
		O_PARAM(files2.count());
		return false;
	}

	files.sort();
	files2.sort();

	if (files != files2)
	{
		for (const QString& filepath : files)
		{
			O_PARAM(filepath);
		}
		for (const QString& filepath : files2)
		{
			O_PARAM(filepath);
		}
		return false;
	}

	for(const QString& filepath:files)
	{
		if (!XFile::equal(directory + QDir::separator() + filepath, directory2 + QDir::separator() + filepath))
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  cleanDirectory
// Params:  const QString& directory
// Params:  const QString& filefilter
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XDirectory::clean(const QString& directory, const QString& filefilter, bool run)
{
	return clean(directory, QStringList()<<filefilter,run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  cleanDirectory
// Params:  const QString& directory
// Params:  const QStringList& filefilter
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XDirectory::clean(const QString& directory, const QStringList& filefilter, bool run)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "clean", "Trace", "Ok", DirectoryJSON(directory));
	}

	if (run)
	{
		concurrent([directory,filefilter,run]
			{
				for (const QString& dirname : dirs(directory))
				{
					clean(directory + QDir::separator() + dirname, filefilter, run);
				}

				for (const QString& filename : files(directory, filefilter))
				{
					XFile::remove(directory + QDir::separator() + filename, run);
				}
			});
	}
	else
	{
		for (const QString& dirname : dirs(directory))
		{
			clean(directory + QDir::separator() + dirname, filefilter, run);
		}

		for (const QString& filename : files(directory, filefilter))
		{
			XFile::remove(directory + QDir::separator() + filename);
		}
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  files
// Params:  const QString& directory
// Params:  const QString& filter
//----------------------------------------------------------------------------- 
QStringList XDirectory::files(const QString& directory, const QString& filter, bool include)
{
	return files(directory, QStringList()<<filter,include);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  files
// Params:  const QString& directory
// Params:  const QStringList& filter
//----------------------------------------------------------------------------- 

QStringList XDirectory::files(const QString& directory, const QStringList& filter, bool include)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "files", "Trace", "Ok", DirectoryJSON(directory));
	}

	QStringList entries;
	QDir dir(directory);

	if (include)
	{
		for (QFileInfo fileinfo : dir.entryInfoList(filter, QDir::Files, QDir::Name))
			entries << fileinfo.fileName();
	}
	else
	{
		QList<QRegularExpression> reglist;
		for (const QString& wildcardfilter : filter)
		{
			QRegularExpression expr = QRegularExpression::fromWildcard(wildcardfilter);
			O_PARAM(expr.pattern());
			reglist << QRegularExpression::fromWildcard(wildcardfilter);
		}

		for (QFileInfo fileinfo : dir.entryInfoList(QStringList() << "*.*", QDir::Files, QDir::Name))
		{
			fileinfo.refresh();
			bool exclude = false;
			for (const QRegularExpression& reg : reglist)
			{
				O_PARAM(fileinfo.fileName());

				if (reg.match(fileinfo.fileName()).hasMatch())
				{
					exclude = true;
				}
			}
			if (!exclude)
			{
				O_PARAM(fileinfo.fileName());
				entries << fileinfo.fileName();
			}
		}

	}
	return entries;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  dirs
// Params:  const QString& directory
// Params:  const QString& filter
//----------------------------------------------------------------------------- 

QStringList XDirectory::dirs(const QString& directory, const QString& filter)
{
	return dirs(directory, QStringList() << filter);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  dirs
// Params:  const QString& directory
// Params:  const QStringList& filter
//----------------------------------------------------------------------------- 

QStringList XDirectory::dirs(const QString& directory, const QStringList& filter)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "dirs", "Trace", "Ok", DirectoryJSON(directory));
	}

	QStringList entries;

	QDir dir(directory);
	for (QFileInfo fileinfo : dir.entryInfoList(filter, QDir::NoDotAndDotDot | QDir::AllDirs, QDir::Name))
		entries << fileinfo.fileName();

	return entries;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  files
// Params:  const QString& directory
// Params:  const QString& filter
// Params:  const QDateTime& fromdatetime
// Params:  const QDateTime& todatetime
//----------------------------------------------------------------------------- 
QStringList XDirectory::files(const QString& directory, const QString& filter, const QDateTime& fromdatetime,const QDateTime& todatetime, bool include)
{
	return files(directory, QStringList() << filter, fromdatetime, todatetime,include);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  files
// Params:  const QString& directory
// Params:  const QStringList& filter
// Params:  const QDateTime& fromdatetime
// Params:  const QDateTime& todatetime
//----------------------------------------------------------------------------- 
QStringList XDirectory::files(const QString& directory, const QStringList& filter, const QDateTime& fromdatetime,const QDateTime& todatetime, bool include)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "files Filter", "Trace", "Ok", DirectoryJSON(directory));
	}

	QStringList entries;
	QDir dir(directory);

	if (include)
	{
		for (QFileInfo fileinfo : dir.entryInfoList(filter, QDir::Files, QDir::Time))
		{
			fileinfo.refresh();
			if (fromdatetime <= fileinfo.lastModified() && fileinfo.lastModified() <= todatetime)
				entries << fileinfo.fileName();
		}
	}
	else
	{
		QList<QRegularExpression> reglist;
		for (const QString& wildcardfilter : filter)
		{
			reglist << QRegularExpression::fromWildcard(wildcardfilter);
		}

		for (QFileInfo fileinfo : dir.entryInfoList(QStringList()<<"*.*", QDir::Files, QDir::Time))
		{
			fileinfo.refresh();
			bool exclude = false;
			for(const QRegularExpression&reg:reglist)
			{
				if (reg.match(fileinfo.fileName()).hasMatch())
					exclude=true;
			}
			if (!exclude&&fromdatetime <= fileinfo.lastModified() && fileinfo.lastModified() <= todatetime)
			{
				entries << fileinfo.fileName();
			}
		}
	}
	
	return entries;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  subFiles
// Params:  const QString& directory
// Params:  const QString& filter
// Params:  
//----------------------------------------------------------------------------- 

QStringList XDirectory::tree(const QString& directory, const QString& filter, bool include)
{
	return tree(directory,"", QStringList() << filter,include);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  subFiles
// Params:  const QString& directory
// Params:  const QStringList& filter
// Params:  
//----------------------------------------------------------------------------- 

QStringList XDirectory::tree(const QString& directory, const QStringList& filter, bool include)
{
	return tree(directory, "", filter,include);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  tree
// Params:  const QString& directory
// Params:  const QString& path
// Params:  const QStringList& filter
//----------------------------------------------------------------------------- 

QStringList XDirectory::tree(const QString& directory, const QString& path, const QStringList& filter, bool include)
{
	if (XInject::isTrace("DIRECTORY"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XDirectory", "tree", "Trace", "Ok", DirectoryJSON(directory));
	}

	QStringList entries;

	for (const QString dirname : dirs(directory))
	{
		if (path.isEmpty())
			entries << XDirectory::tree(directory + QDir::separator() + dirname, dirname, filter);
		else
			entries << XDirectory::tree(directory + QDir::separator() + dirname, path + QDir::separator() + dirname, filter);
	}

	for (const QString filename : files(directory, filter,include))
	{
		if (path.isEmpty())
			entries << filename;
		else
			entries << path + QDir::separator() + filename;
	}

	return entries;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    05.11.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  dirsTree
// Params:  const QString& directory
//----------------------------------------------------------------------------- 
QStringList XDirectory::dirsTree(const QString& directory)
{
	return dirsTree(directory, "");
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    05.11.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  dirsTree
// Params:  const QString& directory
// Params:  const QString& path
//----------------------------------------------------------------------------- 
QStringList XDirectory::dirsTree(const QString& directory, const QString& path)
{
	QStringList entries;

	for (const QString dirname : dirs(directory))
	{
		if (path.isEmpty())
		{
			entries << dirname;
			entries << XDirectory::dirsTree(directory + QDir::separator() + dirname, dirname);
		}
		else
		{
			entries << path + QDir::separator() + dirname;
			entries << XDirectory::dirsTree(directory + QDir::separator() + dirname, path + QDir::separator() + dirname);
		}
	}
	
	return entries;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  mkPath
// Params:  const QString& directory
// Params:  
//----------------------------------------------------------------------------- 

bool XDirectory::mkPath(const QString& directory)
{
	return QDir(directory).mkpath(".");
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  wait
//----------------------------------------------------------------------------- 

void XDirectory::wait()
{
	for (auto future : m_Futures)
	{
		future.waitForFinished();
	}
	m_Futures.clear();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  DirectoryJSON
// Params:  const QString& directory
//----------------------------------------------------------------------------- 

QJsonObject XDirectory::DirectoryJSON(const QString& directory)
{
	QJsonObject obj;
	obj["Directory"] = directory;
	return obj;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  DirectoriesJSON
// Params:  const QString& srcdirectory
// Params:  const QString& dstdirectory
//----------------------------------------------------------------------------- 

QJsonObject XDirectory::DirectoriesJSON(const QString& srcdirectory, const QString& dstdirectory)
{
	QJsonObject obj;
	obj["SrcDirectory"] = srcdirectory;
	obj["DstDirectory"] = dstdirectory;
	return obj;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XDirectory
// Method:  append
// Params:  QFuture<void> future
//----------------------------------------------------------------------------- 

void XDirectory::append(QFuture<void> future)
{
	m_Futures.append(future);
}
