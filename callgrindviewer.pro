HEADERS       = mainwindow.h \
                callgrindhighlighter.h \
                findfiledialog.h \
                assistant.h \
                foldoverlay.h \
                linenumbering.h \
                textedit.h
SOURCES       = main.cpp \
                callgrindhighlighter.cpp \
                foldoverlay.cpp \
                linenumbering.cpp \
                mainwindow.cpp \
                findfiledialog.cpp \
                assistant.cpp \
                textedit.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

QT += widgets

target.path = $$[QT_INSTALL_EXAMPLES]/assistant/callgrindviewer
docs.files += $$PWD/documentation
docs.path = $$target.path

INSTALLS += target docs
