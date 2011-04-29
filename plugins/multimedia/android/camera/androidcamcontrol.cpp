/*
THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "androidcamcontrol.h"
#include <QtGui/QApplication>
#include <QtGui/QWidget>


AndroidCamControl::AndroidCamControl(AndroidCamService * service,QObject *parent) :
        QCameraControl(parent),
        m_internalState(QCamera::UnloadedStatus),
        m_requestedState(QCamera::UnloadedState),
        m_service(service)
{
}

AndroidCamControl::~AndroidCamControl()
{
    m_service = NULL;
}

void AndroidCamControl::setState(QCamera::State state)
{

    if (m_requestedState == state) {
        return;
    }

    switch (state) {

    case QCamera::UnloadedState: // To UnloadedState - Release resources
        switch (m_internalState) {
        case QCamera::UnloadedStatus:
            // Do nothing
            break;

        case QCamera::LoadedStatus:
            // Unload
            QtCameraJni::setCameraState(QtCameraJni::Unload);
            m_internalState = QCamera::UnloadedStatus;
            emit statusChanged(m_internalState);
            break;

        case QCamera::ActiveStatus:
            //Stop and Unload
            QtCameraJni::setCameraState(QtCameraJni::StopPreview);
            QtCameraJni::setCameraState(QtCameraJni::Unload);
            m_internalState = QCamera::UnloadedStatus;
            emit statusChanged(m_internalState);
            break;

        default:
            return;
        }

        m_requestedState = QCamera::UnloadedState;
        break;

        case QCamera::LoadedState: // To LoadedState - Reserve resources OR Stop ViewFinder and Cancel Capture
            switch (m_internalState) {

            case QCamera::UnloadedStatus:
                //Load
                QtCameraJni::setCameraState(QtCameraJni::Load);
                m_internalState = QCamera::LoadedStatus;
                emit statusChanged(m_internalState);
                break;

            case QCamera::LoadedStatus:
                // Do nothing
                break;
            case QCamera::ActiveStatus:
                // Stop
                QtCameraJni::setCameraState(QtCameraJni::StopPreview);
                m_internalState = QCamera::LoadedStatus;
                emit statusChanged(m_internalState);
                break;

            default:
                return;
            }

            m_requestedState = QCamera::LoadedState;
            break;

        case QCamera::ActiveState: // To ActiveState - (Reserve Resources and) Start ViewFinder

            switch (m_internalState) {

            case QCamera::UnloadedStatus:
                // Load and Start (setting state handles starting)
                QtCameraJni::setCameraState(QtCameraJni::Load);
                QtCameraJni::setCameraState(QtCameraJni::StartPreview);
                m_internalState = QCamera::ActiveStatus;
                emit statusChanged(m_internalState);
                break;

            case QCamera::LoadedStatus:
                // Start
                QtCameraJni::setCameraState(QtCameraJni::StartPreview);
                m_internalState = QCamera::ActiveStatus;
                emit statusChanged(m_internalState);
                break;
            case QCamera::ActiveStatus:
                // Do nothing
                break;

            default:
                return;
            }

            m_requestedState = QCamera::ActiveState;
            break;

        default:
            return;
        }
    emit stateChanged(m_requestedState);

}

QCamera::State AndroidCamControl::state() const
{
    return m_requestedState;
}

QCamera::Status AndroidCamControl::status() const
{
    return m_internalState;
}

QCamera::CaptureMode AndroidCamControl::captureMode() const
{
    return m_captureMode;
}

void AndroidCamControl::setCaptureMode(QCamera::CaptureMode mode)
{

    if (!isCaptureModeSupported(mode)) {
        return;
    }

    switch (m_internalState) {

    case QCamera::UnloadedStatus:
    case QCamera::LoadedStatus:
    case QCamera::StandbyStatus:

        switch (mode) {

        case QCamera::CaptureStillImage:
            m_requestedCaptureMode = QCamera::CaptureStillImage;
            m_captureMode = QCamera::CaptureStillImage;
            break;

        case QCamera::CaptureVideo:
            m_requestedCaptureMode = QCamera::CaptureVideo;
            m_captureMode = QCamera::CaptureVideo;
            break;
        }

        break;

    case QCamera::LoadingStatus:
    case QCamera::StartingStatus:
        m_requestedCaptureMode = mode;
        return;

    case QCamera::ActiveStatus:
        // Stop, Change Mode and Start again

        switch (mode) {

        case QCamera::CaptureStillImage:
            m_service->m_mediaCaptureControl->stop();
            m_requestedCaptureMode = QCamera::CaptureStillImage;
            m_captureMode = QCamera::CaptureStillImage;
            emit captureModeChanged(QCamera::CaptureStillImage);
            break;

        case QCamera::CaptureVideo:
            m_captureMode = QCamera::CaptureVideo;
            emit captureModeChanged(QCamera::CaptureVideo);
            break;

        }

        break;

    default:
        break;
    }

}

bool AndroidCamControl::isCaptureModeSupported(QCamera::CaptureMode mode) const
{
    switch (mode) {
    case QCamera::CaptureStillImage:
        return true;
    case QCamera::CaptureVideo:
        return true;

    default:
        return false;
    }
}


bool AndroidCamControl::canChangeProperty(QCameraControl::PropertyChangeType changeType, QCamera::Status status) const
{
    Q_UNUSED(status);

    bool returnValue = false;
    switch (changeType) {
    case QCameraControl::CaptureMode:
    case QCameraControl::VideoEncodingSettings:
    case QCameraControl::ImageEncodingSettings:
    case QCameraControl::Viewfinder:
        returnValue = true;
        break;

    default:
        // Safer to revert state before the unknown operation
        returnValue = false;
        break;
    }
    return returnValue;
}





