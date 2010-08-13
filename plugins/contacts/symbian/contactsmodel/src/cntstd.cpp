/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include "cntstd.h"

GLDEF_C void Panic(TCntModelPanic aPanic)
//
// Panic the thread with CNTMODEL as the category
//
	{
	User::Panic(_L("CNTMODEL"),aPanic);
	}


