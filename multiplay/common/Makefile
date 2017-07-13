##########################################################################
#
# 2016.04.16 jesssoft
#
##########################################################################
TARGET_RELEASE=libcommon.a
OBJDIR_RELEASE=./release
TARGET_DEBUG=libcommon_d.a
OBJDIR_DEBUG=./debug

# release version.
ifeq ($(mode), release)
	CXXFLAGS=-Wall -D_REENTRANT -DNDEBUG -O3 -std=c++0x
	TARGET:=$(TARGET_RELEASE)
	OBJDIR:=$(OBJDIR_RELEASE)
else
# debug version.
	CXXFLAGS=-Wall -g -D_REENTRANT -D_DEBUG -std=c++0x
	TARGET:=$(TARGET_DEBUG)
	OBJDIR:=$(OBJDIR_DEBUG)
endif

# common setting.
##########################################################################
FILES=msg iobuf socket_data util posdata
##########################################################################
CXX=g++
SRCS=$(FILES:=.cc)
OBJS=$(FILES:=.o)
OBJS:=$(addprefix $(OBJDIR)/, $(OBJS))
INCLUDES=
LIBS=
LIBS+=-lpthread
MAPFILE=-Wl,-Map=$(OBJDIR)/$(TARGET).map

all: $(TARGET)

$(TARGET): $(OBJS)
	ar -rcs $(TARGET) $(OBJS)

$(OBJDIR)/%.o:%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	@$(CXX) -MM $(CXXFLAGS) $*.cc > $(OBJDIR)/$*.d
	@sed '1,1s/^/.\/debug\//' $(OBJDIR)/$*.d > $(OBJDIR)/$*.d.tmp
	@mv $(OBJDIR)/$*.d.tmp $(OBJDIR)/$*.d

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f *.gch
	rm -f $(TARGET_RELEASE) $(TARGET_DEBUG)
	rm -f $(OBJDIR_DEBUG)/*
	rm -f $(OBJDIR_RELEASE)/*

init:
	ctags -R

-include $(OBJS:.o=.d)

