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

#ifndef ANDROIDCAMCONTROL_H
#define ANDROIDCAMCONTROL_H


#include <qcameracontrol.h>
#include <qcameraviewfinder.h>
#include <androidcamjni.h>
#include <QDebug>
#include <androidcamservice.h>


QT_USE_NAMESPACE
class AndroidCamService;
class AndroidCamControl:public QCameraControl
{
    Q_OBJECT
public:
    AndroidCamControl(AndroidCamService *,QObject *parent = 0);
    ~AndroidCamControl();

public: // QCameraControl

    // State
    QCamera::State state() const;
    void setState(QCamera::State state);

    // Status
    QCamera::Status status() const;

    // Capture Mode
    QCamera::CaptureMode captureMode() const;
    void setCaptureMode(QCamera::CaptureMode);
    bool isCaptureModeSupported(QCamera::CaptureMode mode) const;

    // Property Setting
    bool canChangeProperty(QCameraControl::PropertyChangeType changeType, QCamera::Status status) const;
    QCamera::Status   m_internalState;
    QCamera::State    m_requestedState;

    QCamera::CaptureMode  m_requestedCaptureMode;
    QCamera::CaptureMode        m_captureMode;

    void cameraLoad();
    void cameraUnload();
    void cameraActive();
    void cameraStop();

private:
    AndroidCamService * m_service;
};

#endif
