PROJECT      = LolLauncher
TARGET       = $(PROJECT).exe
VCINSTALLDIR = C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
CXX          = cl
CXXFLAGS     = /std:c++latest /c /ZI /W4 /WX- /sdl /Od /Oy- /D _MBCS /Gm /EHsc /RTC1 /MDd /GS /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"obj\\" /Fd"bin\vc140.pdb" /Gd /TP /analyze- /nologo
LD           = link
LDFLAGS      = /INCREMENTAL kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /MANIFEST "/MANIFESTUAC:level='asInvoker' uiAccess='false'" /manifest:embed /DEBUG:FASTLINK "/PDB:bin\$(PROJECT).pdb" /TLBID:1 /DYNAMICBASE /NXCOMPAT "/IMPLIB:bin\$(PROJECT).lib" /MACHINE:X86 /nologo

SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp, obj/%.obj, $(SRC))

.PHONY: all
all: bin/$(TARGET)
	@echo done.

.PHONY: clean
clean:
	@echo cleaning...
	@del /S obj\*.obj bin\*.exe 2>nul
	@echo finished.

.PHONY: rebuild
rebuild: clean all

bin/$(TARGET): $(OBJ)
	@echo linking $@...
	@call "$(VCINSTALLDIR)\vcvarsall.bat" x86 && $(LD) $(LDFLAGS) "/OUT:bin\$(TARGET)" $(OBJ)
	@echo link finished.

obj/%.obj: src/%.cpp
	@call "$(VCINSTALLDIR)\vcvarsall.bat" x86 && $(CXX) $(CXXFLAGS) $<