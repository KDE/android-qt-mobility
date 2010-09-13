/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Solutions Commercial License Agreement provided
 ** with the Software or, alternatively, in accordance with the terms
 ** contained in a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** Please note Third Party Software included with Qt Solutions may impose
 ** additional restrictions and it is the user's responsibility to ensure
 ** that they have met the licensing requirements of the GPL, LGPL, or Qt
 ** Solutions Commercial license and the relevant license of the Third
 ** Party Software they are using.
 **
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
#ifndef S60CCAMERAENGINEOBSERVER_H
#define S60CCAMERAENGINEOBSERVER_H

// FORWARD DECLARATIONS
class CFbsBitmap;
class TECAMEvent;

enum TCameraEngineError
{
    EErrReserve,
    EErrPowerOn,
    EErrViewFinderReady,
    EErrImageReady,
    EErrAutoFocusInit,
    EErrAutoFocusMode,
    EErrAutoFocusArea,
    EErrAutoFocusRange,
    EErrAutoFocusType,
    EErrOptimisedFocusComplete,
};

class MAdvancedSettingsObserver
    {
public:

    virtual void HandleAdvancedEvent( const TECAMEvent &aEvent ) = 0;

    };

class MCameraEngineObserver
{
public:

    /**
     * Camera is ready to use for capturing images.
     */
    virtual void MceoCameraReady() = 0;

    /**
     * Camera AF lens has attained optimal focus
     */
    virtual void MceoFocusComplete() = 0;

    /**
     * Captured data is ready - call CCameraEngine::ReleaseImageBuffer()
     * after processing/saving the data (typically, JPG-encoded image)
     * @param aData Pointer to a descriptor containing a frame of camera data.
     */
    virtual void MceoCapturedDataReady( TDesC8* aData ) = 0;

    /**
     * Captured bitmap is ready.
     * after processing/saving the image, call
     * CCameraEngine::ReleaseImageBuffer() to free the bitmap.
     * @param aBitmap Pointer to an FBS bitmap containing a captured image.
     */
    virtual void MceoCapturedBitmapReady( CFbsBitmap* aBitmap ) = 0;

    /**
     * Notifies clients about errors in camera engine
     * @param aErrorType type of error (see TCameraEngineError)
     * @param aError Symbian system-wide error code
     */
    virtual void MceoHandleError( TCameraEngineError aErrorType, TInt aError ) = 0;

    /**
     * Notifies client about other events not recognized by camera engine.
     * The default implementation is empty.
     * @param aEvent camera event (see MCameraObserver2::HandleEvent())
     */
    virtual void MceoHandleOtherEvent( const TECAMEvent& /*aEvent*/ ) {}
};

class MCameraViewfinderObserver
{
public:
    /**
     * A new viewfinder frame is ready.
     * after displaying the frame, call
     * CCameraEngine::ReleaseViewFinderBuffer()
     * to free the bitmap.
     * @param aFrame Pointer to an FBS bitmap containing a viewfinder frame.
     */
    virtual void MceoViewFinderFrameReady( CFbsBitmap& aFrame ) = 0;
};

#endif // CCAMERAENGINEOBSERVER_H

// End of file
