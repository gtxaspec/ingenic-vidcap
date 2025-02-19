INCLUDES 	 = -I./include/imp_sys
SDK_LIB_DIR	=  ./lib/imp_sys/uclibc
IMP_LIBS	= $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a
LIBS	=  $(IMP_LIBS)

CROSS_COMPILE?= mips-linux-uclibc-gnu-

STRIP        = $(CROSS_COMPILE)strip
COMPILE_OPTS =      $(INCLUDES) -I. -O2 -Wall -march=mips32r2 -DSOCKLEN_T=socklen_t -g
#COMPILE_OPTS =      $(INCLUDES) -I. -O2 -Wall -march=mips32r2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DLOCALE_NOT_USED -g
C 			 =         c
C_COMPILER   =	$(CROSS_COMPILE)gcc
C_FLAGS 	 =       $(COMPILE_OPTS) $(CPPFLAGS) $(CFLAGS)
CPP 		 =           cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =   $(COMPILE_OPTS) -Wall -DBSD=1 $(CPPFLAGS) $(CXXFLAGS)
OBJ 		 =           o
LINK 		 =  $(CROSS_COMPILE)gcc -o
LINK_OPTS    =  -lpthread -lm -lrt -ldl -static
CONSOLE_LINK_OPTS = $(LINK_OPTS)
LINK_OBJ	 = pwm.o imp-common.o capture_and_encoding.o ingenic-vidcap.o

ifeq ($(TARGET),wcv3)
	COMPILE_OPTS += -DSENSOR_GC2053 -DSENSOR_FRAME_RATE_NUM=30 \
					-DIRCUT_EN_GPIO=53 -DIRCUT_DIS_GPIO=52 \
					-DEXP_NIGHT_THRESHOLD=30000 -DEXP_DAY_THRESHOLD=5900 -DEXP_IR_THRESHOLD=30000

	# T31 changes
	COMPILE_OPTS += -DPLATFORM_T31
	IMP_LIBS := $(IMP_LIBS:libimp.a=t31/libimp.a)
else
	COMPILE_OPTS += -DSENSOR_JXF23 -DSENSOR_FRAME_RATE_NUM=15 \
					-DIRCUT_EN_GPIO=25 -DIRCUT_DIS_GPIO=26 \
					-DEXP_NIGHT_THRESHOLD=1900000 -DEXP_DAY_THRESHOLD=479832 -DEXP_IR_THRESHOLD=3000000
	COMPILE_OPTS += -DPLATFORM_T20L
endif

APP = ingenic-vidcap

commit_tag=$(shell git rev-parse --short HEAD)
.PHONY:all
all : version $(APP)
version :
	@if  ! grep "$(commit_tag)" version.h >/dev/null ; then                   \
        echo "update version.h"       ;    \
        sed 's/COMMIT_TAG/"$(commit_tag)"/g' version.tpl.h > version.h     ;  \
    fi

$(APP):
.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<
$(APP): $(LINK_OBJ)
	$(LINK)$@  $(LINK_OBJ)  $(LIBS) $(CONSOLE_LINK_OPTS)
	$(STRIP) -s $@
	cp $(APP) ~/tftproot/

clean:
	-rm -rf *.$(OBJ) $(APP) core *.core *~ include/*~ version.h

