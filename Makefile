#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC)
endif

#include $(DEVKITPPC)/gamecube_rules
include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET      :=  boot
BUILD		:=	build
SOURCES		:=	\
    src \
    src/zip \
    src/wii \
    src/wii/common \
    src/wii/common/pngu \
    src/wii/common/FreeTypeGX    
DATA		:=	\
    src/wii/res/fonts \
    src/wii/res/gfx    
INCLUDES	:=	\
    src \
    src/zip \
    src/wii \
    src/wii/common \
    src/wii/common/pngu \
    src/wii/common/FreeTypeGX \
    thirdparty/freetype/include \
    thirdparty/sdl/SDL/include \
    thirdparty/sdl/SDL_ttf/include

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	= -g -O1 -Wall $(MACHDEP) $(INCLUDE) \
          -DNOCRYPT -DWII -DBIG_ENDIAN -DWII_BIN2O \
          -Wno-format-truncation \
          -Wno-narrowing
          
#-DLOWTRACE -DDEBUG
CXXFLAGS	=	$(CFLAGS)
LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lSDL_ttf -lSDL -lfreetype -lfat -lwiiuse -lwiikeyboard -lbte \
            -logc -lm -lpng -lz -lbz2

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= \
    thirdparty/freetype/lib \
    thirdparty/sdl/SDL/lib \
    thirdparty/sdl/SDL_ttf/lib

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:= \
    zip.c \
    unzip.c \
    pngu.c \
    wii_app.c \
    wii_config.c \
    wii_file_io.c \
    wii_freetype.c \
    wii_gx.c \
    wii_hash.c \
    wii_hw_buttons.c \
    wii_input.c \
    wii_main.c \
    wii_resize_screen.c \
    wii_sdl.c \
    wii_snapshot.c \
    wii_util.c \
    wii_video.c
	
CPPFILES	:=	\
    FreeTypeGX.cpp \
    Metaphrasis.cpp \
    Archive.cpp \
    Bios.cpp \
    Cartridge.cpp \
    Common.cpp \
    Database.cpp \
    Hash.cpp \
    Logger.cpp \
    Maria.cpp \
    Memory.cpp \
    Palette.cpp \
    Pokey.cpp \
    ProSystem.cpp \
    Region.cpp \
    Riot.cpp \
    Sally.cpp \
    Sound.cpp \
    Timer.cpp \
    Tia.cpp \
    wii_atari.cpp \
    wii_atari_config.cpp \
    wii_atari_emulation.cpp \
    wii_atari_menu.cpp \
    wii_atari_sdl.cpp \
    wii_atari_snapshot.cpp \
    wii_direct_sound.cpp    
        
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC) \
					-I$(DEVKITPRO)/portlibs/ppc/include
										
#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(CURDIR)/$(dir)) \
                    -L$(LIBOGC_LIB) \
                    -L$(DEVKITPRO)/portlibs/ppc/lib
					
export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol

#---------------------------------------------------------------------------------
run:
	psoload $(TARGET).dol

#---------------------------------------------------------------------------------
reload:
	psoload -r $(TARGET).dol


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	:	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .png extension
#---------------------------------------------------------------------------------
%.png.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

%.ttf.o	:	%.ttf
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
# This rule links in binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

%.mod.o	:	%.mod
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
