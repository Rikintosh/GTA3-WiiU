ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM not in your environment.")
endif

include $(DEVKITARM)/3ds_rules

TARGET		:=	openal
DEBUG		:=	-g
BUILD_CFG	:=	
OPTFLAGS	:=	-O2

sources		:=	$(wildcard ../Alc/*.c)
sources		+=	$(wildcard ../OpenAL32/*.c)
sources		+=	../Alc/backends/null.c \
			../Alc/backends/loopback.c \
			../Alc/backends/ctr.c

includes	:=	./ ../include ../OpenAL32/Include ../Alc ../Alc/backends \
			$(CTRULIB)/include

OBJECTS		:=	$(foreach obj,$(sources),$(patsubst ../%,./obj/%.o,$(obj)))

ARCH		:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
ASFLAGS		:=	$(DEBUG) $(ARCH)
CFLAGS		:=	$(WISDOM) $(DEBUG) $(OPTFLAGS) $(ARCH)
CFLAGS		+=	-mword-relocations -ffunction-sections
CFLAGS		+=	-DARM11 -D_3DS $(BUILD_CFG)
CFLAGS		+=	$(foreach dir,$(includes),-I$(dir))
LIBS		:=	
LIBDIRS		:=	-L$(CTRULIB)/lib
LDFLAGS		=	-specs=3dsx.specs $(DEBUG) $(ARCH) -Wl,-Map,$(notdir $*.map)

obj/%.c.o:	../%.c
	@mkdir -p $(dir $@) && true
	@echo $(CC) -c $< -o $@ '$$(CFLAGS)'
	@$(CC) -c $< -o $@ $(CFLAGS)


default: $(TARGET).a

$(TARGET).a: $(OBJECTS)

