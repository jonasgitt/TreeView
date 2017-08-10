QT += widgets

FORMS       = mainwindow.ui
HEADERS     = mainwindow.h \
              treeitem.h \
              treemodel.h \
    combodelegate.h \
    node.h \
    scriptgenerator.h
RESOURCES   = editabletreemodel.qrc
SOURCES     = mainwindow.cpp \
              treeitem.cpp \
              treemodel.cpp \
              main.cpp \
    combodelegate.cpp \
    node.cpp \
    scriptgenerator.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/editabletreemodel
INSTALLS += target
