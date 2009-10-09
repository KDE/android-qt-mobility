TEMPLATE = lib
TARGET = QtMessaging

include(../common.pri)

INCLUDEPATH += .

#Input

DEFINES += QT_BUILD_MESSAGING_LIB QT_MAKEDLL

PUBLIC_HEADERS += \
           qmessageglobal.h \
           qmessageid.h \
           qmessagecontentcontainerid.h \
           qmessagefolderid.h \
           qmessageaccountid.h \
           qmessagecontentcontainer.h \ 
           qmessageaddress.h \ 
           qmessage.h \
           qmessagefolder.h \
           qmessageaccount.h \
           qmessageaccountfilter.h \
           qmessageaccountordering.h \
           qmessagefolderfilter.h \
           qmessagefolderordering.h \
           qmessagefilter.h \
           qmessageordering.h \
           qmessagestore.h \
           qmessageserviceaction.h

PRIVATE_HEADERS += \
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
           qmessageaccountordering_p.h \
           qmessagefolderfilter_p.h \
           qmessagefolderordering_p.h \
           qmessagefilter_p.h \
           qmessageordering_p.h \
           qmessagestore_p.h

SOURCES += qmessageid.cpp \
           qmessagecontentcontainerid.cpp \
           qmessagefolderid.cpp \
           qmessageaccountid.cpp \
           qmessagecontentcontainer.cpp \
           qmessageaddress.cpp \
           qmessage.cpp \
           qmessagefolder.cpp \
           qmessageaccount.cpp \
           qmessageaccountfilter.cpp \
           qmessageaccountordering.cpp \
           qmessagefolderfilter.cpp \
           qmessagefolderordering.cpp \
           qmessagefilter.cpp \
           qmessageordering.cpp \
           qmessagestore.cpp \
           qmessageserviceaction.cpp


symbian|win32|maemo {
maemo {
SOURCES += qmessageid_maemo.cpp \
           qmessagecontentcontainerid_maemo.cpp \
           qmessagefolderid_maemo.cpp \
           qmessageaccountid_maemo.cpp \
           qmessagecontentcontainer_maemo.cpp \
           qmessage_maemo.cpp \
           qmessagefolder_maemo.cpp \
           qmessageaccount_maemo.cpp \
           qmessageaccountfilter_maemo.cpp \
           qmessageaccountordering_maemo.cpp \
           qmessagefolderfilter_maemo.cpp \
           qmessagefolderordering_maemo.cpp \
           qmessagefilter_maemo.cpp \
           qmessageordering_maemo.cpp \
           qmessagestore_maemo.cpp \
           qmessageserviceaction_maemo.cpp 
}
symbian {
SOURCES += qmessageid_symbian.cpp \
           qmessagecontentcontainerid_symbian.cpp \
           qmessagefolderid_symbian.cpp \
           qmessageaccountid_symbian.cpp \
           qmessagecontentcontainer_symbian.cpp \
           qmessage_symbian.cpp \
           qmessagefolder_symbian.cpp \
           qmessageaccount_symbian.cpp \
           qmessageaccountfilter_symbian.cpp \
           qmessageaccountordering_symbian.cpp \
           qmessagefolderfilter_symbian.cpp \
           qmessagefolderordering_symbian.cpp \
           qmessagefilter_symbian.cpp \
           qmessageordering_symbian.cpp \
           qmessagestore_symbian.cpp \
           qmessageserviceaction_symbian.cpp 

MMP_RULES += "EXPORTUNFROZEN"
}

win32 {
!static:DEFINES += QT_MAKEDLL

PRIVATE_HEADERS += \
           winhelpers_p.h

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
           qmessageaccountordering_win.cpp \
           qmessagefolderfilter_win.cpp \
           qmessagefolderordering_win.cpp \
           qmessagefilter_win.cpp \
           qmessageordering_win.cpp \
           qmessagestore_win.cpp \
           qmessageserviceaction_win.cpp 

wince* {
    LIBS += cemapi.lib strmiids.lib uuid.lib
}
else {
    LIBS += mapi32.lib shlwapi.lib user32.lib
}

}
} else {
DEFINES += USE_QMF_IMPLEMENTATION

# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

PRIVATE_HEADERS += \
           qmfhelpers_p.h

SOURCES += qmessageid_qmf.cpp \
           qmessagecontentcontainerid_qmf.cpp \
           qmessagefolderid_qmf.cpp \
           qmessageaccountid_qmf.cpp \
           qmessagecontentcontainer_qmf.cpp \
           qmessage_qmf.cpp \
           qmessagefolder_qmf.cpp \
           qmessageaccount_qmf.cpp \
           qmessageaccountfilter_qmf.cpp \
           qmessageaccountordering_qmf.cpp \
           qmessagefolderfilter_qmf.cpp \
           qmessagefolderordering_qmf.cpp \
           qmessagefilter_qmf.cpp \
           qmessageordering_qmf.cpp \
           qmessagestore_qmf.cpp \
           qmessageserviceaction_qmf.cpp \
           qmfhelpers.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

include(../features/deploy.pri)
