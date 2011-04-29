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

#ifndef ANDROIDCAMJNI_H
#define ANDROIDCAMJNI_H

#include <jni.h>
#include <QDebug>
#include <QtGui/QImage>
#include <androidcamimagecapturecontrol.h>
#include <QtGui/QPalette>
#include <QThread>
#include <androidcammediacapturecontrol.h>

class AndroidCamImageCaptureControl;
class AndroidCamMediaCaptureControl;
namespace QtCameraJni
{
    enum State {
        Load=0,
        Unload,
        StartPreview,
        StopPreview
    };

    enum MediaFormat {
        ThreeGP=1,
        MPFour
    };

    void setCameraState(State);
    void takePicture(AndroidCamImageCaptureControl *);
    void setSceneMode(QString &);
    void getCompensationRange(int *);
    void setCompensation(int);
    void setFocusMode(QString &);
    void setFlashMode(QString &);
    void setWhiteBalanceMode(QString &);
    void setImageSettings(QList<int>);
    QStringList getSupportedSceneModes();
    QStringList getSupportedFocusModes();
    QStringList getSupportedFlashModes();
    QStringList getSupportedWhiteBalanceModes();
    QList<int> getSupportedImageResolutions();
    QList<int> getSupportedImageFormats();
    int getMaxZoom();
    int getZoom();
    void setZoom(int);
    QImage&  getBuffer ();
    void wait();
    void waitlocked();
    void waitUnlocked();
    void stoprecord();
    void startrecord(AndroidCamMediaCaptureControl*);
    void setVideoOutputLocation(QString &);
    void setVideoOutputFormat(MediaFormat format);
    void setVideoEncodingBitrate(int);
    void setMaxVideoSize(long long);
    void setVideoSettings(QList<int>);
    void setAudioBitRate(int);
    void setAudioChannelsCount(int);
    void setVideoPreviewParams(QList<int>);

}

#endif // ANDROIDCAMJNI_H
