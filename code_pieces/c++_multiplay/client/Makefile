##########################################################################
#
# 2016.04.16 jesssoft
#
##########################################################################
TARGET_RELEASE=client
OBJDIR_RELEASE=./release
TARGET_DEBUG=client_d
OBJDIR_DEBUG=./debug

# release version.
ifeq ($(mode), release)
	CXXFLAGS=-Wall -D_REENTRANT -DNDEBUG -O3 -std=c++0x
	TARGET:=$(TARGET_RELEASE)
	OBJDIR:=$(OBJDIR_RELEASE)
	COMMON_LIB=-L../common -lcommon
else
# debug version.
	CXXFLAGS=-Wall -g -D_REENTRANT -D_DEBUG -std=c++0x
	TARGET:=$(TARGET_DEBUG)
	OBJDIR:=$(OBJDIR_DEBUG)
	COMMON_LIB=-L../common -lcommon_d
endif

# common setting.
##########################################################################
FILES=main netlib player game sprite
##########################################################################
CXX=g++
SRCS=$(FILES:=.cc)
OBJS=$(FILES:=.o)
OBJS:=$(addprefix $(OBJDIR)/, $(OBJS))
INCLUDES=
LIBS=-lSDL2 -lSDL2_image
LIBS+=-lpthread
MAPFILE=-Wl,-Map=$(OBJDIR)/$(TARGET).map

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(MAPFILE) -o $@ $^ $(LIBS) $(COMMON_LIB)

$(OBJDIR)/%.o:%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	@$(CXX) -MM $(CXXFLAGS) $*.cc > $(OBJDIR)/$*.d
	@sed '1,1s/^/.\/debug\//' $(OBJDIR)/$*.d > $(OBJDIR)/$*.d.tmp
	@mv $(OBJDIR)/$*.d.tmp $(OBJDIR)/$*.d

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(TARGET_RELEASE) $(TARGET_DEBUG)
	rm -f $(OBJDIR_DEBUG)/*
	rm -f $(OBJDIR_RELEASE)/*

dist:
	cp ./$(TARGET) ../bin

init:
	ctags -R

-include $(OBJS:.o=.d)


