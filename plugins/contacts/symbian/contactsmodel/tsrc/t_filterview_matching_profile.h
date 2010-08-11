/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cntview.h>
#include "t_utils2.h"
#include "t_utils.h"


//
// Forward declarations.
//

class CViewTesterBase;


//
// CTestConductor.
//

class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL();
	~CTestConductor();
	void AddContactL(TInt aBitwiseFilterType);
	void SetTestError(TInt aTestError);
private:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
	void AddContactsL();
private:
	RFs iFs;
	RTest iTest;
	CLog* iLog;
	CContactDatabase* iDb;
	CRandomContactGenerator* iRandomGenerator;
	TInt iTotalContacts;
	TInt iTestError;
	};


//
// CViewTester.
//

class CViewTester : public CActive, public MContactViewObserver
	{
public:
	static CViewTester* NewL(CLog& aLog,RTest& aTest,CContactDatabase& iDb,CTestConductor* aTestConductor);
	~CViewTester();
private:
	CViewTester(CLog& aLog,RTest& aTest,CContactDatabase& iDb,CTestConductor* aTestConductor);
	void ConstructL();
	void NextTest();
	void ExceriseViewL(CContactViewBase& aView);
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	void TestViewNotificationsL(CContactViewBase& aView);
private:
	TBool ContactIdsEqualL(const CContactViewBase& aView, TInt aIndex);
	void TestViewIndiciesL(const CContactViewBase& aView);
	void PrintTimeTaken(TInt aProfile);
	void PrintTotalTimeTaken();

	void ProfileContactMatchingPrefixL(TInt aNumberOfS, CContactFilteredView* aContactFilterView);
	void ProfileContactMatchingCriteriaL(TInt aNumberOfS, CContactFilteredView* aContactFilterView);

private:
	enum TTest
		{
		ECreateLocalView,
		ECreateUnfilteredView,
		ECreateLandlineFilterView,
		ETestContactsMatchingPrefixL_LandLine,
		ETestContactsMatchingPrefixL_Unfiltered,
		ETestContactsMatchingCriteriaL_LandLine,
		ETestContactsMatchingCriteriaL_Unfiltered,
		ENumTests
		};

private:
	CLog& iLog;
	RTest& iTest;
	CContactDatabase& iDb;
	TInt iCurrentTest;
	RContactViewSortOrder iSortOrder_1;
	RContactViewSortOrder iSortOrder_2;
	CContactTextDef* iTextDef;
	TBuf<128> iScratchBuf;
	CContactNamedRemoteView* iNamedRemoteView;
	CContactFilteredView* iLandlineFilterView;
	CContactFilteredView* iUnfilteredView;
	TInt iFilter;
	TInt iNumNotificationExpected;
	CTestConductor* iTestConductor;
	TInt iItemEventsExpected;
	TInt iItemsEventsRecieved;
	TInt iTimeForTestsToComplete[ENumTests];
	TCntProfile iProfile;
	};
