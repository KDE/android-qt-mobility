TEMPLATE = lib
TARGET = QtMessaging
include(../../common.pri)
INCLUDEPATH += .

# Input
DEFINES += QT_BUILD_MESSAGING_LIB \
    QT_MAKEDLL
PUBLIC_HEADERS += qmessageid.h \
    qmessagecontentcontainerid.h \
    qmessagefolderid.h \
    qmessageaccountid.h \
    qmessagecontentcontainer.h \
    qmessageaddress.h \
    qmessage.h \
    qmessagefolder.h \
    qmessageaccount.h \
    qmessageaccountfilter.h \
    qmessageaccountsortorder.h \
    qmessagefolderfilter.h \
    qmessagefoldersortorder.h \
    qmessagefilter.h \
    qmessagemanager.h \
    qmessagesortorder.h \
    qmessageservice.h \
    qmessagedatacomparator.h \
    qmessageglobal.h
PRIVATE_HEADERS += addresshelper_p.h \
    qmessageid_p.h \
    qmessagecontentcontainerid_p.h \
    qmessagefolderid_p.h \
    qmessageaccountid_p.h \
    qmessagecontentcontainer_p.h \
    qmessageaddress_p.h \
    qmessage_p.h \
    qmessagefolder_p.h \
    qmessageaccount_p.h \
    qmessageaccountfilter_p.h \
    qmessageaccountsortorder_p.h \
    qmessagefolderfilter_p.h \
    qmessagefoldersortorder_p.h \
    qmessagefilter_p.h \
    qmessagesortorder_p.h \
    qmessagestore.h \
    messagingutil_p.h \
    qmessagestore_p.h
SOURCES += qmessageid.cpp \
    qmessagecontentcontainerid.cpp \
    qmessagefolderid.cpp \
    qmessageaccountid.cpp \
    qmessagecontentcontainer.cpp \
    addresshelper.cpp \
    qmessageaddress.cpp \
    qmessage.cpp \
    qmessagefolder.cpp \
    qmessageaccount.cpp \
    qmessageaccountfilter.cpp \
    qmessageaccountsortorder.cpp \
    qmessagefolderfilter.cpp \
    qmessagefoldersortorder.cpp \
    qmessagefilter.cpp \
    qmessagemanager.cpp \
    qmessagesortorder.cpp \
    qmessagestore.cpp \
    qmessageservice.cpp \
    messagingutil.cpp
symbian|win32|maemo6|maemo5|mac:!simulator {
    mac: SOURCES += qmessageid_stub.cpp \
        qmessagecontentcontainerid_stub.cpp \
        qmessagefolderid_stub.cpp \
        qmessageaccountid_stub.cpp \
        qmessagecontentcontainer_stub.cpp \
        qmessage_stub.cpp \
        qmessagefolder_stub.cpp \
        qmessageaccount_stub.cpp \
        qmessageaccountfilter_stub.cpp \
        qmessageaccountsortorder_stub.cpp \
        qmessagefolderfilter_stub.cpp \
        qmessagefoldersortorder_stub.cpp \
        qmessagefilter_stub.cpp \
        qmessagesortorder_stub.cpp \
        qmessagestore_stub.cpp \
        qmessageservice_stub.cpp
    maemo5 {
        QT += dbus
        CONFIG += link_pkgconfig
        PUBLIC_HEADERS -= qmessagecontentcontainer_p.h
        PRIVATE_HEADERS -= qmessagecontentcontainer_p.h
        HEADERS += qmessagecontentcontainer_maemo_p.h \
            qmessageservice_maemo_p.h \
            modestengine_maemo_p.h \
            telepathyengine_maemo_p.h \
            maemohelpers_p.h \
            eventloggerengine_maemo_p.h
        SOURCES += qmessageid_maemo.cpp \
            qmessagecontentcontainerid_maemo.cpp \
            qmessagefolderid_maemo.cpp \
            qmessageaccountid_maemo.cpp \
            qmessagecontentcontainer_maemo.cpp \
            qmessage_maemo.cpp \
            qmessagefolder_maemo.cpp \
            qmessageaccount_maemo.cpp \
            qmessageaccountfilter_maemo.cpp \
            qmessageaccountsortorder_maemo.cpp \
            qmessagefolderfilter_maemo.cpp \
            qmessagefoldersortorder_maemo.cpp \
            qmessagefilter_maemo.cpp \
            qmessagesortorder_maemo.cpp \
            qmessagestore_maemo.cpp \
            qmessageservice_maemo.cpp \
            modestengine_maemo.cpp \
            telepathyengine_maemo.cpp \
            maemohelpers.cpp \
            eventloggerengine_maemo.cpp
        documentation.path = $$QT_MOBILITY_PREFIX/doc
        documentation.files = doc/html
        PKGCONFIG += glib-2.0 \
            dbus-glib-1 \
            gconf-2.0 \
            libosso \
            libmodest-dbus-client-1.0 \
            TpSession \
            TelepathyQt4
        CONFIG += create_pc \
            create_prl
        QMAKE_PKGCONFIG_REQUIRES = glib-2.0 \
            dbus-glib-1 \
            gconf-2.0 \
            osso \
            modest-dbus-client-1.0 \
            TpSession \
            TelepathyQt4
        pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
        pkgconfig.files = QtMessaging.pc
        INSTALLS += pkgconfig \
            documentation
        LIBS += -lgconf-2 \
            -lrtcom-eventlogger \
            -lmodest-dbus-client-1.0 \
            -losso \
            -ldbus-glib-1 \
            -ldbus-1 \
            -lgobject-2.0 \
            -lglib-2.0 \
            -ltpsession \
            -ltelepathy-qt4 \
            -lhildonmime
    }
    maemo6 { 
	# qt-messaging depends on CommHistory, and CommHistory depends on qtm-contacts
	# so the include path needs to contain the QtContacts files CommHistory includes
	INCLUDEPATH += ../../include/QtContacts \
	../contacts \
	../contacts/details \
	../contacts/filters \
	../contacts/requests
        QT += dbus
        QT += sql
        CONFIG += link_pkgconfig
        PUBLIC_HEADERS -= qmessagecontentcontainer_p.h
        PRIVATE_HEADERS -= qmessagecontentcontainer_p.h
        HEADERS += qmessagecontentcontainer_maemo6_p.h
        PRIVATE_HEADERS += qmfhelpers_maemo6_p.h \
            telepathyhelpers_maemo6_p.h \
            telepathyengine_maemo6_p.h \
            tpsessionaccount_p.h \
            tpsessionchannel_p.h \
            qmfservice_maemo6_p.h \
            qmfstore_maemo6_p.h \
            qmessageservice_maemo6_p.h \
            maemo6helpers_p.h \
            smsmodel_maemo6_p.h \
            storageengine_maemo6_p.h
        SOURCES += qmessageid_maemo6.cpp \
            qmessagecontentcontainerid_maemo6.cpp \
            qmessagefolderid_maemo6.cpp \
            qmessageaccountid_maemo6.cpp \
            qmessagecontentcontainer_maemo6.cpp \
            qmessage_maemo6.cpp \
            qmessagefolder_maemo6.cpp \
            qmessageaccount_maemo6.cpp \
            qmessageaccountfilter_maemo6.cpp \
            qmessageaccountsortorder_maemo6.cpp \
            qmessagefolderfilter_maemo6.cpp \
            qmessagefoldersortorder_maemo6.cpp \
            qmessagefilter_maemo6.cpp \
            qmessagesortorder_maemo6.cpp \
            qmessagestore_maemo6.cpp \
            qmessageservice_maemo6.cpp \
            qmfhelpers_maemo6.cpp \
            telepathyengine_maemo6.cpp \
            maemo6helpers.cpp \
            storageengine_maemo6.cpp \
            smsmodel_maemo6.cpp \
            qmfstore_maemo6.cpp \
            qmfservice_maemo6.cpp \
            tpsessionaccount.cpp \
            tpsessionchannel.cpp
        documentation.path = $$QT_MOBILITY_PREFIX/doc
        documentation.files = doc/html
        PKGCONFIG += glib-2.0 \
            dbus-glib-1 \
            gconf-2.0 \
            TelepathyQt4 \
            qmfclient \
            commhistory \
            meegotouch \
            qttracker \
            messagingif0
        CONFIG += create_pc \
            create_prl
        QMAKE_PKGCONFIG_REQUIRES = glib-2.0 \
            dbus-glib-1 \
            gconf-2.0 \
            TelepathyQt4 \
            qmfclient \
            commhistory \
            meegotouch \
            qttracker \
            messagingif0
        pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
        pkgconfig.files = QtMessaging.pc
        INSTALLS += pkgconfig \
            documentation
    }
    symbian { 
        INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	contains(messaging_freestyle_enabled, yes) {
   	CONFIG += FREESTYLEMAIL
	DEFINES += FREESTYLEMAILUSED
	DEFINES += FREESTYLEMAILBOXOBSERVERUSED
	}
	contains(messaging_ncnlist_enabled, no) {
	DEFINES += NCNLISTREMOVED
	}
	contains(messaging_qthighway_enabled, yes) {
            CONFIG += QTHIGHWAY
            DEFINES += QTHIGHWAYUSED
        }
        HEADERS -= qmessagestore_p.h \
            qmessagecontentcontainer_p.h \
            qmessage_p.h
        HEADERS += qmtmengine_symbian_p.h \
            qmessagestore_symbian_p.h \
            qmessageservice_symbian_p.h \
            qmessagecontentcontainer_symbian_p.h \
            qmessage_symbian_p.h \
            maemohelpers_p.h
            contains(CONFIG, FREESTYLEMAIL) {
               HEADERS += qfsengine_symbian_p.h
            }

        SOURCES += qmtmengine_symbian.cpp \
            qmessageid_symbian.cpp \
            qmessagecontentcontainerid_symbian.cpp \
            qmessagefolderid_symbian.cpp \
            qmessageaccountid_symbian.cpp \
            qmessagecontentcontainer_symbian.cpp \
            qmessage_symbian.cpp \
            qmessagefolder_symbian.cpp \
            qmessageaccount_symbian.cpp \
            qmessageaccountfilter_symbian.cpp \
            qmessageaccountsortorder_symbian.cpp \
            qmessagefolderfilter_symbian.cpp \
            qmessagefoldersortorder_symbian.cpp \
            qmessagefilter_symbian.cpp \
            qmessagesortorder_symbian.cpp \
            qmessagestore_symbian.cpp \
            qmessageservice_symbian.cpp \
            maemohelpers.cpp
            
        contains(CONFIG, FREESTYLEMAIL) {
	    SOURCES += qfsengine_symbian.cpp
	}
        contains(CONFIG, QTHIGHWAY) {
            LIBS += -lxqservice \
                    -lxqserviceutil \
        } else {
            LIBS += -lsendui
        }
        LIBS += -lsendas2 \
            -lmsgs \
            -letext \
            -lefsrv \
            -lcharconv \
            -lgsmu \
            -limcm \
            -lbafl \
            -lmtur \
            -lsmcm \
            -limcm \
            -leikcore \
            -lcone \
            -lapgrfx \
            -lapmime \
            -lecom 
        TARGET.CAPABILITY = ALL \
            -TCB
        TARGET.UID3 = 0x2002AC82
        QtMessaging.sources = QtMessaging.dll
        QtMessaging.path = /sys/bin
        DEPLOYMENT += QtMessaging
    }
    win32 { 
        PRIVATE_HEADERS += winhelpers_p.h
        SOURCES += winhelpers.cpp \
            qmessageid_win.cpp \
            qmessagecontentcontainerid_win.cpp \
            qmessagefolderid_win.cpp \
            qmessageaccountid_win.cpp \
            qmessagecontentcontainer_win.cpp \
            qmessage_win.cpp \
            qmessagefolder_win.cpp \
            qmessageaccount_win.cpp \
            qmessageaccountfilter_win.cpp \
            qmessageaccountsortorder_win.cpp \
            qmessagefolderfilter_win.cpp \
            qmessagefoldersortorder_win.cpp \
            qmessagefilter_win.cpp \
            qmessagesortorder_win.cpp \
            qmessagestore_win.cpp \
            qmessageservice_win.cpp
        wince* { 
            # Include the source files from QMF needed for MIME parsing
            # These files are copied directly from the QMF repo with no changes:
            PRIVATE_HEADERS += win32wce/qmailaddress.h \
                win32wce/qmailcodec.h \
                win32wce/qmailfolderfwd.h \
                win32wce/qmailglobal.h \
                win32wce/qmailid.h \
                win32wce/qmailipc.h \
                win32wce/qmaillog.h \
                win32wce/qmailmessage.h \
                win32wce/qmailmessagefwd.h \
                win32wce/qmailmessage_p.h \
                win32wce/qmailnamespace.h \
                win32wce/qmailtimestamp.h \
                win32wce/longstring_p.h \
                win32wce/qprivateimplementation.h \
                win32wce/qprivateimplementationdef.h

           SOURCES += win32wce/qmailaddress.cpp \
                win32wce/qmailcodec.cpp \
                win32wce/qmailid.cpp \
                win32wce/qmailinstantiations.cpp \
                win32wce/qmaillog.cpp \
                win32wce/qmailmessage.cpp \
                win32wce/qmailmessagefwd.cpp \
                win32wce/qmailnamespace.cpp \
                win32wce/qmailtimestamp.cpp \
                win32wce/longstring.cpp \
                win32wce/qprivateimplementation.cpp
            DEFINES += QTOPIAMAIL_PARSING_ONLY \
                QTOPIAMAIL_OMIT_QCOP \
                SINGLE_MODULE_QTOPIAMAIL
            LIBS += cemapi.lib \
                strmiids.lib \
                uuid.lib
        }
        else:LIBS += mapi32.lib \
            shlwapi.lib \
            user32.lib
    }
}
    
simulator {
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
    DEFINES += QTOPIAMAIL_OMIT_QCOP 
}

simulator|contains(qmf_enabled, yes):!maemo6 { 
    DEFINES += USE_QMF_IMPLEMENTATION
    
    QMF_INCLUDEDIR = $$(QMF_INCLUDEDIR)
    QMF_LIBDIR = $$(QMF_LIBDIR)

    # QMF headers must be located at $QMF_INCLUDEDIR
    !isEmpty(QMF_INCLUDEDIR): INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

    # QMF libraries must be located at $QMF_LIBDIR
    macx {
        !isEmpty(QMF_LIBDIR): QMAKE_LFLAGS += -F$$(QMF_LIBDIR)
        LIBS += -framework qmfclient
    } else {
        !isEmpty(QMF_LIBDIR): LIBS += -L$$(QMF_LIBDIR)
	LIBS += -l$$qtLibraryTarget(qmfclient)
    }
		
    # QMF libraries must be located at $QMF_LIBDIR
    mac {
        QMAKE_LFLAGS += -F$$(QMF_LIBDIR)
            LIBS += -framework qmfclient
    } else {
        LIBS += -L$$(QMF_LIBDIR) -l$$qtLibraryTarget(qmfclient)
    }

    PRIVATE_HEADERS += qmfhelpers_p.h \
        qmessagestore_qmf_p.h
    SOURCES += qmessageid_qmf.cpp \
        qmessagecontentcontainerid_qmf.cpp \
        qmessagefolderid_qmf.cpp \
        qmessageaccountid_qmf.cpp \
        qmessagecontentcontainer_qmf.cpp \
        qmessage_qmf.cpp \
        qmessagefolder_qmf.cpp \
        qmessageaccount_qmf.cpp \
        qmessageaccountfilter_qmf.cpp \
        qmessageaccountsortorder_qmf.cpp \
        qmessagefolderfilter_qmf.cpp \
        qmessagefoldersortorder_qmf.cpp \
        qmessagefilter_qmf.cpp \
        qmessagesortorder_qmf.cpp \
        qmessagestore_qmf.cpp \
        qmessageservice_qmf.cpp \
        qmfhelpers.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

CONFIG += middleware
include(../../features/deploy.pri)
