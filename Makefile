BINARY := Helios

OBJS := MSHLanguageMgr.o \
		LanguageConfigView.o \
		MSHLanguageFile.o \
		DebugTools.o \
		AboutWindow.o \
		Application1.o \
		ApplicationConfigView.o \
		AudioCDPanel.o \
		AudioOutput.o \
		AudioRow.o \
		AudioWindow.o \
		BitmapMenuItem.o \
		BootableCDPanel.o \
		BrowseField.o \
		BubbleHelper.o \
		CDExtraPanel.o \
		CDRECORDCmdLine.o \
		CDTypeMenu.o \
		ColorConfigView.o \
		ColorListItem.o \
		ColorTestView.o \
		ColumnListView.o \
		ColumnTypes.o \
		ConfigHeadLine.o \
		ConfigManager.o \
		ConfigView.o \
		ConfigWindow.o \
		DataCDPanel.o \
		DeviceConfigView.o \
		DrawingTidbits.o \
		EditMenuItem.o \
		EditPopUpMenu.o \
		EditView.o \
		ErrorBox.o \
		FSPanel.o \
		FileChooser.o \
		FileColumn.o \
		FileDialog.o \
		FileField.o \
		FolderRow.o \
		FileUtils.o \
		HeliosConfigView.o \
		IconBar.o \
		ImageConfigView.o \
		InfoBox.o \
		InfoFile.o \
		ListView1.o \
		LogSettingsWindow.o \
		M3UList.o \
		MakeBFSImage.o \
		MKISOFSCmdLine.o \
		MString.o \
		MediaInfo.o \
		PathConfigView.o \
		PipeCmd.o \
		ProtocolWindow.o \
		SaveConfigView.o \
		SoundConfigView.o \
		StandardConfigView.o \
		StatusBar.o \
		StatusView.o \
		StatusWindow.o \
		Stringview1.o \
		TransportButton.o \
		VersionConfigView.o \
		VersionControl.o \
		VideoDVDPanel.o \
		View1.o \
		View2.o \
		Window1.o \
		Window2.o \
		main.o \
		CDText/crc16.o \
		CDText/CDText.o
		
OBJDIR := build

RSRCS := res/HeliosCDRECORDer.rsrc

OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

CC := g++
ASM := yasm
LD := $(CC)

LIBS := -lroot -lbe $(STDCPPLIBS) -ltranslation -ltextencoding -lmedia -ltracker
CFLAGS := -O3 -I./includes -I./src/CDText -D_BEOS_HAIKU_BUILD_ -Wno-write-strings
LDFLAGS := 

.PHONY : clean build

default : build

build : $(BINARY)
	
$(BINARY) : $(OBJDIR) $(OBJS) $(RSRCS)
	$(LD) $(CFLAGS) $(LIBS) $(OBJS) -o $(BINARY) $(LDFLAGS)
	xres -o $(BINARY) $(RSRCS)
	mimeset -f $(BINARY)

clean:
	rm -rf $(OBJDIR)/*.*
	rm -rf $(BINARY)

$(OBJDIR)/%.o : src/%.cpp
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/CDText
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : src/%.c
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/CDText
	$(CC) $(CFLAGS) -c $< -o $@
