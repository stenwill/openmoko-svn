FORMS*=controlbase.ui
HEADERS*=control.h attranslator.h gsmspec.h gsmitem.h 
SOURCES*=main.cpp control.cpp attranslator.cpp gsmspec.cpp gsmitem.cpp
TEMPLATE=app
QT*=core gui network xml
LIBS+=-Llib -lphonesim
