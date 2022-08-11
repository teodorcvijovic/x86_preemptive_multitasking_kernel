SRC = ..\*.cpp 
INCLUDES = ..\*.h 
LIBS = ..\*.lib 
INCLUDEPATH = C:\bc31\include;..\;
LIBSPATH = C:\bc31\lib;..\;
TARGET = teo1.exe
$(TARGET):	$(SRC) $(INCLUDES) configuration.xmi
	bcc.exe -mh -v -e$(TARGET) -I$(INCLUDEPATH) -L$(LIBSPATH) $(SRC) $(LIBS)
all:	$(TARGET)
clean:
	-del *.obj
	-del $(TARGET)
	-del *.asm
configuration.xmi:	
	configuration.xmi