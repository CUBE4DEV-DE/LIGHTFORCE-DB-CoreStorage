#include "XCompress.h"


#include "XDirectory.h"
#include "XFile.h"

#include <QtGui/private/qzipreader_p.h>
#include <QtGui/private/qzipwriter_p.h>

#include "XFileBlock.h"

//#include "../CoreBase/XDebug.h"

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XPackage
// Method:  packup
// Params:  const QString& srcdirectory
// Params:  const QString& filename
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XCompress::pack(const QString& srcdirectory, const QString& filename, bool run)
{
    QStringList files = XDirectory::tree(srcdirectory);
    if (files.isEmpty())
        return false;

    XFileBlock block(filename, true);
	QZipWriter writer(filename);

	for(const QString&filepath:files)
    {
        writer.addFile(filepath, XFile::readBinary(srcdirectory+QDir::separator()+ filepath));
    }

    writer.close();

    return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XPackage
// Method:  unpack
// Params:  const QString& srcdirectory
// Params:  const QString& filename
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XCompress::unPack(const QString& dstdirectory, const QString& filename, bool run)
{
    //O_STACK(XCompress, unPack)
    //O_PARAM(dstdirectory);
    //O_PARAM(filename);

    XFileBlock block(filename);

    QZipReader reader(filename);

    for (const QZipReader::FileInfo& fileInfo : reader.fileInfoList())
    {
        //O_PARAM(fileInfo.filePath);

        if (fileInfo.isFile) 
        {
            XFile::writeBinary(dstdirectory + QDir::separator() + fileInfo.filePath,reader.fileData(fileInfo.filePath));
        }
    }

    return true;
}
