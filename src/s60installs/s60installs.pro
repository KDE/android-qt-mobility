TEMPLATE = subdirs

symbian: {
    load(data_caging_paths)

    SUBDIRS =
    TARGET = "QtMobility"
    TARGET.UID3 = 0x2002AC89
    # TP preview 0.1.0
    # Beta 0.2.0
    # Final 1.0.0

    VERSION = 0.2.0

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    qtmobilitydeployment.pkg_prerules += vendorinfo

    EPOCROOT31 = $${EPOCROOT31}
    EPOCROOT50 = $${EPOCROOT50}
    
    qtmobilitydeployment.sources = \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtMessaging.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtServiceFramework.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/SFWDatabaseManagerServer.exe \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtLocation.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtSystemInfo.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtPublishSubscribe.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/PSPathMapperServer.exe \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtContacts.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtVersit.dll


    bearer = \
        "IF package(0x1028315F)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$$(EPOCROOT31)epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
        "ELSE" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
        "ENDIF"

    contacts = \
        "IF package(0x1028315F)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$$(EPOCROOT31)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ELSE" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ENDIF"

    pluginstubs = \
        "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbian/qmakepluginstubs/mobapicontactspluginsymbian.qtplugin\" - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbian.qtplugin\""

    qtmobilitydeployment.pkg_postrules += bearer
    qtmobilitydeployment.pkg_postrules += contacts
    qtmobilitydeployment.pkg_postrules += pluginstubs
    
    qtmobilitydeployment.path = /sys/bin
    
    DEPLOYMENT += qtmobilitydeployment 
}
