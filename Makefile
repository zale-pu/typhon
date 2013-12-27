CUR_PATH := $(shell pwd)
PARENT_PATH := $(shell cd $(dir $(CUR_PATH)) && pwd)

export PRJ_PATH := $(CUR_PATH)/prj
export SO_PATH := $(PRJ_PATH)/lib
export OBJ_PATH := $(PRJ_PATH)/obj
export INC_PATH := -I$(CUR_PATH)/include

CC := gcc
RM := -rm -rf
MKDIR = -mkdir -p
PARAM = -D CONFIG_THREAD_PTHREADS -D CONFIG_THREAD_EMULATION

SUBMAKE_DIR := $(addprefix $(CUR_PATH)/,$(dir $(shell find */ -name "Makefile")))
SUBOBJ_DIR := $(addprefix $(PRJ_PATH)/,$(dir $(shell find */ -name "Makefile")))

CONFIG := $(shell find -name "test_*_yaml"
C_LIB = $(shell find FileAccessModule TestCases GDFlashSim -name "*.c")
OBJ_LIB = $(patsubst $(CUR_PATH)%.c,$(OBJ_DIR)%.o,$(shell find TestCases -name "*.c"))
SO_LIB = $(notdir $(shell find $(SO_PATH) -name "lib*.so"))
A_LIB = $(shell find $(CUR_PATH) -name "libyaml.a")

CFLAGS := -g -Wall
export SOCFLAGS := -g
LDCFLAGS = -L$(SO_PATH) \
		  $(patsubst lib%.so,-l%,$(notdir $(SO_LIB))) \
		  -lpthread \
		  $(A_LIB) \
		  -Wl,-rpath=$(SO_PATH)

.PHONY : all
all : $(SUBMAKE_DIR) sim

.PHONY : $(SUBMAKE_DIR)
$(SUBMAKE_DIR) :
	@echo "---------------------------------------"
	@$(MAKE) -C $@
 
.PHONY : sim
sim :
	@echo "***************************************"
	$(CC) $(CFLAGS) $(SOCFLAGS) $(C_LIB) -o $(PRJ_PATH)/$@ $(INC_PATH) $(LDCFLAGS)
 
.PHONY : clean
clean :
	$(RM) $(OBJ_DIR)
	$(RM) $(SO_PATH)/$(SO_LIB)

.PHONY : rm
rm :
	$(RM) $(PRJ_PATH)

.PHONY : check
check :
	@echo "a  ----------" $(shell find $(PARENT_PATH) -name "*.a")
	@echo "so ----------" $(shell find $(SO_PATH) -name "lib*.so")
	@echo "lso----------" $(patsubst lib%.so,-l%,$(notdir $(shell find $(SO_PATH) -name "lib*.so")))
	@echo "obj----------" $(shell find $(OBJ_PATH)/*_obj -name "*.o")
