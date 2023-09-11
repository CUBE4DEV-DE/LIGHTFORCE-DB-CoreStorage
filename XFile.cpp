#include "XFile.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QtConcurrent>
#include <QTextStream>

#include "XFileBlock.h"
#include "XFileCache.h"
#include "XCrypt.h"
#include "XFileAlias.h"
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

QList<QFuture<void> > XFile::m_Futures;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  alias
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This C++ function, named "alias", is a member of the "XFile" class.It takes a 
// "filename" parameter as a QString reference and simply returns the result of the
// "alias" method from the "XFileAlias" class, which is called with the same 
// "filename" parameter.

QString XFile::alias(const QString& filename)
{
	return XFileAlias::alias(filename);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  alias
// Params:  const QString& filename
// Params:  const QString& alias
//----------------------------------------------------------------------------- 

// This "alias" method is part of the "XFile" class and takes two QString references 
// as parameters : "filename" and "alias".Initially,  it checks if both strings are empty,
// and if so, invokes the "clear" method from the "XFileAlias" class,
// potentially clearing all aliases.If only "filename" is empty, it again calls the "clear" method,
// this time with the "alias" parameter, possibly to remove a specific alias.Finally,
// if both strings are not empty, it calls the "insert" method of the "XFileAlias" 
// class to create or update an alias with the provided "alias" and "filename".

void XFile::alias(const QString& filename, const QString& alias)
{

	if (filename.isEmpty()&&alias.isEmpty())
		XFileAlias::clear();
	else if (filename.isEmpty())
		XFileAlias::clear(alias);

	if (!filename.isEmpty() && !alias.isEmpty())
		XFileAlias::insert(alias,filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readFile
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This "read" function, part of the "XFile" class, takes a filename as an input
// parameter and attempts to read the content of the file with that name. Initially,
// it checks if a trace for "FILE" is active using "XInject::isTrace", logging 
// a trace message if true. Then, it tries to open and read the specified file,
// logging a warning and returning an empty QString if the file cannot be opened
// or if any other error occurs during the read operation. If the file is successfully opened
// , it reads the entire content using a QTextStream and returns it as a QString.
// Throughout the function, it uses an "alias" method to possibly resolve the filename 
// to an alias before accessing the file, and "FileNameJSON" method to 
// format the filename for logging.

QString XFile::read(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE","CoreStorage", "XFile", "read", "Trace", "Ok", FileNameJSON(filename));
	}


	XFileBlock block(alias(filename));

	try
	{
		QFile file(alias(filename));

		if (!file.open(QIODevice::ReadOnly))
		{
			X_WARNINGJSON("CoreStorage", "XFile", "read", "Open", "Failed", FileNameJSON(filename));
			return QString();
		}

		QTextStream fileread(&file);
		fileread.setAutoDetectUnicode(true);
		return fileread.readAll();
	}
	catch(...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "read", "Read", "Failed",FileNameJSON(filename));
		return QString();
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This function, named "readCache", is part of the "XFile" class and takes a
// "filename" parameter as a QString reference. Initially, it checks if a trace
// for "FILE" is active using "XInject::isTrace", and if true, logs a trace message.
// Then, it calls the "read" method from the "XFileCache" class, passing an alias
// of the "filename" (resolved through the "alias" method of the "XFile" class) 
// as the parameter, and returns its result.

const QString& XFile::readCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return XFileCache::read(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeFile
// Params:  const QString& filename
// Params:  const QString& content
// Params:  
//----------------------------------------------------------------------------- 

// This source code defines a method `XFile::write` in C++ that takes three
// parameters: a filename (as a QString), content (as a QString), and a 
// boolean flag named "run". The method first checks if a trace for "FILE" 
// is active using the `XInject::isTrace` method and logs a message if it is. 
// Next, based on the value of the "run" parameter, it writes the content to 
// the specified file in two different ways : 

// 1. If "run" is true, it starts a concurrent operation(potentially running 
// in a separate thread) to write the content to the file.It tries to create 
// necessary paths and open the file for writing, logging a warning message 
// if any of these operations fail.It writes the content to the file using 
// a QTextStream, which is set to automatically detect Unicode encoding.

// 2. If "run" is false, it performs a similar series of operations but within
//  the main thread, returning true if the write operation was successful and 
// false otherwise, logging a warning message if the operation fails at any step.

bool XFile::write(const QString& filename, const QString& content, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "write", "Trace", "Ok", FileNameJSON(filename));
	}

	if (run)
	{
		concurrent([filename,content]
			{
				XFileBlock block(alias(filename),true);

				try {

					mkFilePath(filename);

					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly))
					{
						X_WARNINGJSON("CoreStorage", "XFile", "write", "Open", "Failed", FileNameJSON(filename));
						return;
					}
					{
						QTextStream filewrite(&file);
						filewrite.setAutoDetectUnicode(true);
						filewrite << content;
					}
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "write", "Write", "Failed", FileNameJSON(filename));
				}
			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);
		
		try
		{
			mkFilePath(filename);

			QFile file(alias(filename));
			if (!file.open(QIODevice::WriteOnly))
			{
				X_WARNINGJSON("CoreStorage", "XFile", "write", "Open", "Failed", FileNameJSON(filename));
				return false;
			}

			{
				QTextStream filewrite(&file);
				filewrite.setAutoDetectUnicode(true);
				filewrite << content;
			}

			return true;
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "write", "Write", "Failed", FileNameJSON(filename));
			return false;
		}
	}

}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  append
// Params:  const QString& filename
// Params:  const QString& content
// Params:  bool run
//----------------------------------------------------------------------------- 

// This script defines a method named `append` within the `XFile` class that
// takes four arguments: a filename (as a QString), content (as a QString), 
// a boolean named "run", and a boolean named "ignorelog".
// 
// 1. Initially, it checks whether a trace is active for "FILE" using 
// `XInject::isTrace("FILE")` and if "ignorelog" is false.
// If so, it logs a trace message using `X_LOGJSON`.
// 
// 2. Then, the script checks the "run" argument.If "run" is true,
// it executes a concurrent block where it attempts to append the provided
// "content" to the file specified by "filename" in a separate thread.
// During this process, it creates an `XFileBlock` and opens the file in append mode,
// then writes the "content" to it using a `QTextStream`. If it encounters any 
// errors(either in opening the file or during the write operation), and "ignorelog" 
// is false, it logs a warning message using `X_WARNINGJSON`. The concurrent
// operation doesn't return any value.
// 
// 3. If "run" is false, the script executes a similar block of code but not
// in a separate thread.It tries to append "content" to the file and logs
// warning messages if necessary(same as in the concurrent block).It returns `true`
// if the operation succeeds and `false` otherwise.

// Throughout the code, a series of operations(such as aliasing the filename 
// and creating the file path) are performed to manage the file before the 
// writing operation.

bool XFile::append(const QString& filename, const QString& content, bool run, bool ignorelog)
{
	if (XInject::isTrace("FILE")&&!ignorelog)
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "append", "Trace", "Ok", FileNameJSON(filename));
	}

	if (run)
	{
		concurrent([filename, content,ignorelog]
			{
				XFileBlock block(alias(filename), true);

				try {

					mkFilePath(filename);

					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
					{
						if (!ignorelog)
						{
							X_WARNINGJSON("CoreStorage", "XFile", "append", "Open", "Failed", FileNameJSON(filename));
						}
						return;
					}

					{
						QTextStream filewrite(&file);
						filewrite.setAutoDetectUnicode(true);
						filewrite << content;
					}
				}
				catch (...)
				{
					if (!ignorelog)
					{
						X_WARNINGJSON("CoreStorage", "XFile", "append", "Write", "Failed", FileNameJSON(filename));
					}
				}
			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);

		try {

			mkFilePath(filename);

			QFile file(alias(filename));
			if (!file.open(QIODevice::WriteOnly| QIODevice::Append))
			{
				if (!ignorelog)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "append", "Open", "Failed", FileNameJSON(filename));
				}
				return false;
			}


			{
				QTextStream filewrite(&file);
				filewrite.setAutoDetectUnicode(true);
				filewrite << content;
			}

			return true;

		}
		catch (...)
		{
			if (!ignorelog)
			{
				X_WARNINGJSON("CoreStorage", "XFile", "append", "Write", "Failed", FileNameJSON(filename));
			}
			return false;
		}

	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readBinary
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

//This source code defines a method named `readBinary` in the `XFile` class
//  that reads binary data from a file specified by the "filename" parameter
//  and returns it as a `QByteArray`. 

// Here is a brief summary in English :

// 1. A log trace is created if "FILE" tracing is enabled, logging the filename in JSON format.
//
// 2. It tries to open the specified file in read - only mode.
// - If the file cannot be opened, it logs a warning with details including the
//  filename in JSON format and returns an empty `QByteArray`.
// - If the file is successfully opened, it reads all contents and returns it as
//  a `QByteArray`.
// 
// 3. If any exception occurs during the reading process, it logs a warning with
//  details including the filename in JSON format and returns an empty `QByteArray`.

// Error handling is implemented using JSON-formatted logging and try - catch blocks.
// It makes use of utility functions such as `alias` and `FileNameJSON` 
// for file handling and logging purposes respectively.

QByteArray XFile::readBinary(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readBinary", "Trace", "Ok", FileNameJSON(filename));
	}

	XFileBlock block(alias(filename));

	try
	{
		QFile file(alias(filename));
		if (!file.open(QIODevice::ReadOnly))
		{
			X_WARNINGJSON("CoreStorage", "XFile", "readBinary", "Open", "Failed",FileNameJSON(filename));
			return QByteArray();
		}

		return file.readAll();
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "readBinary", "Read", "Failed", FileNameJSON(filename));
	}

	return QByteArray();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readBinaryCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This source code defines a method `readBinaryCache` in the `XFile` class
// that takes a filename (as a `QString`) as input and returns a reference
// to a `QByteArray`. Inside the method, it checks if tracing is enabled for 
// the "FILE" category using the `XInject::isTrace` method. If tracing is enabled,
// it logs a trace message with various details, including the filename, 
// using the `X_LOGJSON` macro. Finally, it reads the binary data associated
// with the alias of the input filename from `XFileCache` and returns it.

const QByteArray& XFile::readBinaryCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readBinaryCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return XFileCache::readBinary(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeBinary
// Params:  const QString& filename
// Params:  const QByteArray& data
//----------------------------------------------------------------------------- 

// This source code defines a method `writeBinary` in the `XFile` class which 
// writes binary data to a specified file either concurrently or in a single
//  thread based on the value of the `run` parameter. 

// 1. Initially, it logs a trace message if the "FILE" trace flag is active
//  in the `XInject` class.
// 
// 2. If the `run` parameter is true, it attempts to write the data concurrently 
// using a lambda function passed to the `concurrent` function.It creates a file
//  with the given filename, opens it in write - only mode, and writes the data to it.
// 
// If any step fails, appropriate warnings are logged.This branch returns true,
//  irrespective of whether the write succeeded or not.
// 
// 3. If the `run` parameter is false, it performs the write operation in a single
//  thread with similar steps to the concurrent branch, but this time returning false
//  if the operation fails at any step, and true only if the write succeeds successfully.

// Throughout the method, it uses functionalities like logging(`X_LOGJSON`, `X_WARNINGJSON`)
//  and filename aliasing(`alias`) and creates necessary file paths(`mkFilePath`) 
// using helper methods and classes(`XFileBlock`, `XInject`) which are presumably defined
//  elsewhere in the code base.

bool XFile::writeBinary(const QString& filename, const QByteArray& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeBinary", "Trace", "Ok", FileNameJSON(filename));
	}

	if(run)
	{
		concurrent([filename, data]
			{
				XFileBlock block(alias(filename), true);

				try
				{
					mkFilePath(filename);
					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly))
					{
						X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Open", "Failed", FileNameJSON(filename));
						return;
					}
					file.write(data);
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Write", "Failed", FileNameJSON(filename));
				}

			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);

		try
		{
			mkFilePath(filename);

			QFile file(alias(filename));
			if (!file.open(QIODevice::WriteOnly))
			{
				X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Open", "Failed", FileNameJSON(filename));
				return false;
			}

			file.write(data);

			return true;
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Write", "Failed", FileNameJSON(filename));
			return false;
		}

	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  appendBinary
// Params:  const QString& filename
// Params:  const QByteArray& data
// Params:  bool run
//----------------------------------------------------------------------------- 

// This script defines a function `appendBinary` in the `XFile` class that
// appends binary data to a file. The function accepts the following parameters:
// `filename` (the name of the file to append data to), `data` 
// (the binary data to append), `run` (a flag indicating whether to perform
// the operation in a concurrent manner), and `ignorelog` (a flag that, if 
// set to true, prevents logging of warnings).

// Here is the summary of each section of the code :

// 1. If tracing is enabled for "FILE" through `XInject: : isTrace`, it logs
// a trace message with various details including the filename.
// 2. If the `run` parameter is true, it initiates a concurrent operation
// which does the following :
// -It creates an `XFileBlock` instance with the alias of the filename and 
// a flag set to true.
// - It tries to create the path to the file using `mkFilePath`.
// - It then tries to open the file in append mode and writes the data to it.
// If any operation fails and `ignorelog` is false, it logs a warning message.
// 3. If the `run` parameter is false, it performs the same operations as in 
// the concurrent block but in a synchronous manner.

// Note that the function returns `true` if the operations are initiated 
// successfully(in concurrent mode) or completed successfully
// (in synchronous mode), and `false` otherwise.


bool XFile::appendBinary(const QString& filename, const QByteArray& data, bool run, bool ignorelog)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "appendBinary", "Trace", "Ok", FileNameJSON(filename));
	}

	if (run)
	{
		concurrent([filename, data,ignorelog]
			{
				XFileBlock block(alias(filename), true);

				try
				{

					mkFilePath(filename);

					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
					{
						if (!ignorelog)
						{
							X_WARNINGJSON("CoreStorage", "XFile", "appendBinary", "Open", "Failed", FileNameJSON(filename));
						}
						return;
					}
					file.write(data);
				}
				catch (...)
				{
					if (!ignorelog)
					{
						X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Write", "Failed", FileNameJSON(filename));
					}
				}
			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);
		try
		{
			mkFilePath(filename);

			QFile file(alias(filename));

			if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
			{
				if (!ignorelog)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "appendBinary", "Open", "Failed", FileNameJSON(filename));
				}
				return false;
			}

			file.write(data);
			return true;

		}
		catch (...)
		{
			if (!ignorelog)
			{
				X_WARNINGJSON("CoreStorage", "XFile", "appendBinary", "Write", "Failed", FileNameJSON(filename));
			}
			return false;
		}

	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocument
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

// The function `readDocument` is a method in the `XFile` class that takes a
//  filename (as a QString type) as an argument and returns a `QJsonDocument`.
//  Inside the function, it first checks if tracing for "FILE" is enabled using
//  `XInject::isTrace("FILE")`. If tracing is enabled, it logs some trace
//  information including the filename in JSON format using a custom log 
// function `X_LOGJSON`. Finally, it reads the binary content of the specified
//  file using `readBinary(filename)` and converts it to a `QJsonDocument` 
// object using `QJsonDocument::fromJson` before returning it.

QJsonDocument XFile::readDocument(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocument", "Trace", "Ok", FileNameJSON(filename));
	}

	return QJsonDocument::fromJson(readBinary(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocumentCache
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

// This source code defines a method named "readDocumentCache" within the XFile
//  class, which takes a QString reference named "filename" as a parameter and 
// returns a reference to a QJsonDocument.

// Inside the method :

// 1. It first checks if "FILE" is a trace within XInject class using the
//  "isTrace" method.
// 
// 2. If the trace condition is true, it logs a message with various parameters 
// including the "filename" (processed by FileNameJSON function) through the
//  X_LOGJSON macro.
// 
// 3. It then returns the result of calling the "read" method from the XFileCache 
// class, passing an alias of the "filename" as a parameter.

const QJsonDocument& XFile::readDocumentCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocumentCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return XFileCache::readDocument(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeDocument
// Params:  const QString& filename
// Params:  const QJsonDocument& data
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

// This source code defines a method called "writeDocument" within the "XFile"
//  class that accepts three parameters: a filename (as a QString), 
// data (as a QJsonDocument), and a boolean called "run". Inside the method,
//  it first checks if the trace for "FILE" is active using the "XInject::isTrace"
//  method. If it is active, it logs some information using "X_LOGJSON".
//  Afterwards, it calls another method named "writeBinary", passing the filename
// , the JSON representation of the data, and the "run" boolean as arguments
// , and returns the result of this "writeBinary" method call.

bool XFile::writeDocument(const QString& filename, const QJsonDocument& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeDocument", "Trace", "Ok", FileNameJSON(filename));
	}

	return writeBinary(filename,data.toJson(),run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocumentMap
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This source code defines a method in the XFile class called `readDocumentMap`,
//  which takes a filename (as a QString) as an argument and returns a nested
//  QMap containing QString keys and QVariant values. Here's a brief summary
//  of its functionality in English:

// 1. It first checks if the "FILE" trace is active in the `XInject` class;
//  if true, it logs a JSON message with the details of the file being accessed.
//
// 2. It reads a JSON document from the specified filename using another
//  method in the XFile class : `readDocument`.
// 
// 3. It initializes an empty nested QMap to store the results.
// 
// 4. If the read document is empty, it immediately returns the empty map.
// 
// 5. If the document contains a JSON object, it iterates through all the 
// keys in the root JSON object.
// 
// 6. For each key that maps to a JSON object, it iterates through the
//  child keys and maps them, along with their respective
//  values(converted to QVariant), into the nested QMap.
// 
// 7. The populated map is then returned.


QMap<QString, QMap<QString, QVariant>> XFile::readDocumentMap(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocumentMap", "Trace", "Ok", FileNameJSON(filename));
	}

	//O_STACK(XFile, readDocumentMap);

	const QJsonDocument& doc = XFile::readDocument(filename);
	QMap<QString, QMap<QString, QVariant>> map;

	if (doc.isEmpty())
		return map;

	if (doc.isObject())
	{
		const QJsonObject& rootobject = doc.object();
		for (const QString& key : rootobject.keys())
		{
			//O_PARAM(key);
			if (rootobject[key].isObject())
			{
				QJsonObject jsonobj = rootobject[key].toObject();
				for (const QString& childkey : jsonobj.keys())
				{
					map[key][childkey] = jsonobj.value(childkey).toVariant();
					//O_PARAM(childkey);
					//O_PARAM(map[key][childkey].toString());
				}
			}
		}
	}
	return map;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocumentMapCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This code defines a function `readDocumentMapCache` in the `XFile` class 
// that takes a filename as input and returns a nested QMap structure.
//  Initially, it performs a trace log if the "FILE" tracing is enabled.
//  Then, it reads a JSON document from the cache using the given filename. 

// The function proceeds to parse the JSON document.If the document is empty,
//  it returns an empty map.If the document is a JSON object, it iterates 
// over its keys and, for each key pointing to a JSON object, it iterates 
// over its keys to build a nested map with the respective values.
// This nested map is then returned.

QMap<QString, QMap<QString, QVariant>> XFile::readDocumentMapCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocumentMapCache", "Trace", "Ok", FileNameJSON(filename));
	}

	const QJsonDocument& doc = XFile::readDocumentCache(filename);
	QMap<QString,QMap<QString,QVariant>> map;

	if (doc.isEmpty())
		return map;

	if (doc.isObject())
	{
		const QJsonObject& rootobject=doc.object();
		for (const QString& key:rootobject.keys())
		{
			if (rootobject[key].isObject())
			{
				QJsonObject jsonobj = rootobject[key].toObject();
				for (const QString& childkey : jsonobj.keys())
				{
					map[key][childkey]=jsonobj.value(childkey).toVariant();
				}
			}
		}
	}
	return map;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeDocumentMapMap
// Params:  const QString& filename
// Params:  const QMap<QString, QMap<QString, QVariant>>& data
// Params:  bool run
//----------------------------------------------------------------------------- 

// This source code defines a method in the `XFile` class named
// `writeDocumentMap` which takes three parameters: a filename (as a QString),
// a nested QMap structure containing data, and a boolean named "run". 
// Inside the method, it first checks if the trace for "FILE" is active 
// using `XInject::isTrace("FILE")`, and if so, logs some information 
// using `X_LOGJSON`.

// Afterwards, it creates a QJsonObject named `rootobj` and iterates over 
// the keys of the input data map.Inside this loop, it creates another
// QJsonObject named `jsonobj` and iterates over the keys of the inner
// map(associated with the current key in the outer loop), inserting 
// each key - value pair from the inner map into `jsonobj` (converting
// the value to a JSON value in the process).After processing all keys
// in the inner map, it inserts the `jsonobj` into `rootobj` with the 
// current key from the outer loop as the key.

// Finally, it creates a QJsonDocument with `rootobj` as the content,
// and calls another method named `writeDocument` with the filename, 
// the created QJsonDocument, and the "run" parameter, and 
// returns the result of that method call.

bool XFile::writeDocumentMap(const QString& filename, const QMap<QString, QMap<QString, QVariant>>& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeDocumentMap", "Trace", "Ok", FileNameJSON(filename));
	}

	//O_STACK(XFile,writeDocumentMap)

	QJsonObject rootobj;
	for (const QString& key : data.keys())
	{
		//O_PARAM(key);
		QJsonObject jsonobj;
		for (const QString& childkey:data[key].keys())
		{
			//O_PARAM(childkey);
			//O_PARAM(data[key][childkey].toString());
			jsonobj.insert(childkey, data[key][childkey].toJsonValue());
		}
		rootobj.insert(key, jsonobj);
	}

	QJsonDocument doc(rootobj);
	return writeDocument(filename, doc, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readList
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

// This code defines a function `readList` in the `XFile` class that takes a 
// filename as a parameter (of type `QString`) and returns a QStringList.
// Inside the function, it first checks if tracing for "FILE" is enabled
// using the `XInject::isTrace` method. If tracing is enabled, it logs a 
// trace message using the `X_LOGJSON` macro, which logs the event with
// various details including a JSON representation of the filename. 
// Finally, it calls another function `read` with the `QStringList` as 
// the template argument and the filename as the parameter, and returns the result.

QStringList XFile::readList(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readList", "Trace", "Ok", FileNameJSON(filename));
	}

	return read<QStringList>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readListCache
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 
QStringList XFile::readListCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readListCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return readCache<QStringList>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeList
// Params:  const QString& filename
// Params:  const QStringList& data
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XFile::writeList(const QString& filename, const QStringList& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeList", "Trace", "Ok", FileNameJSON(filename));
	}

	return write<QStringList>(filename, data, run);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readMap
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

QMap<QString,QString> XFile::readMap(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readMap", "Trace", "Ok", FileNameJSON(filename));
	}

	return read<QMap<QString, QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readMapCache
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

QMap<QString,QString> XFile::readMapCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readMapCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return readCache<QMap<QString, QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeMap
// Params:  const QString& filename
// Params:  const QMap<QString, QString>& data
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XFile::writeMap(const QString& filename, const QMap<QString, QString>& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeMap", "Trace", "Ok", FileNameJSON(filename));
	}

	return write<QMap<QString, QString>>(filename, data, run);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readSet
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QSet<QString> XFile::readSet(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readSet", "Trace", "Ok", FileNameJSON(filename));
	}

	return read<QSet<QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readSetCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QSet<QString> XFile::readSetCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readSetCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return readCache<QSet<QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeSet
// Params:  const QString& filename
// Params:  const QSet<QString>& data
// Params:  bool run
//----------------------------------------------------------------------------- 

// This source code defines a method named "writeSet" in the "XFile" class that 
// takes three parameters: a filename (as a QString reference), a set of
// QString data, and a boolean named "run". Inside the method, it first 
// checks if tracing is enabled for "FILE" using the "isTrace" method from 
// the "XInject" class. If tracing is enabled, it logs a JSON formatted message
// with various details including the filename. Finally, the method calls 
// another write method with the specified filename, data, and run parameters
// and returns its result.

bool XFile::writeSet(const QString& filename, const QSet<QString>& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeSet", "Trace", "Ok", FileNameJSON(filename));
	}

	return write<QSet<QString>>(filename, data, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  copyFile
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
//----------------------------------------------------------------------------- 

// This source code defines a `copy` method in the `XFile` class that takes 
// three parameters: `srcfilename` (source file name), `dstfilename` 
// (destination file name), and `run` (a boolean indicating whether to
//  perform the copy operation concurrently).

// 1. Initially, it checks if the `FILE` trace is active using 
// `XInject::isTrace("FILE")`, and if true, logs a JSON - formatted 
// message using `X_LOGJSON`.

// 2. Then it performs several validations :
// -If `srcfilename` is empty, it returns false.
// - It gathers information about the source and destination 
// files(`srcinfo` and `dstinfo`) and refreshes this info.If the files have
//  the same last modified date, birth time, and size, it logs a warning and 
// returns false indicating that the copy was skipped.
// 	- If the source file does not exist, it logs another warning and returns 
// false.

// 	3. If the `run` parameter is true, it performs the copy operation 
// concurrently within a lambda function passed to the `concurrent` function.
// If an exception occurs during the copy, it logs a warning.

// 	4. If the `run` parameter is false, it performs the copy operation 
// synchronously, also logging a warning in case of a failure.


bool XFile::copy(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "copy", "Trace", "Ok", FileNamesJSON(srcfilename,dstfilename));
	}

	//O_STACK(XFile, copy);
	//O_PARAM(srcfilename);
	//O_PARAM(dstfilename);

	if (srcfilename.isEmpty())
		return false;

	QFileInfo srcinfo(alias(srcfilename));
	srcinfo.refresh();
	QFileInfo dstinfo(alias(dstfilename));
	dstinfo.refresh();

	if (srcinfo.lastModified() == dstinfo.lastModified() &&
		srcinfo.birthTime() == dstinfo.birthTime() &&
		srcinfo.size() == dstinfo.size())
	{
		X_WARNINGJSON("CoreStorage", "XFile", "copy", "Equal", "Skipped", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "copy", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{

		concurrent([srcfilename, dstfilename]
			{
				XFileBlock srcblock(alias(srcfilename));
				XFileBlock dstblock(alias(dstfilename),true);

				try {
					mkFilePath(dstfilename);
					QFile::copy(alias(srcfilename), alias(dstfilename));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "copy", "Copy","Failed",FileNamesJSON(srcfilename,dstfilename));
				}

			});

		return true;
	}
	else
	{
		XFileBlock srcblock(alias(srcfilename));
		XFileBlock dstblock(alias(dstfilename),true);

		try {
			mkFilePath(dstfilename);
			return QFile::copy(alias(srcfilename), alias(dstfilename));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "copy", "Copy", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    12.01.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  rename
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

// The given source code defines a method named `rename` in the `XFile` 
// class which takes three arguments: `srcfilename` (source filename), 
// `dstfilename` (destination filename), and `run` (although `run` is 
// not used in the function). The method returns a boolean value indicating
//  the success or failure of the rename operation.

// 1. Initially, it checks if the "FILE" trace is enabled using
// `XInject::isTrace("FILE")`. If enabled, it logs a trace message using `X_LOGJSON`.

// 2. It then creates two instances of `XFileBlock`, `srcblock` and `dstblock`,
// representing the source and destination file blocks respectively, with the
// alias of the respective filenames.

// 3. Before attempting to rename the file, it checks if the source file 
// exists using the `exists` method.If the source file does not exist, it
// logs a warning message and returns false.

// 4. It then attempts to rename the file using `QFile::rename`. If the
// renaming succeeds, it returns true, otherwise, it catches any exception,
// logs a warning message, and returns false.

bool XFile::rename(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "rename", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	XFileBlock srcblock(alias(srcfilename));
	XFileBlock dstblock(alias(dstfilename), true);

	if (!exists(alias(srcfilename)))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "rename", "Source", "Missing",FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	try {
		return QFile::rename(alias(srcfilename), alias(dstfilename));
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "rename", "Rename", "Failed", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    12.01.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  move
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

// The source code defines a method named `move` in the `XFile` class that
//  attempts to move a file from a source path (`srcfilename`) to a destination
//  path (`dstfilename`). Here's a brief summary of each segment of the code:

// 1. It starts with checking whether tracing for "FILE" is enabled using
//  `XInject::isTrace("FILE")`. If enabled, it logs a JSON message with the
//  details of the operation.

// 2. If the source filename is empty, it immediately returns false,
//  indicating failure.

// 3. It then gathers information about both the source and destination
//  files using `QFileInfo` objects, and refreshes this information.

// 4. It checks if the last modified time, birth time, and size of both
//  files are identical.If they are, it returns false to avoid unnecessary
//  file operations.

// 5. It checks whether the source file exists using the `exists` method.
// If not, it logs a warning message and returns false.

// 6. Depending on the value of the `run` parameter, it proceeds with the
//  file move operation either concurrently or sequentially.In both cases,
//  it performs the following operations :
// -Acquires file blocks for both the source and destination files.
// - Attempts to create the destination file path using `mkFilePath`.
// - Copies the source file to the destination path using `QFile::copy`.
// - Removes the original source file using `QFile::remove`.

// 7. If an exception occurs during the file move operations, it logs
//  a warning message and returns false to indicate failure.Otherwise,
//  it returns true to indicate success.

// It's important to note that the code uses lambdas and concurrency to
//  potentially perform the move operation in a separate thread when the 
// `run` parameter is true. This is intended to prevent the move operation
//  from blocking the main thread.

bool XFile::move(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "move", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (srcfilename.isEmpty())
		return false;

	QFileInfo srcinfo(alias(srcfilename));
	srcinfo.refresh();
	QFileInfo dstinfo(alias(dstfilename));
	dstinfo.refresh();

	if (srcinfo.lastModified() == dstinfo.lastModified() &&
		srcinfo.birthTime() == dstinfo.birthTime() &&
		srcinfo.size() == dstinfo.size())
		return false;

	if (!exists(alias(srcfilename)))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "move", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{
		
		concurrent([srcfilename, dstfilename]
			{
				XFileBlock srcblock(alias(srcfilename));
				XFileBlock dstblock(alias(dstfilename), true);

				try {
					mkFilePath(dstfilename);
					QFile::copy(alias(srcfilename), alias(dstfilename));
					QFile::remove(alias(srcfilename));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "move", "Moving", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}
	else
	{
		XFileBlock srcblock(alias(srcfilename));
		XFileBlock dstblock(alias(dstfilename), true);

		try
		{
			mkFilePath(dstfilename);

			QFile::copy(alias(srcfilename), alias(dstfilename));
			QFile::remove(alias(srcfilename));
			return true;
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "move", "Moving", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  removeFile
// Params:  const QString& filename
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

// This C++ function `XFile::remove` is a member of the `XFile` class and 
// takes two parameters: a QString reference `filename` and a boolean `run`.
//  Initially, it checks if a trace flag for "FILE" is active and if so,
//  logs a trace message. It then verifies if the file, determined by 
// aliasing the filename, exists; if not, it logs a warning message and 
// returns false.

// Depending on the value of the `run` parameter, it proceeds to delete 
// the file either concurrently or not:

// 1. If `run` is true:
// -A concurrent task is initiated to remove the file.
// - Inside the concurrent task, it creates a `XFileBlock` instance
//  and attempts to remove the file(both alias and actual file).
// - If any exception occurs during the removal, it logs a warning
//  message.
// - Regardless of whether the removal was successful or not, the function
//  will return true.

// 2. If `run` is false:
// -Similar to when `run` is true, but the removal is done 
// synchronously(not in a separate task).
// - If the removal succeeds, it returns the result of `QFile::remove`.
// - If an exception occurs, it logs a warning message and returns false.

// Notably, there are a couple of commented lines at the beginning
//  of the function, potentially for stack tracing and parameter
//  logging which are currently disabled.

bool XFile::remove(const QString& filename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "remove", "Trace", "Ok", FileNameJSON(filename));
	}

	//O_STACK(XFile, remove);
	//O_PARAM(filename);
	if (!exists(alias(filename)))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "remove", "Filename", "Missing", FileNameJSON(filename));
		return false;
	}

	if (run)
	{

		concurrent([filename]
			{
				XFileBlock srcblock(alias(filename),true);

				try 
				{
					XFileAlias::remove(alias(filename));
					QFile::remove(alias(filename));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "remove", "Remove", "Failed", FileNameJSON(filename));
				}
			});

		return true;
	}
	else
	{
		XFileBlock srcblock(alias(filename),true);

		try {
			XFileAlias::remove(alias(filename));
			return QFile::remove(alias(filename));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "remove", "Remove", "Failed", FileNameJSON(filename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  remove
// Params:  const QStringList& filenames
// Params:  bool run
//----------------------------------------------------------------------------- 

// The given code defines a method in the `XFile` class that tries to remove 
// a list of files specified in the "filenames" parameter.It iterates over 
// each filename in the list and tries to remove it using the `XFile: :remove` method.
// If any of the removal operations fail(returns false), it updates the `ret` 
// variable to false.At the end of the method, it returns the `ret` variable.
// If all files are removed successfully, it returns true, otherwise it returns false.
// The "run" parameter is passed to the `XFile::remove` method for each file.

bool XFile::remove(const QStringList& filenames, bool run)
{
	bool ret = true;
	for (const QString& filename : filenames)
		if (!XFile::remove(filename, run))
			ret = false;
	return ret;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    13.01.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  unique
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This source code defines a function named "unique" in the "XFile" 
// class that takes a QString reference named "filename" as a
//  parameter and returns a unique filename as a QString.

// 1. Initially, it checks if a file with the given filename does not
// exist; if true, it immediately returns the input filename.
// 
// 2. It extracts information about the file using QFileInfo.
// 
// 3. It extracts the base name(the name without the extension) of the 
// file and stores it in "renamefilename".
// 
// 4. It checks if "renamefilename" contains a '_' and if found, trims 
// the name up to the position before the last underscore.
// 
// 5. It initializes a "basefilename" variable to store the modified
// base name and a "number" variable with the value 1.
// 
// 6. Then, in a loop, it constructs new filenames by appending a 
// number(formatted as a 4 - digit string) to "basefilename" until
// it finds a filename that does not exist in the specified path.
// 
// 7. Once a unique filename is found, it constructs the full path with the
// unique base name and original file extension, and returns it.

// The function aims to avoid file name collisions by creating a unique
// filename through appending a number at the end of the base name.

QString XFile::unique(const QString& filename)
{

	if (!QFile::exists(filename))
		return filename;

	QFileInfo fileinfo(filename);

	QString renamefilename(fileinfo.baseName());

	qint32 pos = renamefilename.lastIndexOf('_');
	if (pos >= 0)
		renamefilename = renamefilename.mid(0, pos-1);

	QString basefilename= renamefilename;

	qint32 number=1;
	while (QFile::exists(fileinfo.path()+QDir::separator()+ renamefilename+"."+fileinfo.suffix()))
	{
		renamefilename = basefilename + QString("_%1").arg(number++, 4, 10, QChar('0'));
	}

	return fileinfo.path() + QDir::separator() + renamefilename + "." + fileinfo.suffix();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  exists
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

// This source code defines a method named `exists` in the `XFile` class.
// It takes a constant QString reference as a parameter named `filename`.
//  Inside the method, it calls another method named `alias`
//  with `filename` as an argument, and then checks if a file with the 
// resultant alias exists using `QFile::exists` method.It returns `true`
//  if the file exists and `false` otherwise.

bool XFile::exists(const QString& filename)
{
	return QFile::exists(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  equal
// Params:  const QString& filename
// Params:  const QString& filename2
// Params:  
//----------------------------------------------------------------------------- 

// This source code defines a method called "equal" in the "XFile" class that 
// takes two filename strings as inputs and returns a boolean indicating 
// whether the files are equal or not.Here is a brief summary of the code:

// 1. It obtains file information(like size, last modified time, and 
// creation time) for each file using their filenames.
// 2. If the sizes of the two files are not equal, it returns `false`.
// 3. If the sizes are equal, it then checks whether both the last modified
//  time and the creation time are equal.If both are equal, it returns `true`.
// 4. If the last modified and creation times are not equal, it then reads
//  the binary data of both files and compares them.If the binary data is
//  equal, it returns `true`; otherwise, it returns `false`.

// This method, therefore, uses a multi-stage comparison process to check
// whether the two files are equal, considering both file metadata and content.

bool XFile::equal(const QString& filename, const QString& filename2)
{
	QFileInfo fileinfo(alias(filename));
	fileinfo.refresh();
	QFileInfo fileinfo2(alias(filename2));
	fileinfo2.refresh();

	if (fileinfo.size() != fileinfo2.size())
		return false;

	if (fileinfo.lastModified() == fileinfo2.lastModified() &&
		fileinfo.birthTime() == fileinfo2.birthTime())
		return true;

	return XFile::readBinary(filename)==XFile::readBinary(filename2);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  contains
// Params:  const QString& filename
// Params:  const QString& search
// Params:  Qt::CaseSensitivity sensitivity
//----------------------------------------------------------------------------- 

// The given source code defines a function `contains` in the `XFile` class 
// that takes three parameters : `filename` (a string indicating the file name),
// `search` (a string representing the text to search for), and `sensitivity`
// (a parameter that specifies the case sensitivity of the search operation).
// It returns a boolean value indicating whether the text specified in the 
// `search` parameter is found within the file specified by the `filename`
// parameter.The search operation considers the case sensitivity parameter 
// to conduct the search operation accordingly.The `contains` function 
// leverages the `read` function from the same class (`XFile`) to read 
// the file content before performing the search operation.

bool XFile::contains(const QString& filename, const QString& search, Qt::CaseSensitivity sensitivity)
{
	return XFile::read(filename).contains(search, sensitivity);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  contains
// Params:  const QString& filename
// Params:  const QRegularExpression& search
//----------------------------------------------------------------------------- 

// This C++ method, a member of the XFile class, takes two parameters: 
// a filename (of type QString) and a regular expression (of type QRegularExpression).
// It reads the content of the file specified by the filename and returns `true`
// if the content contains a match for the regular expression pattern,
// and `false` otherwise.

bool XFile::contains(const QString& filename, const QRegularExpression& search)
{
	return XFile::read(filename).contains(search);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  compressFile
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
//----------------------------------------------------------------------------- 

// This C++ code defines a method in the `XFile` class named `compress` which 
// takes three parameters : `srcfilename` (source file name), `dstfilename`
// (destination file name), and a boolean `run`.

// - Initially, it checks if the trace flag for "FILE" is active using 
// `XInject::isTrace` and logs a trace message if true.
// 
// - It then checks if the source file exists using the `exists` method.
// If the source file doesn't exist, it logs a warning message and returns false.
// 
// - If the `run` parameter is true, it attempts to compress the file concurrently,
// where it reads the binary data from the source file, compresses it using
// `qCompress`, and writes the compressed data to the destination file,
// all in a separate thread.If any error occurs during this operation,
// it logs a warning message.
// 
// - If the `run` parameter is false, it performs the same compressing operation
// but not concurrently(i.e., in the same thread).It logs a warning message 
// if an error occurs during this operation.
// 
// - The method returns true if the compression was initiated successfully(in case of concurrent operation)
// or completed successfully(in case of non - concurrent operation), and false otherwise.

// Note: The `writeBinary`, `qCompress`, and `readBinary` methods are being used for file operations, 
// but their implementations are not shown in this code snippet.

bool XFile::compress(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "compress", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "compress", "Source", "Missing", FileNamesJSON(srcfilename,dstfilename));
		return false;
	}

	if (run)
	{
		concurrent([srcfilename, dstfilename]
			{
				try 
				{
					writeBinary(dstfilename, qCompress(readBinary(srcfilename)));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "compress", "Compress", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}
	else
	{
		try 
		{
			return writeBinary(dstfilename, qCompress(readBinary(srcfilename)));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "compress", "Compress", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  unCompressFile
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
//----------------------------------------------------------------------------- 

// This source code defines a method `unCompress` within the `XFile` class 
// that attempts to uncompress a file. The method accepts three parameters: 
// the source file name (`srcfilename`), the destination file name 
// (`dstfilename`), and a boolean (`run`) to determine whether the 
// operation should be performed concurrently.
// Here is a step-by-step summary of what the code does :

// 1. It first checks if the trace for the file is active using 
// `XInject: : isTrace("FILE")`, and if true, logs a trace message 
// using `X_LOGJSON`.

// 2. Then, it checks whether the source file exists using the `exists` method.
// If the file does not exist, it logs a warning message using `X_WARNINGJSON` 
// and returns false.

// 3. If the `run` parameter is true, it tries to uncompress the source file 
// concurrently using a lambda function inside the `concurrent` method.
// If any exception occurs during this process, it logs a warning message 
// and continues.

// 4. If the `run` parameter is false, it tries to uncompress the source 
// file synchronously inside a try - catch block.If any exception occurs,
// it logs a warning message and returns false.

// 5. The uncompression is done using `qUncompress` method, reading from the 
// source file and writing to the destination file using `readBinary` and 
// `writeBinary` methods respectively.

// The return type of the method is boolean, indicating the success(true) or failure(false) of the uncompression operation.

bool XFile::unCompress(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "unCompress", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "unCompress", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{
		
		concurrent([srcfilename,dstfilename]
			{
				try
				{
					writeBinary(dstfilename, qUncompress(readBinary(srcfilename)));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "unCompress", "UnCompress", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}
	else
	{
		try
		{
			return writeBinary(dstfilename, qUncompress(readBinary(srcfilename)));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "unCompress", "UnCompress", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  compressFile
// Params:  const QString& srcfilename
//----------------------------------------------------------------------------- 

// This code defines a method in the XFile class called "compress". 
// It takes two parameters: "srcfilename" which is a QString and "run" which is a boolean.
// It returns the result of calling a overloaded method with "srcfilename",
// "srcfilename.z" (appending ".z" to the source filename), and "run" as arguments. 

bool XFile::compress(const QString& srcfilename, bool run)
{
	return compress(srcfilename, srcfilename+QString(".z"),run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  unCompressFile
// Params:  const QString& srcfilename
// Params:  
//----------------------------------------------------------------------------- 
// This code defines a method named `unCompress` within the `XFile` class that 
// takes two parameters: `srcfilename` (a reference to a `QString`) and
// `run` (a boolean). The method calls itself recursively with slightly
// modified arguments - it removes the ".z" extension from `srcfilename`
// and uses that as the second parameter run in the recursive call. 
// It returns the result of this overloaded call. 

bool XFile::unCompress(const QString& srcfilename, bool run)
{
	return unCompress(srcfilename , QString(srcfilename).remove(".z"),run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  crypt
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  const QString& key
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

// The source code defines a `crypt` function in the `XFile` class that takes
// in four parameters : the source filename(`srcfilename`), the destination
// filename(`dstfilename`), a cryptographic key(`key`), and a boolean(`run`)
// indicating whether the encryption should be performed concurrently or not.

// 1. At the start of the function, if the "FILE" trace is enabled in the
// `XInject` class, it logs the srcfilename and dstfilename as a JSON 
// message with a trace status of "Ok".

// 2. It then checks if the source file exists using the `exists` method.
// If the source file does not exist, it logs a warning message with a 
// status of "Missing" and returns false.

// 3. If the `run` parameter is true, it performs the encryption concurrently
// by calling a lambda function inside the `concurrent` function.This lambda
// function tries to read the binary data from the source file, encrypt it 
// using the given key with `XCrypt::crypt` method, and write the encrypted
// data to the destination file.If any error occurs during this process, 
// it logs a warning message with a status of "Failed".

// 4. If the `run` parameter is false, it performs the encryption non - 
// concurrently in a try - catch block, attempting the same read - encrypt - 
// write process as in the concurrent branch but within the main thread.
// If any exception is caught, it logs a warning message and returns false.
// If the operation succeeds, it returns true.

bool XFile::crypt(const QString& srcfilename, const QString& dstfilename, const QString& key, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "crypt", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "crypt", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{
		concurrent([srcfilename, dstfilename, key]
			{
				try
				{
					XFile::writeBinary(dstfilename, XCrypt::crypt(XFile::readBinary(srcfilename), key));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "crypt", "Crypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}

	try
	{
		return XFile::writeBinary(dstfilename,XCrypt::crypt(XFile::readBinary(srcfilename),key));
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "crypt", "Crypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  unCrypt
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  const QString& key
// Params:  bool run
//----------------------------------------------------------------------------- 

// This C++ code defines a method `XFile::unCrypt` which takes four parameters:
// source file name (`srcfilename`), destination file name (`dstfilename`),
// a key (`key`), and a boolean indicating whether to run concurrently (`run`).
// The method attempts to decrypt a file and write the result to a destination file.

// 1. Initially, it checks if tracing is enabled for "FILE" using 
// `XInject::isTrace`, and if so, logs a trace message with some details using
// `X_LOGJSON`.

// 2. It then checks if the source file exists using the `exists` function.
// If the file does not exist, it logs a warning message using `X_WARNINGJSON`
// and returns false.

// 3. If the `run` parameter is true, it attempts to decrypt the file 
// concurrently using a lambda function which is passed to the `concurrent`
// function.Inside the lambda, it reads the binary data from the source file,
// decrypts it using the `XCrypt::unCrypt` function(with the given key), 
// and writes the result to the destination file.If any exception occurs, 
// it logs a warning message and the lambda function ends.

// 4. If the `run` parameter is false, it performs the decryption in a similar
// manner but in a synchronous way(not inside a lambda passed to `concurrent`).
// If an exception occurs during this process, it logs a warning message and
// returns false.

// 5. If the decryption and writing to the destination file succeed, it returns true.

bool XFile::unCrypt(const QString& srcfilename, const QString& dstfilename, const QString& key, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "unCrypt", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "unCrypt", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{

		concurrent([srcfilename, dstfilename,key]
			{
				try
				{
					XFile::writeBinary(dstfilename, XCrypt::unCrypt(XFile::readBinary(srcfilename), key));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "unCrypt", "UnCrypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}

			});

		return true;
	}

	try
	{
		return XFile::writeBinary(dstfilename, XCrypt::unCrypt(XFile::readBinary(srcfilename), key));
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "unCrypt", "UnCrypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  wait
//----------------------------------------------------------------------------- 

// This code defines a method named `wait` in the `XFile` class.Within the
// method, it iterates over all the "future" objects stored in the
// `m_Futures` container.For each "future", it calls the `waitForFinished()` 
// method, which will block the execution until that particular "future" is 
// finished.After iterating over all the "futures", it clears the
// `m_Futures` container.

void XFile::wait()
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
// Class:   XFile
// Method:  FileNameJSON
// Params:  const QString& filename  
//----------------------------------------------------------------------------- 

// This source code defines a method named `FileNameJSON` that belongs to the
// `XFile` class.The method takes a QString parameter named `filename`
// and returns a QJsonObject.Inside the method, a QString variable `aliasfilename`
// is initialized by calling an `alias` function with `filename` as the argument.
// Then, it checks whether `aliasfilename` is equal to `filename`. If they 
// are equal, a key - value pair with the key "Filename" and the value as 
// `filename` is added to the QJsonObject `obj`. If they are not equal, two 
// key - value pairs are added to `obj`: one with the key "FilenameAlias" and 
// the value `filename`, and another with the key "Filename" and the value 
// `aliasfilename`. Finally, `obj` is returned.

QJsonObject XFile::FileNameJSON(const QString& filename)
{
	QJsonObject obj;

	QString aliasfilename(alias(filename));

	if (aliasfilename == filename)
	{
		obj["Filename"] = filename;
	}
	else
	{
		obj["FilenameAlias"] = filename;
		obj["Filename"] = aliasfilename;
	}
	
	return obj;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  FileNamesJSON
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename 
//----------------------------------------------------------------------------- 

// This is a function named `FileNamesJSON` in the `XFile` class.It accepts
// two `QString` arguments : `srcfilename` and `dstfilename`. The function 
// returns a `QJsonObject`.

// Inside the function :

// 1. A `QJsonObject` named `obj` is created.

// 2. It computes aliases for `srcfilename` and `dstfilename` using
//  a function named `alias`.

// 3. If the alias of `srcfilename` is the same as `srcfilename`, it sets
//  the `"SrcFilename"` key in the JSON object to `srcfilename`. If not,
//  it sets `"SrcFilenameAlias"` to `srcfilename` and `"SrcFilename"` to the
//  alias of `srcfilename`.

// 4. Similarly, it does the same check for `dstfilename` and sets the
//  `"DstFilename"` and `"DstFilenameAlias"` keys in the JSON object accordingly.

// 5. Finally, it returns the JSON object.

QJsonObject XFile::FileNamesJSON(const QString& srcfilename, const QString& dstfilename)
{
	QJsonObject obj;

	QString srcaliasfilename(alias(srcfilename));

	if (srcaliasfilename == srcfilename)
	{
		obj["SrcFilename"] = srcfilename;
	}
	else
	{
		obj["SrcFilenameAlias"] = srcfilename;
		obj["SrcFilename"] = srcaliasfilename;
	}

	QString dstaliasfilename(alias(dstfilename));
	if (dstaliasfilename == dstfilename)
	{
		obj["DstFilename"] = dstfilename;
	}
	else
	{
		obj["DstFilenameAlias"] = dstfilename;
		obj["DstFilename"] = dstaliasfilename;
	}

	return obj;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  append
// Params:  QFuture<void>& future
//----------------------------------------------------------------------------- 
void XFile::append(QFuture<void> future)
{
	m_Futures.append(future);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  mkPath
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

// This code defines a function named "mkFilePath" in the "XFile" class, which
// takes a string parameter "filename" and returns a boolean value. 

// It tries to create a file path using the given "filename" with the 
// following steps :

// 1. It retrieves file information using "alias(filename)" and stores it
//  in the "info" object.

// 2. It attempts to create a path using the "mkpath" method on a "QDir"
//  object created with the path from the "info" object.

// 3. If the path is successfully created, it returns true.

// If any exception occurs during this process(caught by the catch block),
// it logs a warning message with the "X_WARNINGJSON" macro, which includes
// details like "CoreStorage", "XFile", "mkFilePath", and the filename.
// In the case of an exception, it returns false.

bool XFile::mkFilePath(const QString& filename)
{
	try
	{
		QFileInfo info(alias(filename));
		return QDir(info.path()).mkpath(".");
	}
	catch(...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "mkFilePath", "MakeFilePath","Failed", FileNameJSON(filename));
	}

	return false;
}

