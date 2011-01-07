/****************************************************************************
**
** Copyright 2010 Elektrobit(EB)(http://www.elektrobit.com)
**
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/

//JNI file to Interact With Java
#include <QtCore>
#include <android/log.h>
#include <jni.h>

#include "qcontactandroidbackend_p.h"
#include "qcontactabook_p.h"
#include "qtcontacts.h"
#include "contactsjni.h"

static JavaVM *m_javaVM = 0;
static JNIEnv *m_env = 0;

template <class T>
        class JavaContactsGlobalObject
{
public:
    JavaContactsGlobalObject(const T & value = NULL)
    {
        if (m_env && value)
        {
            m_value=m_env->NewGlobalRef(value);
        }
        else
        {
            m_value=value;
        }
    };
    virtual ~JavaContactsGlobalObject()
    {
        if (m_value && m_env)
        {
            m_env->DeleteGlobalRef(m_value);
        }
    };
    T& operator =(const T& value)
                 {
        if (m_value && m_env)
        {
            m_env->DeleteGlobalRef(m_value);
        }
        if (m_env && value)
        {
            m_value=m_env->NewGlobalRef(value);
        }
        else
        {
            m_value=value;
        }
        return m_value;
    }
    T& operator()()
    {
        return m_value;
    }

private:
    T m_value;
};

static jmethodID m_getContactsID=0;
static jmethodID m_saveContactsID=0;
static jmethodID m_removeContactsID=0;
static jmethodID m_updateContactsID=0;
static jmethodID m_myContactsConstructor=0;
static jmethodID m_phoneNumberConstructor=0;
static jmethodID m_nameDataConstructor=0;
static jmethodID m_emailDataConstructor=0;
static jmethodID m_AddressDataConstructor=0;
static jmethodID m_organizationalDataConstructor=0;

static JavaContactsGlobalObject<jobject> m_qtAndroidContactsObjects = 0;
static JavaContactsGlobalObject<jobject> m_myContactsObject=0;
static JavaContactsGlobalObject<jobject> m_allContactsObject=0;
static JavaContactsGlobalObject<jobject> m_phoneNumberObject=0;
static JavaContactsGlobalObject<jobject> m_emailDataObject=0;
static JavaContactsGlobalObject<jobject> m_nameDataObject=0;
static JavaContactsGlobalObject<jobject> m_addressDataObject=0;
static JavaContactsGlobalObject<jobject> m_organizationalDataObject=0;
static JavaContactsGlobalObject<jobject> m_onlineAccountObject=0;

static const char *QtAndroidContactsClassPathName = "com/nokia/qt/android/QtAndroidContacts";
static const char *QtMyContactsClassPathName = "com/nokia/qt/android/MyContacts";
static const char *QtAllAndroidContactsClassPathName = "com/nokia/qt/android/AndroidContacts";
static const char *QtPhoneNumberClassPathName = "com/nokia/qt/android/PhoneNumber";
static const char *QtEmailDataClassPathName = "com/nokia/qt/android/EmailData";
static const char *QtAddressDataClassPathName = "com/nokia/qt/android/AddressData";
static const char *QtNameDataClassPathName = "com/nokia/qt/android/NameData";
static const char *QtOrganizationalDataClassPathName = "com/nokia/qt/android/OrganizationalData";
static const char *QtOnlineAccountClassPathName = "com/nokia/qt/android/OnlineAccount";

static QtContactsJNI::FieldID javaFieldIds;

static int setQtApplicationObject(JNIEnv* env)
{
    jclass qtAndroidContactClass;

    qtAndroidContactClass = env->FindClass(QtAndroidContactsClassPathName);
    if (qtAndroidContactClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtAndroidContactsClassPathName);
        return JNI_FALSE;
    }
    m_qtAndroidContactsObjects = qtAndroidContactClass;
    jclass myContactsClass;
    myContactsClass = env->FindClass(QtMyContactsClassPathName);
    if (myContactsClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtMyContactsClassPathName);
        return JNI_FALSE;
    }
    m_myContactsObject = myContactsClass;
    jclass allContactsClass;
    allContactsClass = env->FindClass(QtAllAndroidContactsClassPathName);
    if (allContactsClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtAllAndroidContactsClassPathName);
        return JNI_FALSE;
    }
    m_allContactsObject = allContactsClass;

    jclass phoneNumberClass;
    phoneNumberClass = env->FindClass(QtPhoneNumberClassPathName);
    if (phoneNumberClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtPhoneNumberClassPathName);
        return JNI_FALSE;
    }
    m_phoneNumberObject = phoneNumberClass;

    jclass emailDataClass;
    emailDataClass = env->FindClass(QtEmailDataClassPathName);
    if (emailDataClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtEmailDataClassPathName);
        return JNI_FALSE;
    }
    m_emailDataObject = emailDataClass;

    jclass addressDataClass;
    addressDataClass = env->FindClass(QtAddressDataClassPathName);
    if (addressDataClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtAddressDataClassPathName);
        return JNI_FALSE;
    }
    m_addressDataObject = addressDataClass;

    jclass organizationalDataClass;
    organizationalDataClass = env->FindClass(QtOrganizationalDataClassPathName);
    if (organizationalDataClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtOrganizationalDataClassPathName);
        return JNI_FALSE;
    }
    m_organizationalDataObject = organizationalDataClass;

    jclass nameDataClass;
    nameDataClass = env->FindClass(QtNameDataClassPathName);
    if (nameDataClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtNameDataClassPathName);
        return JNI_FALSE;
    }
    m_nameDataObject = nameDataClass;

    jclass onlieAccountClass;
    onlieAccountClass = env->FindClass(QtOnlineAccountClassPathName);
    if (onlieAccountClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtOnlineAccountClassPathName);
        return JNI_FALSE;
    }
    m_onlineAccountObject = onlieAccountClass;

    return true;
}


static int registerNativeMethods(JNIEnv* env)
{
    if (!setQtApplicationObject(env))
    {
        QANDROID_DEBUG<<"Failed In registering Native MEthods";
        return JNI_FALSE;
    }
    // Registering java methods
    m_getContactsID = env->GetMethodID((jclass)m_qtAndroidContactsObjects(),"getContacts","()V");
    m_saveContactsID = env->GetMethodID((jclass)m_qtAndroidContactsObjects(),"saveContact","(Lcom/nokia/qt/android/MyContacts;)Ljava/lang/String;");
    m_myContactsConstructor = env->GetMethodID((jclass)m_myContactsObject(),"<init>","(Lcom/nokia/qt/android/NameData;[Lcom/nokia/qt/android/PhoneNumber;[Lcom/nokia/qt/android/EmailData;Ljava/lang/String;[Lcom/nokia/qt/android/AddressData;[Lcom/nokia/qt/android/OrganizationalData;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V");
    m_phoneNumberConstructor = env->GetMethodID((jclass)m_phoneNumberObject(),"<init>","(Ljava/lang/String;I)V");
    m_emailDataConstructor =  env->GetMethodID((jclass)m_emailDataObject(),"<init>","(Ljava/lang/String;I)V");
    m_organizationalDataConstructor =  env->GetMethodID((jclass)m_organizationalDataObject(),"<init>","(Ljava/lang/String;Ljava/lang/String;I)V");
    m_AddressDataConstructor = env->GetMethodID((jclass)m_addressDataObject(),"<init>","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    m_removeContactsID = env->GetMethodID((jclass)m_qtAndroidContactsObjects(),"removeContact","(Ljava/lang/String;)I");
    m_updateContactsID = env->GetMethodID((jclass)m_qtAndroidContactsObjects(),"updateContact","(Ljava/lang/String;Lcom/nokia/qt/android/MyContacts;)V");
    m_nameDataConstructor = env->GetMethodID((jclass)m_nameDataObject(),"<init>","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    // Getting all required fieldIDs from Java
    javaFieldIds.m_androidContacts_fid = env->GetStaticFieldID((jclass)m_qtAndroidContactsObjects(),"m_androidContacts","Lcom/nokia/qt/android/AndroidContacts;");
    javaFieldIds.m_myContacts_fid = env->GetFieldID((jclass)m_allContactsObject(),"m_allAndroidContacts","[Lcom/nokia/qt/android/MyContacts;");
    javaFieldIds.m_displayName_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_dispalyName","Ljava/lang/String;");
    javaFieldIds.m_nameData_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_names","Lcom/nokia/qt/android/NameData;");
    javaFieldIds.m_firstName_fid = (env)->GetFieldID((jclass)m_nameDataObject(),"m_firstName","Ljava/lang/String;");
    javaFieldIds.m_lastName_fid = (env)->GetFieldID((jclass)m_nameDataObject(),"m_lastName","Ljava/lang/String;");
    javaFieldIds.m_middleName_fid = (env)->GetFieldID((jclass)m_nameDataObject(),"m_middleName","Ljava/lang/String;");
    javaFieldIds.m_prefix_fid = (env)->GetFieldID((jclass)m_nameDataObject(),"m_prefix","Ljava/lang/String;");
    javaFieldIds.m_suffix_fid = (env)->GetFieldID((jclass)m_nameDataObject(),"m_suffix","Ljava/lang/String;");
    javaFieldIds.m_phonenumberobjects_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_phoneNumbers","[Lcom/nokia/qt/android/PhoneNumber;");
    javaFieldIds.m_phonenumber_fid = (env)->GetFieldID((jclass)m_phoneNumberObject(),"m_number","Ljava/lang/String;");
    javaFieldIds.m_phonetype_fid = (env)->GetFieldID((jclass)m_phoneNumberObject(),"m_type","I");
    javaFieldIds.m_emailDataObjects_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_email","[Lcom/nokia/qt/android/EmailData;");
    javaFieldIds.m_email_fid = env->GetFieldID((jclass)m_emailDataObject(),"m_email","Ljava/lang/String;");
    javaFieldIds.m_emailtype_fid = (env)->GetFieldID((jclass)m_emailDataObject(),"m_type","I");
    javaFieldIds.m_note_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_contactNote","Ljava/lang/String;");
    javaFieldIds.m_addressDataObject_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_address","[Lcom/nokia/qt/android/AddressData;");
    javaFieldIds.m_pobox_fid = env->GetFieldID((jclass)m_addressDataObject(),"m_pobox","Ljava/lang/String;");
    javaFieldIds.m_street_fid = env->GetFieldID((jclass)m_addressDataObject(),"m_street","Ljava/lang/String;");
    javaFieldIds.m_city_fid = env->GetFieldID((jclass)m_addressDataObject(),"m_city","Ljava/lang/String;");
    javaFieldIds.m_region_fid = env->GetFieldID((jclass)m_addressDataObject(),"m_region","Ljava/lang/String;");
    javaFieldIds.m_postCode_fid = env->GetFieldID((jclass)m_addressDataObject(),"m_postCode","Ljava/lang/String;");
    javaFieldIds.m_country_fid = env->GetFieldID((jclass)m_addressDataObject(),"m_country","Ljava/lang/String;");
    javaFieldIds.m_addresstype_fid = (env)->GetFieldID((jclass)m_addressDataObject(),"m_type","I");
    javaFieldIds.m_organizationalData_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_organizations","[Lcom/nokia/qt/android/OrganizationalData;");
    javaFieldIds.m_organizaion_fid = (env)->GetFieldID((jclass)m_organizationalDataObject(),"m_organization","Ljava/lang/String;");
    javaFieldIds.m_organizaiontitle_fid = (env)->GetFieldID((jclass)m_organizationalDataObject(),"m_title","Ljava/lang/String;");
    javaFieldIds.m_organizationtype_fid = (env)->GetFieldID((jclass)m_organizationalDataObject(),"m_type","I");
    javaFieldIds.m_birthday_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_contactBirthDay","Ljava/lang/String;");
    javaFieldIds.m_anniversary_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_contactAnniversary","Ljava/lang/String;");
    javaFieldIds.m_nickName_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_contactNickName","Ljava/lang/String;");
    javaFieldIds.m_url_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_contactUrls","[Ljava/lang/String;");
    javaFieldIds.m_id_fid = (env)->GetFieldID((jclass)m_myContactsObject(),"m_contactID","Ljava/lang/String;");
    javaFieldIds.m_onlineAccount_fid=(env)->GetFieldID((jclass)m_myContactsObject(),"m_onlineAccount","[Lcom/nokia/qt/android/OnlineAccount;");
    javaFieldIds.m_onlineAccountUri_fid =  env->GetFieldID((jclass)m_onlineAccountObject(),"m_account","Ljava/lang/String;");
    javaFieldIds.m_onlineAccountStatus_fid = env->GetFieldID((jclass)m_onlineAccountObject(),"m_status","Ljava/lang/String;");
    javaFieldIds.m_onlineAccountCustomProtocol_fid = env->GetFieldID((jclass)m_onlineAccountObject(),"m_customProtocol","Ljava/lang/String;");
    javaFieldIds.m_onlineAccountProtocol_fid = env->GetFieldID((jclass)m_onlineAccountObject(),"m_protocol","I");
    javaFieldIds.m_onlineAccountPresnce_fid = env->GetFieldID((jclass)m_onlineAccountObject(),"m_presence","I");
    javaFieldIds.m_onlineAccountTimeStamp_fid = env->GetFieldID((jclass)m_onlineAccountObject(),"m_timeStamp","J");
    javaFieldIds.m_onlineAccountType_fid = env->GetFieldID((jclass)m_onlineAccountObject(),"m_type","I");

    return JNI_TRUE;
}


namespace QtContactsJNI
{
    void updateInAndroidContacts(int id,Contacts& savingcontact)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jobject ret = constructAndroidContactObject(env,savingcontact);
        QString lid;
        lid.setNum(id);
        jstring jid = env->NewString((const jchar *) lid.unicode(), (long) lid.length());
        env->CallVoidMethod(m_qtAndroidContactsObjects(),m_updateContactsID,jid,ret);
         env->DeleteLocalRef(ret);
        m_javaVM->DetachCurrentThread();
    }

    int removeFromAndroidContacts(int id)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return 0;
        }
        QString lid;
        lid.setNum(id);
        jstring jid = env->NewString((const jchar *) lid.unicode(), (long) lid.length());
        int deletionFlag = (int)env->CallIntMethod(m_qtAndroidContactsObjects(),m_removeContactsID,jid);
        m_javaVM->DetachCurrentThread();
        return deletionFlag;
    }

    QString saveToAndroidContacts(Contacts& savingcontact)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return "NULL";
        }
        jobject ret = constructAndroidContactObject(env,savingcontact);
        jstring jid=(jstring)env->CallObjectMethod(m_qtAndroidContactsObjects(),m_saveContactsID,ret);
        const jchar *  _jchar_str_id;
        _jchar_str_id = env->GetStringChars(jid,0);
        QString id;
        id.setUnicode((QChar *) _jchar_str_id, env->GetStringLength(jid));
        env->ReleaseStringChars(jid, _jchar_str_id);
        env->DeleteLocalRef(ret);
        m_javaVM->DetachCurrentThread();
        return id;
    }

    QMap<QContactLocalId,Contacts> initAndroidContacts()
    {
        QMap<QContactLocalId,Contacts> contactsMap;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";

        }
        QString temp_string;
        env->CallVoidMethod(m_qtAndroidContactsObjects(),m_getContactsID);
        jobject contactObjects= env->GetStaticObjectField((jclass)m_qtAndroidContactsObjects(),javaFieldIds.m_androidContacts_fid);
        jobjectArray contactsArray = (jobjectArray)env->GetObjectField(contactObjects,javaFieldIds.m_myContacts_fid);
        jsize count = env->GetArrayLength(contactsArray);

        for(int i=0;i<count;i++)
        {

            Contacts contactInformation;
            jobject contactObject;
            contactObject = env->GetObjectArrayElement(contactsArray,i);

            jstring jdisplayName = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_displayName_fid);
            const jchar *  _jchar_str_displayName;
            _jchar_str_displayName = env->GetStringChars(jdisplayName,0);
            contactInformation.m_displayName.setUnicode((QChar *) _jchar_str_displayName, env->GetStringLength(jdisplayName));
            env->ReleaseStringChars(jdisplayName, _jchar_str_displayName);
            env->DeleteLocalRef(jdisplayName);

            jobject jnameDataObject = (jobject)env->GetObjectField(contactObject,javaFieldIds.m_nameData_fid);
            QtContactsJNI::NameData names;

            jstring jfirstName = (jstring)env->GetObjectField(jnameDataObject,javaFieldIds.m_firstName_fid);
            const jchar *  _jchar_str_firstName;
            _jchar_str_firstName = env->GetStringChars(jfirstName,0);
            names.m_firstName.setUnicode((QChar *) _jchar_str_firstName, env->GetStringLength(jfirstName));
            env->ReleaseStringChars(jfirstName, _jchar_str_firstName);
            env->DeleteLocalRef(jfirstName);

            jstring jlastName = (jstring)env->GetObjectField(jnameDataObject,javaFieldIds.m_lastName_fid);
            const jchar *  _jchar_str_lastName;
            _jchar_str_lastName = env->GetStringChars(jlastName,0);
            names.m_lastName.setUnicode((QChar *) _jchar_str_lastName, env->GetStringLength(jlastName));
            env->ReleaseStringChars(jlastName, _jchar_str_lastName);
            env->DeleteLocalRef(jlastName);

            jstring jmiddleName = (jstring)env->GetObjectField(jnameDataObject,javaFieldIds.m_middleName_fid);
            const jchar *  _jchar_str_middleName;
            _jchar_str_middleName = env->GetStringChars(jmiddleName,0);
            names.m_middleName.setUnicode((QChar *) _jchar_str_middleName, env->GetStringLength(jmiddleName));
            env->ReleaseStringChars(jmiddleName, _jchar_str_middleName);
            env->DeleteLocalRef(jmiddleName);

            jstring jprefix = (jstring)env->GetObjectField(jnameDataObject,javaFieldIds.m_prefix_fid);
            const jchar *  _jchar_str_prefix;
            _jchar_str_prefix = env->GetStringChars(jprefix,0);
            names.m_prefix.setUnicode((QChar *) _jchar_str_prefix, env->GetStringLength(jprefix));
            env->ReleaseStringChars(jprefix, _jchar_str_prefix);
            env->DeleteLocalRef(jprefix);

            jstring jsuffix = (jstring)env->GetObjectField(jnameDataObject,javaFieldIds.m_suffix_fid);
            const jchar *  _jchar_str_suffix;
            _jchar_str_suffix = env->GetStringChars(jsuffix,0);
            names.m_suffix.setUnicode((QChar *) _jchar_str_suffix, env->GetStringLength(jsuffix));
            env->ReleaseStringChars(jsuffix, _jchar_str_suffix);
            env->DeleteLocalRef(jsuffix);
            contactInformation.m_names = names;
            env->DeleteLocalRef(jnameDataObject);

            jobjectArray phoneNumbersArray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_phonenumberobjects_fid);
            jsize phoneNumberCount = env->GetArrayLength(phoneNumbersArray);
            for(int j=0;j<phoneNumberCount;j++)
            {
                jobject jphoneNumberObject = (jobject)env->GetObjectArrayElement(phoneNumbersArray,j);
                jstring jphoneNumberString = (jstring)env->GetObjectField(jphoneNumberObject,javaFieldIds.m_phonenumber_fid);
                jint jtype = (jint)env->GetObjectField(jphoneNumberObject,javaFieldIds.m_phonetype_fid);
                const jchar *  _jchar_str_number;
                _jchar_str_number = env->GetStringChars(jphoneNumberString,0);
                temp_string.setUnicode((QChar *) _jchar_str_number, env->GetStringLength(jphoneNumberString));
                QtContactsJNI::PhoneNumber phoneNumberInfo;
                phoneNumberInfo.m_number = temp_string;
                phoneNumberInfo.m_type = (int)jtype;
                contactInformation.m_phonenumber.append(phoneNumberInfo);
                env->ReleaseStringChars(jphoneNumberString, _jchar_str_number);
                env->DeleteLocalRef(jphoneNumberString);
                env->DeleteLocalRef(jphoneNumberObject);
            }
            env->DeleteLocalRef(phoneNumbersArray);

            jobjectArray jemailsArray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_emailDataObjects_fid);
            jsize emailCount = env->GetArrayLength(jemailsArray);

            for(int j=0;j<emailCount;j++)
            {
                jobject jemailObject = (jobject)env->GetObjectArrayElement(jemailsArray,j);
                jstring jemailString = (jstring)env->GetObjectField(jemailObject,javaFieldIds.m_email_fid);
                jint jtype = (jint)env->GetObjectField(jemailObject,javaFieldIds.m_emailtype_fid);
                const jchar *  _jchar_str_email;
                _jchar_str_email = env->GetStringChars(jemailString,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_email, env->GetStringLength(jemailString));
                QtContactsJNI::EmailData emailInfo;
                emailInfo.m_email = temp_string;
                emailInfo.m_type = (int)jtype;
                contactInformation.m_email.append(emailInfo);
                env->ReleaseStringChars(jemailString, _jchar_str_email);
                env->DeleteLocalRef(jemailString);
                env->DeleteLocalRef(jemailObject);

            }
            env->DeleteLocalRef(jemailsArray);

            jstring jnote = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_note_fid);
            const jchar *  _jchar_str_note;
            _jchar_str_note = env->GetStringChars(jnote,0);
            contactInformation.m_note.setUnicode((QChar *) _jchar_str_note, env->GetStringLength(jnote));
            env->ReleaseStringChars(jnote, _jchar_str_note);
            env->DeleteLocalRef(jnote);

            jobjectArray addressArray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_addressDataObject_fid);
            jsize addrCount = env->GetArrayLength(addressArray);
            for(int j=0;j<addrCount;j++)
            {
                jobject jaddressObject = (jobject)env->GetObjectArrayElement(addressArray,j);
                jstring jpobox = (jstring)env->GetObjectField(jaddressObject,javaFieldIds.m_pobox_fid);
                const jchar *  _jchar_str_pobox;
                _jchar_str_pobox = env->GetStringChars(jpobox,0);
                QtContactsJNI::AddressData addressInfo;
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_pobox, env->GetStringLength(jpobox));
                addressInfo.m_pobox = temp_string;
                env->ReleaseStringChars(jpobox, _jchar_str_pobox);
                env->DeleteLocalRef(jpobox);

                jstring jstreet = (jstring)env->GetObjectField(jaddressObject,javaFieldIds.m_street_fid);
                const jchar *  _jchar_str_street;
                _jchar_str_street = env->GetStringChars(jstreet,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_street, env->GetStringLength(jstreet));
                addressInfo.m_street = temp_string;
                env->ReleaseStringChars(jstreet, _jchar_str_street);
                env->DeleteLocalRef(jstreet);

                jstring jcity = (jstring)env->GetObjectField(jaddressObject,javaFieldIds.m_city_fid);
                const jchar *  _jchar_str_city;
                _jchar_str_city = env->GetStringChars(jcity,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_city, env->GetStringLength(jcity));
                addressInfo.m_city = temp_string;
                env->ReleaseStringChars(jcity, _jchar_str_city);
                env->DeleteLocalRef(jcity);

                jstring jregion = (jstring)env->GetObjectField(jaddressObject,javaFieldIds.m_region_fid);
                const jchar *  _jchar_str_region;
                _jchar_str_region = env->GetStringChars(jregion,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_region, env->GetStringLength(jregion));
                addressInfo.m_region = temp_string;
                env->ReleaseStringChars(jregion, _jchar_str_region);
                env->DeleteLocalRef(jregion);

                jstring jpostCode = (jstring)env->GetObjectField(jaddressObject,javaFieldIds.m_postCode_fid);
                const jchar *  _jchar_str_postCode;
                _jchar_str_postCode = env->GetStringChars(jpostCode,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_postCode, env->GetStringLength(jpostCode));
                addressInfo.m_postCode = temp_string;
                env->ReleaseStringChars(jpostCode, _jchar_str_postCode);
                env->DeleteLocalRef(jpostCode);

                jstring jcountry = (jstring)env->GetObjectField(jaddressObject,javaFieldIds.m_country_fid);
                const jchar *  _jchar_str_country;
                _jchar_str_country = env->GetStringChars(jcountry,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_country, env->GetStringLength(jcountry));
                addressInfo.m_country = temp_string;
                env->ReleaseStringChars(jcountry, _jchar_str_country);
                env->DeleteLocalRef(jcountry);

                jint jtype = (jint)env->GetObjectField(jaddressObject,javaFieldIds.m_addresstype_fid);
                addressInfo.m_type = (int)jtype;
                contactInformation.m_address.append(addressInfo);

            }
            env->DeleteLocalRef(addressArray);

            jobjectArray onlineAccountArray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_onlineAccount_fid);
            jint onlineAccountCount = env->GetArrayLength(onlineAccountArray);

            for(int j=0;j<onlineAccountCount;j++)
            {
                jobject jonlineAccount = (jobject)env->GetObjectArrayElement(onlineAccountArray,j);

                jstring jaccountUri = (jstring)env->GetObjectField(jonlineAccount,javaFieldIds.m_onlineAccountUri_fid);
                const jchar* _jchar_str_accountUri;
                _jchar_str_accountUri = env->GetStringChars(jaccountUri,0);
                QtContactsJNI::OnlineAccount onlineAccountInfo;
                temp_string.clear();
                temp_string.setUnicode((QChar *)_jchar_str_accountUri,env->GetStringLength(jaccountUri));
                onlineAccountInfo.m_acoountUri = temp_string;
                env->ReleaseStringChars(jaccountUri, _jchar_str_accountUri);
                env->DeleteLocalRef(jaccountUri);

                jstring jaccountStatus = (jstring)env->GetObjectField(jonlineAccount,javaFieldIds.m_onlineAccountStatus_fid);
                const jchar* _jchar_str_accountStatus;
                _jchar_str_accountStatus = env->GetStringChars(jaccountStatus,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *)_jchar_str_accountStatus,env->GetStringLength(jaccountStatus));
                onlineAccountInfo.m_status = temp_string;
                env->ReleaseStringChars(jaccountStatus, _jchar_str_accountStatus);
                env->DeleteLocalRef(jaccountStatus);

                onlineAccountInfo.m_presence = (int)env->GetIntField(jonlineAccount,javaFieldIds.m_onlineAccountPresnce_fid);
                onlineAccountInfo.m_timeStamp = (long)env->GetLongField(jonlineAccount,javaFieldIds.m_onlineAccountTimeStamp_fid);
                onlineAccountInfo.m_type = (int)env->GetIntField(jonlineAccount,javaFieldIds.m_onlineAccountType_fid);
                onlineAccountInfo.m_protocol = (int)env->GetIntField(jonlineAccount,javaFieldIds.m_onlineAccountProtocol_fid);
                if(onlineAccountInfo.m_protocol == -1)
                {
                    jstring jaccountCustomProtocol = (jstring)env->GetObjectField(jonlineAccount,javaFieldIds.m_onlineAccountCustomProtocol_fid);
                    const jchar* _jchar_str_accountCustomProtocol;
                    _jchar_str_accountCustomProtocol = env->GetStringChars(jaccountCustomProtocol,0);
                    temp_string.clear();
                    temp_string.setUnicode((QChar *)_jchar_str_accountCustomProtocol,env->GetStringLength(jaccountCustomProtocol));
                    onlineAccountInfo.m_customProtocol = temp_string;
                    env->ReleaseStringChars(jaccountCustomProtocol, _jchar_str_accountCustomProtocol);
                    env->DeleteLocalRef(jaccountCustomProtocol);
                }
                contactInformation.m_onlineAccount.append(onlineAccountInfo);

            }
            env->DeleteLocalRef(onlineAccountArray);

            jobjectArray orgArray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_organizationalData_fid);
            jsize orgCount = env->GetArrayLength(orgArray);

            for(int j=0;j<orgCount;j++)
            {

                jobject jorganizationData = (jobject)env->GetObjectArrayElement(orgArray,j);

                jstring jorganization = (jstring)env->GetObjectField(jorganizationData,javaFieldIds.m_organizaion_fid);
                const jchar *  _jchar_str_organization;
                _jchar_str_organization = env->GetStringChars(jorganization,0);
                QtContactsJNI::OrganizationalData orginfo;
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_organization, env->GetStringLength(jorganization));
                orginfo.m_organization = temp_string;
                env->ReleaseStringChars(jorganization, _jchar_str_organization);
                env->DeleteLocalRef(jorganization);

                jstring jorganizationTitle = (jstring)env->GetObjectField(jorganizationData,javaFieldIds.m_organizaiontitle_fid);
                const jchar *  _jchar_str_organizationtitle;
                _jchar_str_organizationtitle = env->GetStringChars(jorganizationTitle,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_organizationtitle, env->GetStringLength(jorganizationTitle));
                orginfo.m_title = temp_string;
                env->ReleaseStringChars(jorganizationTitle, _jchar_str_organizationtitle);
                env->DeleteLocalRef(jorganizationTitle);

                jint jtype = (jint)env->GetObjectField(jorganizationData,javaFieldIds.m_organizationtype_fid);
                orginfo.m_type = (int)jtype;
                contactInformation.m_organization.append(orginfo);
                env->DeleteLocalRef(jorganizationData);

            }
            env->DeleteLocalRef(orgArray);

            jstring jbirthday = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_birthday_fid);
            const jchar *  _jchar_str_birthday;
            _jchar_str_birthday = env->GetStringChars(jbirthday,0);
            contactInformation.m_birthday.setUnicode((QChar *) _jchar_str_birthday, env->GetStringLength(jbirthday));
            env->ReleaseStringChars(jbirthday, _jchar_str_birthday);
            env->DeleteLocalRef(jbirthday);

            jstring janniversary = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_anniversary_fid);
            const jchar *  _jchar_str_anniversary;
            _jchar_str_anniversary = env->GetStringChars(janniversary,0);
            contactInformation.m_anniverasary.setUnicode((QChar *) _jchar_str_anniversary, env->GetStringLength(janniversary));
            env->ReleaseStringChars(janniversary, _jchar_str_anniversary);
            env->DeleteLocalRef(janniversary);

            jstring jnickName = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_nickName_fid);
            const jchar *  _jchar_str_nickName;
            _jchar_str_nickName = env->GetStringChars(jnickName,0);
            contactInformation.m_nickName.setUnicode((QChar *) _jchar_str_nickName, env->GetStringLength(jnickName));
            env->ReleaseStringChars(jnickName, _jchar_str_nickName);
            env->DeleteLocalRef(jnickName);
            jobjectArray urlArray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_url_fid);
            jsize urlCount = env->GetArrayLength(urlArray);

            for(int j=0;j<urlCount;j++)
            {
                jstring jurl = (jstring)env->GetObjectArrayElement(urlArray,j);
                const jchar *  _jchar_str_url;
                _jchar_str_url = env->GetStringChars(jurl,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_url, env->GetStringLength(jurl));
                contactInformation.m_url.append(temp_string);
                env->ReleaseStringChars(jurl, _jchar_str_url);
                env->DeleteLocalRef(jurl);

            }
            env->DeleteLocalRef(urlArray);

            jstring jid = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_id_fid);
            const jchar *  _jchar_str_id;
            _jchar_str_id = env->GetStringChars(jid,0);
            contactInformation.m_id.setUnicode((QChar *) _jchar_str_id, env->GetStringLength(jid));
            env->ReleaseStringChars(jid, _jchar_str_id);
            env->DeleteLocalRef(jid);
            env->DeleteLocalRef(contactObject);
            contactsMap.insert(contactInformation.m_id.toInt(0,10),contactInformation);
        }
        env->DeleteLocalRef(contactsArray);
        env->DeleteLocalRef(contactObjects);
        m_javaVM->DetachCurrentThread();
        return contactsMap;
    }

    jobject constructAndroidContactObject(JNIEnv* env,Contacts& savingcontact)
    {
        jstring jfirstName;
        if(!savingcontact.m_names.m_firstName.isEmpty())
        {
            jfirstName = env->NewStringUTF(savingcontact.m_names.m_firstName.toStdString().c_str());
        }
        else
        {
            jfirstName =  env->NewStringUTF("");
        }
        jstring jlastName;
        if(!savingcontact.m_names.m_lastName.isEmpty())
        {
            jlastName = env->NewStringUTF(savingcontact.m_names.m_lastName.toStdString().c_str());
        }
        else
        {
            jlastName =  env->NewStringUTF("");
        }
        jstring jmiddleName;
        if(!savingcontact.m_names.m_middleName.isEmpty())
        {
            jmiddleName = env->NewStringUTF(savingcontact.m_names.m_middleName.toStdString().c_str());
        }
        else
        {
            jmiddleName =  env->NewStringUTF("");
        }
        jstring jprefix;
        if(!savingcontact.m_names.m_prefix.isEmpty())
        {
            jprefix = env->NewStringUTF(savingcontact.m_names.m_prefix.toStdString().c_str());
        }
        else
        {
            jprefix =  env->NewStringUTF("");
        }
        jstring jsuffix;
        if(!savingcontact.m_names.m_suffix.isEmpty())
        {
            jsuffix = env->NewStringUTF(savingcontact.m_names.m_suffix.toStdString().c_str());
        }
        else
        {
            jsuffix =  env->NewStringUTF("");
        }
        jobject nameObject = env->NewObject((jclass)m_nameDataObject(),m_nameDataConstructor,jfirstName,jlastName,jmiddleName,jprefix,jsuffix);
        env->DeleteLocalRef(jfirstName);
        env->DeleteLocalRef(jlastName);
        env->DeleteLocalRef(jmiddleName);
        env->DeleteLocalRef(jprefix);
        env->DeleteLocalRef(jsuffix);

        jobjectArray phoneNumbersArray;
        if(!savingcontact.m_phonenumber.isEmpty())
        {
            jint numberCount = savingcontact.m_phonenumber.count();
            phoneNumbersArray=(jobjectArray)env->NewObjectArray(numberCount,(jclass)m_phoneNumberObject(),NULL);
            for(int i=0;i<numberCount;i++)
            {
                jstring jNumber = env->NewStringUTF(savingcontact.m_phonenumber[i].m_number.toStdString().c_str());
                jint jType = savingcontact.m_phonenumber[i].m_type;
                jobject jphoneNumberObject =env->NewObject((jclass)m_phoneNumberObject(),m_phoneNumberConstructor,jNumber,jType);
                env->DeleteLocalRef(jNumber);
                env->SetObjectArrayElement(phoneNumbersArray,i,jphoneNumberObject);
            }
        }
        else
        {
            phoneNumbersArray=(jobjectArray)env->NewObjectArray(1,(jclass)m_phoneNumberObject(),NULL);
            jstring jNumber =  env->NewStringUTF("");
            jobject jphoneNumberObject =env->NewObject((jclass)m_phoneNumberObject(),m_phoneNumberConstructor,jNumber,0);
            env->DeleteLocalRef(jNumber);
            env->SetObjectArrayElement(phoneNumbersArray,0,jphoneNumberObject);
        }

        jobjectArray emailArray;
        if(!savingcontact.m_email.isEmpty())
        {
            jint emailCount = savingcontact.m_email.count();
            emailArray= (jobjectArray)env->NewObjectArray(emailCount,(jclass)m_emailDataObject(),NULL);
            for(int i=0;i<emailCount;i++)
            {
                jstring jEmail = env->NewStringUTF(savingcontact.m_email[i].m_email.toStdString().c_str());
                jint jType = savingcontact.m_email[i].m_type;
                jobject jEmailDataObject =env->NewObject((jclass)m_emailDataObject(),m_emailDataConstructor,jEmail,jType);
                env->DeleteLocalRef(jEmail);
                env->SetObjectArrayElement(emailArray,i,jEmailDataObject);
            }
        }
        else
        {
            emailArray= (jobjectArray)env->NewObjectArray(1,(jclass)m_emailDataObject(),NULL);
            jstring jEmail =  env->NewStringUTF("");
            jobject jEmailDataObject =env->NewObject((jclass)m_emailDataObject(),m_emailDataConstructor,jEmail,0);
            env->DeleteLocalRef(jEmail);
            env->SetObjectArrayElement(emailArray,0,jEmailDataObject);
        }

        jobjectArray addrArray;
        jint addrCount = savingcontact.m_address.count();
        addrArray= (jobjectArray)env->NewObjectArray(addrCount,(jclass)m_addressDataObject(),NULL);
        for(int i=0;i<addrCount;i++)
        {
            jstring jpobox;
            if(!savingcontact.m_address[i].m_pobox.isEmpty())
            {
                jpobox = env->NewStringUTF(savingcontact.m_address[i].m_pobox.toStdString().c_str());
            }
            else
            {
                jpobox = env->NewStringUTF("");
            }
            jstring jstreet;
            if(!savingcontact.m_address[i].m_street.isEmpty())
            {
                jstreet = env->NewStringUTF(savingcontact.m_address[i].m_street.toStdString().c_str());
            }
            else
            {
                jstreet = env->NewStringUTF("");
            }
            jstring jcity;
            if(!savingcontact.m_address[i].m_city.isEmpty())
            {
                jcity = env->NewStringUTF(savingcontact.m_address[i].m_city.toStdString().c_str());
            }
            else
            {
                jcity = env->NewStringUTF("");
            }
            jstring jregion;
            if(!savingcontact.m_address[i].m_region.isEmpty())
            {
                jregion = env->NewStringUTF(savingcontact.m_address[i].m_region.toStdString().c_str());
            }
            else
            {
                jregion = env->NewStringUTF("");
            }
            jstring jpostCode;
            if(!savingcontact.m_address[i].m_postCode.isEmpty())
            {
                jpostCode = env->NewStringUTF(savingcontact.m_address[i].m_postCode.toStdString().c_str());
            }
            else
            {
                jpostCode = env->NewStringUTF("");
            }
            jstring jcountry;
            if(!savingcontact.m_address[i].m_country.isEmpty())
            {
                jcountry = env->NewStringUTF(savingcontact.m_address[i].m_country.toStdString().c_str());
            }
            else
            {
                jcountry = env->NewStringUTF("");
            }
            jint jType = savingcontact.m_address[i].m_type;
            jobject jAddressDataObject =env->NewObject((jclass)m_addressDataObject(),m_AddressDataConstructor,jpobox,jstreet,jcity,jregion,jpostCode,jcountry,jType);
            env->DeleteLocalRef(jpobox);
            env->DeleteLocalRef(jstreet);
            env->DeleteLocalRef(jcity);
            env->DeleteLocalRef(jregion);
            env->DeleteLocalRef(jpostCode);
            env->DeleteLocalRef(jcountry);
            env->SetObjectArrayElement(addrArray,i,jAddressDataObject);
        }

        jobjectArray orgArray;
        jint orgCount = savingcontact.m_organization.count();
        orgArray= (jobjectArray)env->NewObjectArray(orgCount,(jclass)m_organizationalDataObject(),NULL);
        for(int i=0;i<orgCount;i++)
        {
            jstring jOrganization;
            if(!savingcontact.m_organization[i].m_organization.isEmpty())
            {
                jOrganization = env->NewStringUTF(savingcontact.m_organization[i].m_organization.toStdString().c_str());
            }
            else
            {
                jOrganization =  env->NewStringUTF("");
            }
            jstring jOrganizationTitle;
            if(!savingcontact.m_organization[i].m_title.isEmpty())
            {
                jOrganizationTitle = env->NewStringUTF(savingcontact.m_organization[i].m_title.toStdString().c_str());
            }
            else
            {
                jOrganizationTitle= env->NewStringUTF("");
            }
            jint jType = savingcontact.m_organization[i].m_type;
            jobject jOrganizationDataObject =env->NewObject((jclass)m_organizationalDataObject(),m_organizationalDataConstructor,jOrganization,jOrganizationTitle,jType);
            env->DeleteLocalRef(jOrganization);
            env->DeleteLocalRef(jOrganizationTitle);
            env->SetObjectArrayElement(orgArray,i,jOrganizationDataObject);
        }
        jobjectArray urlsArray;
        if(!savingcontact.m_url.isEmpty())
        {
            jint urlCount = savingcontact.m_url.count();
            urlsArray= (jobjectArray)env->NewObjectArray(urlCount,env->FindClass("java/lang/String"),NULL);
            for(int i=0;i<urlCount;i++)
            {
                env->SetObjectArrayElement(urlsArray,i,env->NewStringUTF(savingcontact.m_url[i].toStdString().c_str()));
            }
        }
        else
        {
            urlsArray= (jobjectArray)env->NewObjectArray(1,env->FindClass("java/lang/String"),NULL);
            env->SetObjectArrayElement(urlsArray,0,env->NewStringUTF(""));
        }
        jstring jNote;
        if(!savingcontact.m_note.isEmpty())
        {
            jNote = env->NewStringUTF(savingcontact.m_note.toStdString().c_str());
        }
        else
        {
            jNote =  env->NewStringUTF("");
        }
        jstring jBirthday;
        if(!savingcontact.m_birthday.isEmpty())
        {
            jBirthday = env->NewStringUTF(savingcontact.m_birthday.toStdString().c_str());
        }
        else
        {
            jBirthday =  env->NewStringUTF("");
        }
        jstring jAnniversary;
        if(!savingcontact.m_anniverasary.isEmpty())
        {
            jAnniversary = env->NewStringUTF(savingcontact.m_anniverasary.toStdString().c_str());
        }
        else
        {
            jAnniversary=  env->NewStringUTF("");
        }
        jstring jNickName;
        if(!savingcontact.m_nickName.isEmpty())
        {
            jNickName = env->NewStringUTF(savingcontact.m_nickName.toStdString().c_str());
        }
        else
        {
            jNickName =  env->NewStringUTF("");
        }
        jobject ret = env->NewObject((jclass)m_myContactsObject(),m_myContactsConstructor,nameObject,phoneNumbersArray,emailArray,jNote,addrArray,orgArray,jBirthday,jAnniversary,jNickName,urlsArray);
        env->DeleteLocalRef(jNote);
        env->DeleteLocalRef(jBirthday);
        env->DeleteLocalRef(jAnniversary);
        env->DeleteLocalRef(jNickName);
        env->DeleteLocalRef(nameObject);
        env->DeleteLocalRef(phoneNumbersArray);
        env->DeleteLocalRef(emailArray);
        env->DeleteLocalRef(addrArray);
        env->DeleteLocalRef(orgArray);
        env->DeleteLocalRef(urlsArray);
        return ret;
    }
};

static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env))
        return JNI_FALSE;

    return JNI_TRUE;
}

typedef union {
    JNIEnv* nativeEnvironment;
    void* venv;
} UnionJNIEnvToVoid;

Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    __android_log_print(ANDROID_LOG_INFO,"Qt", "qt conatct jni start");
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    m_javaVM = 0;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }
    m_env = uenv.nativeEnvironment;
    if (!registerNatives(m_env))
    {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "registerNatives failed");
        return -1;
    }
    m_javaVM = vm;
    return JNI_VERSION_1_4;
}


