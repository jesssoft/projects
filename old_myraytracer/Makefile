#########################################################################
#
# 2010.12.01 jesssoft.
#
#########################################################################
TARGET_RELEASE=run
OBJDIR_RELEASE=./release
TARGET_DEBUG=run_d
OBJDIR_DEBUG=./debug

# release version.
ifeq ($(mode),release)
	CFLAGS=-Wall -O3 -DNDEBUG -std=c++0x
	TARGET:=$(TARGET_RELEASE)
	OBJDIR:=$(OBJDIR_RELEASE)
else
# debug version.
	CFLAGS=-Wall -g -D_DEBUG -std=c++0x
	TARGET:=$(TARGET_DEBUG)
	OBJDIR:=$(OBJDIR_DEBUG)
endif
#########################################################################
FILES=main engine ray surface vector3 material plane raytracer\
      twister matrix primitive scene util_imgfile
#########################################################################
CXX=g++
SRCS=$(FILES:=.cc)
OBJS=$(FILES:=.o)
OBJS:=$(addprefix $(OBJDIR)/, $(OBJS))
INCLUDES=
LIBS=
MAPFILE=-Wl,-Map=$(OBJDIR)/$(TARGET).map

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(INCLUDES) $(LIBS) $(MAPFILE) -o $@ $^

$(OBJDIR)/%.o:%.cc
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ -c $<
	@$(CXX) -MM $(CFLAGS) $*.cc > $(OBJDIR)/$*.d 
	@sed '1,1s/^/.\/debug\//' $(OBJDIR)/$*.d > $(OBJDIR)/$*.d.tmp
	@mv $(OBJDIR)/$*.d.tmp $(OBJDIR)/$*.d	

clean:
	rm -f $(OBJDIR_DEBUG)/*
	rm -f $(OBJDIR_RELEASE)/*
	rm -f $(TARGET_RELEASE) $(TARGET_DEBUG)

init:
	ctags -R

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

-include $(OBJS:.o=.d)

