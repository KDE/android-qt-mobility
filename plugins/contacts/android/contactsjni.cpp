//JNI file to Interact With Java
#include <android/log.h>
#include<QtCore>
#include<jni.h>

#include "qcontactandroidbackend_p.h"
#include"qcontactabook_p.h"
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
            m_value=m_env->NewGlobalRef(value);
        else
            m_value=value;
    };
    virtual ~JavaContactsGlobalObject()
    {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
    };
    T& operator =(const T& value)
                 {
        if (m_value && m_env)
            m_env->DeleteGlobalRef(m_value);
        if (m_env && value)
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
static jmethodID m_getContactsID=0;

static JavaContactsGlobalObject<jobject> m_qtAndroidContactsObjects = 0;
static JavaContactsGlobalObject<jobject> m_myContactsObject=0;
static JavaContactsGlobalObject<jobject> m_allContactsObject=0;
static JavaContactsGlobalObject<jobject> m_phoneNumberObject=0;
static JavaContactsGlobalObject<jobject> m_emailDataObject=0;
static JavaContactsGlobalObject<jobject> m_nameDataObject=0;
static JavaContactsGlobalObject<jobject> m_addressDataObject=0;
static JavaContactsGlobalObject<jobject> m_organizationalDataObject=0;
static JavaContactsGlobalObject<jobject> m_stringObject=0;
static const char *QtAndroidContactsClassPathName = "com/nokia/qt/android/QtAndroidContacts";
static const char *QtMyContactsClassPathName = "com/nokia/qt/android/MyContacts";
static const char *QtAllContactsClassPathName = "com/nokia/qt/android/AllContacts";
static const char *QtPhoneNumberClassPathName = "com/nokia/qt/android/PhoneNumber";
static const char *QtEmailDataClassPathName = "com/nokia/qt/android/EmailData";
static const char *QtAddressDataClassPathName = "com/nokia/qt/android/AddressData";
static const char *QtNameDataClassPathName = "com/nokia/qt/android/NameData";
static const char *QtOrganizationalDataClassPathName = "com/nokia/qt/android/OrganizationalData";
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
    allContactsClass = env->FindClass(QtAllContactsClassPathName);
    if (allContactsClass == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtAllContactsClassPathName);
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
    return true;
}


static int registerNativeMethods(JNIEnv* env,const char* className)
{
    if (!setQtApplicationObject(env))
    {
        QANDROID_DEBUG<<"Failed In registering Native MEthods";
        return JNI_FALSE;
    }
    // Registering java methods
    m_getContactsID = env->GetMethodID((jclass)m_qtAndroidContactsObjects(),"getQtContacts","()V");
    // Getting all required fieldIDs from Java
    javaFieldIds.m_allContacts_fid = env->GetStaticFieldID((jclass)m_qtAndroidContactsObjects(),"m_androidContacts","Lcom/nokia/qt/android/AllContacts;");
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

    return JNI_TRUE;
}


namespace QtContactsJNI
{
    QMap<QContactLocalId,MyContacts> initAndroidContacts()
    {
        QMap<QContactLocalId,MyContacts> contactsMap;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";

        }
        QString temp_string;
        env->CallVoidMethod(m_qtAndroidContactsObjects(),m_getContactsID);
        jobject contactObjects= env->GetStaticObjectField((jclass)m_qtAndroidContactsObjects(),javaFieldIds.m_allContacts_fid);
        jobjectArray contactsarray = (jobjectArray)env->GetObjectField(contactObjects,javaFieldIds.m_myContacts_fid);
        jsize count = env->GetArrayLength(contactsarray);

        for(int i=0;i<count;i++)
        {
            MyContacts contactInformation;
            jobject contactObject;
            contactObject = env->GetObjectArrayElement(contactsarray,i);
            jstring jname = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_displayName_fid);
            const jchar *  _jchar_str_name;
            _jchar_str_name = env->GetStringChars(jname,0);
            contactInformation.m_displayName.setUnicode((QChar *) _jchar_str_name, env->GetStringLength(jname));
            env->ReleaseStringChars(jname, _jchar_str_name);
            env->DeleteLocalRef(jname);
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
            jobjectArray orgsarray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_organizationalData_fid);
            jsize orgcount = env->GetArrayLength(orgsarray);

            for(int j=0;j<orgcount;j++)
            {

                jobject jOrganizationData = (jobject)env->GetObjectArrayElement(orgsarray,j);
                jstring jOrganization = (jstring)env->GetObjectField(jOrganizationData,javaFieldIds.m_organizaion_fid);
                const jchar *  _jchar_str_organization;
                _jchar_str_organization = env->GetStringChars(jOrganization,0);
                QtContactsJNI::OrganizationalData orginfo;
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_organization, env->GetStringLength(jOrganization));
                orginfo.m_organization = temp_string;
                env->ReleaseStringChars(jOrganization, _jchar_str_organization);
                env->DeleteLocalRef(jOrganization);
                jstring jOrganizationTitle = (jstring)env->GetObjectField(jOrganizationData,javaFieldIds.m_organizaiontitle_fid);
                const jchar *  _jchar_str_organizationtitle;
                _jchar_str_organizationtitle = env->GetStringChars(jOrganizationTitle,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_organizationtitle, env->GetStringLength(jOrganizationTitle));
                orginfo.m_title = temp_string;
                env->ReleaseStringChars(jOrganizationTitle, _jchar_str_organizationtitle);
                env->DeleteLocalRef(jOrganizationTitle);
                jint jtype = (jint)env->GetObjectField(jOrganizationData,javaFieldIds.m_organizationtype_fid);
                orginfo.m_type = (int)jtype;
                contactInformation.m_organization.append(orginfo);
                env->DeleteLocalRef(jOrganizationData);

            }
            env->DeleteLocalRef(orgsarray);
            jstring jbir = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_birthday_fid);
            const jchar *  _jchar_str_bir;
            _jchar_str_bir = env->GetStringChars(jbir,0);
            contactInformation.m_birthday.setUnicode((QChar *) _jchar_str_bir, env->GetStringLength(jbir));
            env->ReleaseStringChars(jbir, _jchar_str_bir);
            env->DeleteLocalRef(jbir);
            jstring janni = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_anniversary_fid);
            const jchar *  _jchar_str_anni;
            _jchar_str_anni = env->GetStringChars(janni,0);
            contactInformation.m_anniverasay.setUnicode((QChar *) _jchar_str_anni, env->GetStringLength(janni));
            env->ReleaseStringChars(janni, _jchar_str_anni);
            env->DeleteLocalRef(janni);
            jstring jnick = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_nickName_fid);
            const jchar *  _jchar_str_nick;
            _jchar_str_nick = env->GetStringChars(jnick,0);
            contactInformation.m_nickName.setUnicode((QChar *) _jchar_str_nick, env->GetStringLength(jnick));
            env->ReleaseStringChars(jnick, _jchar_str_nick);
            env->DeleteLocalRef(jnick);
            jobjectArray urlsarray = (jobjectArray)env->GetObjectField(contactObject,javaFieldIds.m_url_fid);
            jsize urlcount = env->GetArrayLength(urlsarray);

            for(int j=0;j<urlcount;j++)
            {
                jstring jurlstring = (jstring)env->GetObjectArrayElement(urlsarray,j);
                const jchar *  _jchar_str_url;
                _jchar_str_url = env->GetStringChars(jurlstring,0);
                temp_string.clear();
                temp_string.setUnicode((QChar *) _jchar_str_url, env->GetStringLength(jurlstring));
                contactInformation.m_url.append(temp_string);
                env->ReleaseStringChars(jurlstring, _jchar_str_url);
                env->DeleteLocalRef(jurlstring);

            }
            env->DeleteLocalRef(urlsarray);
            jstring jid = (jstring)env->GetObjectField(contactObject,javaFieldIds.m_id_fid);
            const jchar *  _jchar_str_id;
            _jchar_str_id = env->GetStringChars(jid,0);
            contactInformation.m_id.setUnicode((QChar *) _jchar_str_id, env->GetStringLength(jid));
            env->ReleaseStringChars(jid, _jchar_str_id);
            env->DeleteLocalRef(jid);
            env->DeleteLocalRef(contactObject);
            contactsMap.insert(contactInformation.m_id.toInt(0,10),contactInformation);
        }
        env->DeleteLocalRef(contactsarray);
        env->DeleteLocalRef(contactObjects);
        m_javaVM->DetachCurrentThread();
        return contactsMap;
    }

};
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, QtAndroidContactsClassPathName))
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

