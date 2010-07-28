/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:

*

* Description: 

*
*/

#ifndef IPCFUZZTEST_H
#define IPCFUZZTEST_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32test.h>

// CLASS DECLARATION

/**
*  CIpcFuzzTest
* 
*/
class RIpcFuzzTest : public RSessionBase
{
public: // Constructors and destructor

	/**
		* Constructor for performing 1st stage construction
		*/
	RIpcFuzzTest(RTest& aTest);

	/**
		* Destructor.
		*/
	~RIpcFuzzTest();

	/**
		* Destructor.
		*/
	void RunTestL(const TDesC& aTargetSrvName, TInt aMinFuncRange, TInt aMaxFuncRange);
	
private:
	TInt Fuzz(TInt aMsg, TInt);
	TInt FuzzL(TInt aMsg, const TDesC&);

private:
	RTest& 		 iTest;
};

#endif // IPCFUZZTEST_H

