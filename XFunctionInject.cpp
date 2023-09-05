#include "XFunctionInject.h"

XStringInject XInject::m_StringInjects;
QSet<QString> XInject::m_Trace;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    16.02.2023
// Context: DefaultNamespace
// Class:   XFunctionInject
// Method:  inject
// Params:  const QString& name
// Params:  functionString function
//----------------------------------------------------------------------------- 
void XInject::inject(const QString& name, XFunctionString function)
{
	m_StringInjects.inject(name, function);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    16.02.2023
// Context: DefaultNamespace
// Class:   XFunctionInject
// Method:  call
// Params:  const QString& name
// Params:  const QString& content
// Params:  
//----------------------------------------------------------------------------- 
QString XInject::call(const QString& name, const QString& content)
{
	return m_StringInjects.call(name, content);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    16.02.2023
// Context: DefaultNamespace
// Class:   XFunctionInject
// Method:  hasStringFunction
// Params:  const QString& name
//----------------------------------------------------------------------------- 

bool XInject::hasStringFunction(const QString& name)
{
	return m_StringInjects.hasFunction(name);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XFunctionInject
// Method:  Trace
// Params:  const QString& name
//----------------------------------------------------------------------------- 

void XInject::Trace(const QString& name)
{
	m_Trace.insert(name);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XFunctionInject
// Method:  ClearTrace
// Params:  const QString& name
//----------------------------------------------------------------------------- 
void XInject::ClearTrace(const QString& name)
{
	if (name.isEmpty())
		m_Trace.clear();
	else
		m_Trace.remove(name);
}

