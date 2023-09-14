#pragma once


#include "CoreStorageGlobal.h"

#include <QString>
#include <QFuture>
#include <QList>
#include <QtConcurrent>

namespace CoreStorage
{

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XPackage
// Method:  XPackage
// Description: It contains two methods, pack and unPack. These methods are
//	used to compress and decompress a given directory in zip format,
//	respectively. The input parameters to both functions are the directory
//	to process, the file name to store the compressed data, and a Boolean
//	flag indicating whether to run in a concurrent thread in a future implementation.
//----------------------------------------------------------------------------- 
	
class CORESTORAGE_EXPORT XCompress
{
	public:
		static bool pack(const QString& srcdirectory, const QString& filename, bool run);
		static bool unPack(const QString& dstdirectory, const QString& filename, bool run);
};

};
using namespace CoreStorage;


