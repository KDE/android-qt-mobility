TEMPLATE = subdirs

SUBDIRS += accel arrowkeys metadata metadata2

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

SUBDIRS += grueplugin grueapp

SUBDIRS += sensor_explorer

contains(QT_CONFIG,opengl):SUBDIRS += cubehouse

SUBDIRS +=\
    show_acceleration\
    show_als\
    show_compass\
    show_magneticflux\
    show_orientation\
    show_proximity\
    show_rotation\
    show_tap\
    show_light\
    show_gyroscope

