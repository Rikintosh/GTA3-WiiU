# based on DKP's makefile template.

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM not in your environment.")
endif

include $(DEVKITARM)/3ds_rules

TARGET		:=	lights
BUILD_CFG	:=	
DEBUG		:=	-g
WISDOM		:=	-Wall
OPTFLAGS	:=	-O2

LIBRW_SRC	:=	../src \
			../src/3ds \
			../src/3ds/tex \
			../src/d3d \
			../src/gl \
			../src/ps2 \
			../src/lodepng

LIGHTS_SRC	:=	../tools/lights \
			../skeleton \
			../skeleton/imgui

SOURCE_DIRS	=	$(LIBRW_SRC) $(LIGHTS_SRC)
INCLUDE_DIRS	=	$(CTRULIB)/include ../ $(SOURCE_DIRS)

# code generation

ARCH		:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
ASFLAGS		:=	$(DEBUG) $(ARCH)
CFLAGS		:=	$(WISDOM) $(DEBUG) $(OPTFLAGS) $(ARCH)
CFLAGS		+=	-mword-relocations -ffunction-sections -fpermissive
CFLAGS		+=	-DARM11 -D_3DS -DRW_3DS -DLIBRW $(BUILD_CFG)
CFLAGS		+=	$(foreach dir,$(INCLUDE_DIRS),-I$(dir))
LIBS		:=	-lcitro3d -lctru -lm
LIBDIRS		:=	-L$(CTRULIB)/lib
LDFLAGS		=	-specs=3dsx.specs $(DEBUG) $(ARCH) -Wl,-Map,$(notdir $*.map)
CXXFLAGS	=	-fno-rtti -fno-exceptions -std=gnu++11 $(CFLAGS) $(INC_PICA)

OBJTREE		:=	$(foreach dir,$(SOURCE_DIRS),$(patsubst ../%,./obj/%.dir,$(dir)))
CPPFILES	:=	$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.cpp))
PICAFILES	:=	$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.shlist))

OBJ_CXX		:=	$(foreach file,$(CPPFILES),$(patsubst ../%,./obj/%,$(patsubst %.cpp,%.o,$(file))))
OBJ_PICA	:=	$(foreach file,$(PICAFILES),$(patsubst ../%,./obj/%,$(patsubst %.shlist,%.shbin.o,$(file))))
HDR_PICA	:=	$(foreach file,$(PICAFILES),$(patsubst ../%,./obj/%,$(patsubst %.shlist,%_shbin.h,$(file))))
INC_PICA	:=	$(sort $(foreach obj,$(OBJ_PICA),-I$(dir $(obj))))

DEPS		:=	$(foreach obj,$(OBJ_CXX),$(patsubst %.o,%.d,$(obj)))

# rules
default: $(TARGET).3dsx

# creating object directories
./obj/%.dir:
	@mkdir -p $(patsubst %.dir,%,$@)

# creating shader objects
./obj/%.shbin.o ./obj/%_shbin.h: ../%.shlist
	@echo building shader $@
	@set -e; \
	 obj=$@; \
	 src=$$(realpath $<); \
	 cnt=0; \
	 bin=$(patsubst %.shbin.o,%.shbin,$@); \
	 hdr=$(patsubst %.shbin.o,%_shbin.h,$@); \
	 name=$(patsubst %.shbin.o,%,$(notdir $@)); \
	 if [ -f $$obj ]; then echo rm -f $$obj; fi; \
	 if [ -f $$hdr ]; then echo rm -f $$hdr; fi; \
	 if [ -f $$bin ]; then echo rm -f $$bin; fi; \
	 oldpwd="$$(pwd)"; \
	 cd $$(dirname $$src); \
	 srcfiles=$$(realpath $$(cat $$src)); \
	 cd "$$oldpwd"; \
	 echo $$srcfiles; \
	 picasso $$srcfiles -o $$bin -h $$hdr; \
	 echo "extern const u8 $${name}_shbin[];" >> $$hdr; \
	 echo "extern const u8 $${name}_shbin_end[];" >> $$hdr; \
	 echo "extern const u32 $${name}_shbin_size;" >> $$hdr; \
	 for prg in $$srcfiles; do \
	   prgname=$$(basename $$prg .v.pica | tr [a-z] [A-Z]); \
	   echo "#define VSH_PRG_$${prgname} $$cnt" >> $$hdr; \
	   cnt=$$((cnt+1)); \
	 done; \
	 bin2s $$bin | $(AS) -o $$obj;

# creating dependencies (lmao copied from GNU documentation)
./obj/%.d: ../%.cpp
	@echo creating dependency $@
	@set -e; rm -f $@; \
	  $(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	  sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	  rm -f $@.$$$$

# compiling
./obj/%.o: ../%.cpp
	@echo $(CXX) -c $< -o $@ '$$(CXXFLAGS)'
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

# linking
%.elf:
	@echo linking $(notdir $@)
	@$(CXX) $(LDFLAGS) $(OBJ_PICA) $(OBJ_CXX) $(LIBDIRS) $(LIBS) -o $@
	@$(NM) -CSn $@ > $(notdir $*.lst)

# order only prerequisites (I'm still confused so don't worry)
$(OBJ_CXX): | $(OBJTREE)
$(OBJ_PICA): | $(OBJTREE)
$(DEPS): | $(OBJ_PICA)
$(OBJ_CXX): | $(DEPS)

# make likes killing elfs unless you tell it not to.
$(TARGET).3dsx:	$(TARGET).elf

$(TARGET).elf:	$(OBJ_CXX) $(OBJ_PICA)

.PHONY: clean
clean:
	@rm -f $(TARGET).3dsx $(TARGET).elf $(HDR_PICA) $(OBJ_PICA) $(OBJ_CXX)

-include $(DEPS)
