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

#include <androidcamjni.h>
#include <androidcamcontrol.h>
#include <QBuffer>
#include <QFile>
#include <QQueue>
#include <QWaitCondition>
#include <QTime>
#include <QtGui/QPixmap>
#include <QList>
using namespace std;
#define QtCameraClassPathName "eu/licentia/necessitas/mobile/QtCamera"
static JNIEnv *m_env = NULL;
static JavaVM *m_javaVM = NULL;
static const int kBufferCount = 4;
template <class T> class CameraGlobalObject
{
public:
    CameraGlobalObject(const T & value= NULL)
    {
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
    }
    virtual ~CameraGlobalObject()
    {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
    }
    T& operator =(const T& value)
                 {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
        if (m_env)
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
        return m_value;
    }
    T& operator()()
    {
        return m_value;
    }

private:
    T m_value;
};


namespace QtCameraJni
{
    static AndroidCamImageCaptureControl * m_imageCaptureControl = NULL;
    static AndroidCamMediaCaptureControl * m_videoCaptureControl = NULL;
    static QMutex m_mutex;
    static QQueue<QPixmap *> m_imageQueue;
    static QWaitCondition m_waitCondition;
    static QPixmap *m_pixmap;
    QString m_tempstring;
    static QImage m_imageBuffer;


    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    struct QtCameraId
    {
        CameraGlobalObject<jobject> m_CameraObject;
        CameraGlobalObject<jobject> m_CameraInstance;
        jmethodID m_CameraConstructorID;
        jmethodID m_setCameraStateID;
        jmethodID m_takePictureID;
        jmethodID m_setSceneModeID;
        jmethodID m_getCompensationRangeID;
        jmethodID m_setCompensationID;
        jmethodID m_setFocusModeID;
        jmethodID m_setFlashModeID;
        jmethodID m_getMaxZoomID;
        jmethodID m_getZoomID;
        jmethodID m_setZoomID;
        jmethodID m_setWhiteBalanceModeID;
        jmethodID m_setImageSettingsID;
        jmethodID m_getSupportedImageResolutionsID;
        jmethodID m_getSupportedImageFormatsID;
        jmethodID m_startRecordingID;
        jmethodID m_stopRecordingID;
        jmethodID m_setOutputLocationID;
        jmethodID m_setOutputFormatID;
        jmethodID m_setVideoEncodingBitrateID;
        jmethodID m_setMaxVideoSizeID;
        jmethodID m_setVideoSettingsID;
        jmethodID m_setAudioBitRateID;
        jmethodID m_setAudioChannelsCountID;

        jfieldID m_sceneListID;
        jfieldID m_focusModesID;
        jfieldID m_flashModesID;
        jfieldID m_whiteBalanceModesID;
        jfieldID m_videoPreviewParamsID;
    };

    static QtCameraId qtCameraId;

    void setCameraState(State state)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setCameraStateID,state);
        m_javaVM->DetachCurrentThread();

    }


    void takePicture(AndroidCamImageCaptureControl * control)
    {
        m_imageCaptureControl = control;
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_takePictureID);
        m_javaVM->DetachCurrentThread();

    }

    static void getImage(JNIEnv* env, _jobject obj,jbyteArray image)
    {
        Q_UNUSED(obj);
        QImage picture;
        int size=env->GetArrayLength(image);
        jbyte* imageBytearray = env->GetByteArrayElements(image,NULL);
        picture.loadFromData((uchar*)imageBytearray,size,"JPG");
        m_imageCaptureControl->ImageRecieved(picture);
        m_imageCaptureControl = NULL;
        env->ReleaseByteArrayElements(image,imageBytearray,0);
    }

    void setSceneMode(QString & mode)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        jstring jid = env->NewString((const jchar *) mode.unicode(), (long) mode.length());
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setSceneModeID,jid);
        env->DeleteLocalRef(jid);
        m_javaVM->DetachCurrentThread();

    }

    void getCompensationRange(int * rangeStore)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        jintArray range =(jintArray) env->CallObjectMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_getCompensationRangeID);
        int * getRange =  env->GetIntArrayElements(range,NULL);
        qMemCopy(rangeStore,getRange,sizeof(int)*2);
        env->ReleaseIntArrayElements(range,getRange,0);
        m_javaVM->DetachCurrentThread();
    }

    void setCompensation(int value)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setCompensationID,value);
        m_javaVM->DetachCurrentThread();
    }

    void setFocusMode(QString & mode)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        jstring jid = env->NewString((const jchar *) mode.unicode(), (long) mode.length());
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setFocusModeID,jid);
        env->DeleteLocalRef(jid);
        m_javaVM->DetachCurrentThread();

    }

    void setFlashMode(QString & mode)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        jstring jid = env->NewString((const jchar *) mode.unicode(), (long) mode.length());
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setFlashModeID,jid);
        env->DeleteLocalRef(jid);
        m_javaVM->DetachCurrentThread();

    }

    QStringList getSupportedSceneModes()
    {
        JNIEnv* env;
        QStringList supportedScene;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return QStringList();
        }

        jobjectArray sceneArray = (jobjectArray)env->GetObjectField((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_sceneListID);
        jsize sceneCount = env->GetArrayLength(sceneArray);

        for(int j=0;j<sceneCount;j++)
        {
            jstring jscene = (jstring)env->GetObjectArrayElement(sceneArray,j);
            const jchar *  _jchar_str_scene;
            _jchar_str_scene = env->GetStringChars(jscene,0);
            m_tempstring.clear();
            m_tempstring.setUnicode((QChar *) _jchar_str_scene, env->GetStringLength(jscene));
            supportedScene.append(m_tempstring);
            env->ReleaseStringChars(jscene, _jchar_str_scene);
            env->DeleteLocalRef(jscene);

        }

        env->DeleteLocalRef(sceneArray);

        m_javaVM->DetachCurrentThread();
        return supportedScene;
    }

    QStringList getSupportedFocusModes()
    {
        JNIEnv* env;
        QStringList supportedFocus;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return QStringList();
        }
        jobjectArray focusArray = (jobjectArray)env->GetObjectField((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_focusModesID);
        jsize focusCount = env->GetArrayLength(focusArray);
        for(int j=0;j<focusCount;j++)
        {
            jstring jfocus = (jstring)env->GetObjectArrayElement(focusArray,j);
            const jchar *  _jchar_str_focus;
            _jchar_str_focus = env->GetStringChars(jfocus,0);
            m_tempstring.clear();
            m_tempstring.setUnicode((QChar *) _jchar_str_focus, env->GetStringLength(jfocus));
            supportedFocus.append(m_tempstring);
            env->ReleaseStringChars(jfocus, _jchar_str_focus);
            env->DeleteLocalRef(jfocus);
        }
        env->DeleteLocalRef(focusArray);

        m_javaVM->DetachCurrentThread();
        return supportedFocus;

    }

    QStringList getSupportedFlashModes()
    {
        JNIEnv* env;
        QStringList supportedFlash;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return QStringList();
        }

        jobjectArray flashArray = (jobjectArray)env->GetObjectField((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_flashModesID);
        jsize flashCount = env->GetArrayLength(flashArray);

        for(int j=0;j<flashCount;j++)
        {
            jstring jflash = (jstring)env->GetObjectArrayElement(flashArray,j);
            const jchar *  _jchar_str_flash;
            _jchar_str_flash = env->GetStringChars(jflash,0);
            m_tempstring.clear();
            m_tempstring.setUnicode((QChar *) _jchar_str_flash, env->GetStringLength(jflash));
            supportedFlash.append(m_tempstring);
            env->ReleaseStringChars(jflash, _jchar_str_flash);
            env->DeleteLocalRef(jflash);

        }

        env->DeleteLocalRef(flashArray);

        m_javaVM->DetachCurrentThread();
        return supportedFlash;
    }

    QStringList getSupportedWhiteBalanceModes()
    {
        JNIEnv* env;
        QStringList supportedWhiteBalanceModes;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return QStringList();
        }

        jobjectArray whiteBalanceArray = (jobjectArray)env->GetObjectField((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_whiteBalanceModesID);
        jsize whiteBalanceCount = env->GetArrayLength(whiteBalanceArray);

        for(int j=0;j<whiteBalanceCount;j++)
        {
            jstring jwhiteBalance =  (jstring)env->GetObjectArrayElement(whiteBalanceArray,j);
            const jchar *  _jchar_str_whiteBalance;
            _jchar_str_whiteBalance = env->GetStringChars(jwhiteBalance,0);
            m_tempstring.clear();
            m_tempstring.setUnicode((QChar *) _jchar_str_whiteBalance, env->GetStringLength(jwhiteBalance));
            supportedWhiteBalanceModes.append(m_tempstring);
            env->ReleaseStringChars(jwhiteBalance, _jchar_str_whiteBalance);
            env->DeleteLocalRef(jwhiteBalance);

        }

        env->DeleteLocalRef(whiteBalanceArray);
        m_javaVM->DetachCurrentThread();
        return supportedWhiteBalanceModes;

    }

    QList<int> getSupportedImageFormats()
    {
        JNIEnv* env;
        QList<int> supportedFormats;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return  QList<int>();
        }
        jintArray imageFormatsArray = (jintArray)env->CallObjectMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_getSupportedImageFormatsID);
        jsize size = env->GetArrayLength(imageFormatsArray);
        jint * formatArray = env->GetIntArrayElements(imageFormatsArray,NULL);
        for(int i=0;i<size;i++)
        {
            supportedFormats.append((int)formatArray[i]);
        }
        env->ReleaseIntArrayElements(imageFormatsArray,formatArray,0);
        env->DeleteLocalRef(imageFormatsArray);
        m_javaVM->DetachCurrentThread();

        return supportedFormats;
    }

    QList<int> getSupportedImageResolutions()
    {
        JNIEnv* env;
        QList<int> supportedResolutions;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return  QList<int>();
        }
        jintArray imageResolutionsArray = (jintArray)env->CallObjectMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_getSupportedImageResolutionsID);
        jsize size = env->GetArrayLength(imageResolutionsArray);
        jint * resolutionArray = env->GetIntArrayElements(imageResolutionsArray,NULL);
        for(int i=0;i<size;i++)
        {
            supportedResolutions.append((int)resolutionArray[i]);
        }
        env->ReleaseIntArrayElements(imageResolutionsArray,resolutionArray,0);
        env->DeleteLocalRef(imageResolutionsArray);
        m_javaVM->DetachCurrentThread();
        return supportedResolutions;
    }

    int getMaxZoom()
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return 0;
        }

        int maxZoom = env->CallIntMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_getMaxZoomID);
        m_javaVM->DetachCurrentThread();
        return maxZoom;

    }

    int getZoom()
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return 0;
        }

        int zoom = env->CallIntMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_getZoomID);
        m_javaVM->DetachCurrentThread();
        return zoom;

    }

    void setZoom(int zoom)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setZoomID,zoom);
        m_javaVM->DetachCurrentThread();

    }


    void setWhiteBalanceMode(QString & mode)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }

        jstring jid = env->NewString((const jchar *) mode.unicode(), (long) mode.length());
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setWhiteBalanceModeID,jid);
        env->DeleteLocalRef(jid);
        m_javaVM->DetachCurrentThread();

    }

    void setImageSettings(QList<int> imageSettings)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        int settings[imageSettings.count()];
        for(int i=0;i<imageSettings.count();i++)
        {
            settings[i] = imageSettings.at(i);
        }
        jintArray jimageSettings = env->NewIntArray(imageSettings.count());
        env->SetIntArrayRegion(jimageSettings,0,sizeof(settings)/sizeof(int),(jint *)settings);
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setImageSettingsID,jimageSettings);
        env->DeleteLocalRef(jimageSettings);
        m_javaVM->DetachCurrentThread();

    }

    static void getPreviewBuffers(JNIEnv* env, _jobject obj,jbyteArray preview)
    {
        Q_UNUSED(obj);
        m_mutex.lock();
        int size=env->GetArrayLength(preview);
        m_pixmap = new QPixmap();
        jbyte* bytePriview= env->GetByteArrayElements(preview,NULL);
        m_pixmap->loadFromData((uchar*)bytePriview,size);
        m_imageQueue.enqueue(m_pixmap);
        env->ReleaseByteArrayElements(preview,bytePriview,0);
        m_waitCondition.wakeAll();
        m_mutex.unlock();
    }

    void wait ()
    {
        m_waitCondition.wait(&m_mutex);
    }

    void waitlocked()
    {
        m_mutex.lock();
    }

    void waitUnlocked()
    {
        m_mutex.unlock();
    }

    QImage& getBuffer ()
    {
        if(!m_imageQueue.isEmpty())
        {
            m_imageBuffer = m_imageQueue.head()->toImage();
            delete(m_imageQueue.dequeue());
        }
        return m_imageBuffer;

    }

    void startrecord(AndroidCamMediaCaptureControl* control)
    {
        m_videoCaptureControl = control;
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_startRecordingID);
        m_javaVM->DetachCurrentThread();
    }

    void stoprecord()
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_stopRecordingID);
        m_javaVM->DetachCurrentThread();
    }

    void setVideoOutputLocation(QString & path)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        jstring jid = env->NewString((const jchar *) path.unicode(), (long) path.length());
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setOutputLocationID,jid);
        env->DeleteLocalRef(jid);
        m_javaVM->DetachCurrentThread();
    }

    void setVideoOutputFormat(MediaFormat format)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setOutputFormatID,format);
        m_javaVM->DetachCurrentThread();

    }

    void setVideoEncodingBitrate(int rate)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setVideoEncodingBitrateID,rate);
        m_javaVM->DetachCurrentThread();
    }

    void setMaxVideoSize(long long size)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setMaxVideoSizeID,size);
        m_javaVM->DetachCurrentThread();
    }

    void setVideoSettings(QList<int> videoSettings)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        int settings[videoSettings.count()];
        for(int i=0;i<videoSettings.count();i++)
        {
            settings[i] = videoSettings.at(i);
        }
        jintArray jvideoSettings = env->NewIntArray(videoSettings.count());
        env->SetIntArrayRegion(jvideoSettings,0,sizeof(settings)/sizeof(int),(jint *)settings);
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setVideoSettingsID,jvideoSettings);
        env->DeleteLocalRef(jvideoSettings);
        m_javaVM->DetachCurrentThread();

    }

    void setAudioBitRate(int rate)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setAudioBitRateID,rate);
        m_javaVM->DetachCurrentThread();

    }


    void setAudioChannelsCount(int count)
    {
        JNIEnv* env;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        env->CallVoidMethod((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_setAudioChannelsCountID,count);
        m_javaVM->DetachCurrentThread();

    }

    void setVideoPreviewParams(QList<int> params)
    {
        JNIEnv* env;
        QStringList supportedScene;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return ;
        }
        int *parameters;
        parameters = new int[4];
        for(int i=0;i<4;i++)
        {
            parameters[i] = params.at(i);
        }
        jintArray jparams = env->NewIntArray(4);
        env->SetIntArrayRegion(jparams,0,4,parameters);
        env->SetObjectField((jobject)qtCameraId.m_CameraInstance(),qtCameraId.m_videoPreviewParamsID,jparams);
        env->DeleteLocalRef(jparams);
        delete(parameters);
        m_javaVM->DetachCurrentThread();
    }


    static void stopRecord(JNIEnv* env, _jobject obj)
    {
        Q_UNUSED(obj);
        Q_UNUSED(env);
        if(m_videoCaptureControl != NULL)
        {
            m_videoCaptureControl->setRecordStop();
        }
        m_videoCaptureControl = NULL;
    }


    static JNINativeMethod methods[] = {
        {"getImage", "([B)V", (void *)QtCameraJni::getImage},
        {"getPreviewBuffers", "([B)V", (void *)QtCameraJni::getPreviewBuffers},
        {"stopRecord", "()V", (void *)QtCameraJni::stopRecord}
    };

    static int registerNativeMethods(JNIEnv* env, const char* className,
                                     JNINativeMethod* gMethods, int numMethods)
    {
        jclass clazz;

        clazz = env->FindClass(className);
        if (clazz == NULL)
        {
            return JNI_FALSE;
        }

        if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
        {
            return JNI_FALSE;
        }

        qtCameraId.m_CameraObject = clazz;
        qtCameraId.m_CameraConstructorID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"<init>","()V");
        qtCameraId.m_CameraInstance= env->NewObject((jclass)qtCameraId.m_CameraObject (), qtCameraId.m_CameraConstructorID);
        qtCameraId.m_setCameraStateID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setCameraState","(I)V");
        qtCameraId.m_takePictureID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"takePicture","()V");
        qtCameraId.m_setSceneModeID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setSceneMode","(Ljava/lang/String;)V");
        qtCameraId.m_getCompensationRangeID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"getCompensationRange","()[I");
        qtCameraId.m_setCompensationID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setCompensation","(I)V");
        qtCameraId.m_setFocusModeID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setFocusMode","(Ljava/lang/String;)V");
        qtCameraId.m_setFlashModeID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setFlashMode","(Ljava/lang/String;)V");
        qtCameraId.m_getMaxZoomID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"getMaxZoom","()I");
        qtCameraId.m_getZoomID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"getZoom","()I");
        qtCameraId.m_setZoomID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setZoom","(I)V");
        qtCameraId.m_setWhiteBalanceModeID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setWhiteBalanceMode","(Ljava/lang/String;)V");
        qtCameraId.m_setImageSettingsID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setImageSettings","([I)V");
        qtCameraId.m_getSupportedImageResolutionsID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"getSupportedImageResolutions","()[I");
        qtCameraId.m_getSupportedImageFormatsID = env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"getSupportedImageFormats","()[I");
        qtCameraId.m_startRecordingID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"startRecording","()V");
        qtCameraId.m_stopRecordingID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"stopRecording","()V");
        qtCameraId.m_setOutputLocationID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setOutputFile","(Ljava/lang/String;)V");
        qtCameraId.m_setOutputFormatID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setOutputFormat","(I)V");
        qtCameraId.m_setVideoEncodingBitrateID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setVideoEncodingBitrate","(I)V");
        qtCameraId.m_setMaxVideoSizeID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setMaxVideoSize","(J)V");
        qtCameraId.m_setVideoSettingsID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setVideoSettings","([I)V");
        qtCameraId.m_setAudioBitRateID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setAudioBitRate","(I)V");
        qtCameraId.m_setAudioChannelsCountID=env->GetMethodID((jclass)qtCameraId.m_CameraObject (),"setAudioChannelsCount","(I)V");


        qtCameraId.m_sceneListID  = env->GetFieldID((jclass)qtCameraId.m_CameraObject (),"m_sceneList","[Ljava/lang/String;");
        qtCameraId.m_focusModesID = env->GetFieldID((jclass)qtCameraId.m_CameraObject (),"m_focusModes","[Ljava/lang/String;");
        qtCameraId.m_flashModesID =  env->GetFieldID((jclass)qtCameraId.m_CameraObject (),"m_flashModes","[Ljava/lang/String;");
        qtCameraId.m_whiteBalanceModesID =  env->GetFieldID((jclass)qtCameraId.m_CameraObject (),"m_whiteBalanceModes","[Ljava/lang/String;");
        qtCameraId.m_videoPreviewParamsID =  env->GetFieldID((jclass)qtCameraId.m_CameraObject (),"m_videoPreviewParams","[I");


        return JNI_TRUE;
    }

}



Q_DECL_EXPORT JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    qDebug()<<"JNI CAMERA LOADING"<<"\n";

    QtCameraJni::UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }

    m_env= uenv.nativeEnvironment;
    m_javaVM = vm;

    QtCameraJni::registerNativeMethods(m_env,QtCameraClassPathName,QtCameraJni::methods,3);
    return JNI_VERSION_1_4;
}
