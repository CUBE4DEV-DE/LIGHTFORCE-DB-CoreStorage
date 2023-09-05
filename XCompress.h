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
//----------------------------------------------------------------------------- 
	
class CORESTORAGE_EXPORT XCompress
{
	public:
		static bool pack(const QString& srcdirectory, const QString& filename, bool run);
		static bool unPack(const QString& dstdirectory, const QString& filename, bool run);
};

};
using namespace CoreStorage;


