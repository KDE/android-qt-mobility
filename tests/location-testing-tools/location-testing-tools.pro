TEMPLATE = subdirs

equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7) {
    contains(mobility_modules,bearer) {
    	SUBDIRS +=  geoservicedemo \
                    # mapviewer \
		    # mapbox
    }
} else {
    SUBDIRS +=  geoservicedemo \
                # mapviewer \
                # mapbox
}
