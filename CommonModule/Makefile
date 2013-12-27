INC_PATH ?= -I../include
PRJ_PATH ?= $(shell pwd)
OBJ_PATH ?= $(PRJ_PATH)/obj
SO_PATH ?= $(PRJ_PATH)/lib

CC := gcc
RM := -rm -rf
MKDIR = -mkdir -p
CFLAGS := -g -Wall
SOCFLAGS += -fPIC --shared
PARAM ?=

TARGET := so
ifeq ($(TARGET),obj)
	OBJ_DIR := $(OBJ_PATH)/$(notdir $(shell pwd))_obj
else
	OBJ_DIR := $(OBJ_PATH)/$(notdir $(shell pwd))
endif

C_LIB := $(patsubst ./%, %, $(shell find -path "*bk" -prune -o -name "*.c" -print))
OBJ_LIB := $(patsubst %.c, %.o, $(C_LIB))
SO_LIB = $(addsuffix .so, $(addprefix lib, $(notdir $(shell pwd))))

.PHONY : all
all : $(TARGET)

.PHONY: obj
obj : mkobjdir $(OBJ_LIB)
mkobjdir : $(dir $(C_LIB))
	$(MKDIR) $(OBJ_DIR) $(addprefix $(OBJ_DIR)/,$(subst ./,,$^))

$(OBJ_LIB) : %.o : %.c
	$(CC) -o $(OBJ_DIR)/$@ -c $< $(CFLAGS) $(INC_PATH) $(PARAM)

.PHONY : so
so : mksodir mkobjdir $(SO_LIB)
mksodir :
	$(MKDIR) $(SO_PATH)

$(SO_LIB) : $(OBJ_LIB)
	cd $(OBJ_DIR) && \
		$(CC) $(SOCFLAGS) $^ -o $(SO_PATH)/$@

# $(SO_LIB) : $(C_LIB)
	# $(CC) $(CFLAGS) $(SOCFLAGS) $^ -o $(SO_PATH)/$@ $(INC_PATH) $(PARAM)

.PHONY : clean
clean :
	$(RM) $(OBJ_DIR)
	$(RM) $(SO_PATH)/$(SO_LIB)

.PHONY : rm
rm :
	$(RM) $(OBJ_PATH)
	$(RM) $(SO_PATH)

.PHONY : check
check : $(C_LIB)
	@echo "c****" $(C_LIB)
	@echo "c****"
	@echo "obj****" $(OBJ_LIB)
	@echo "so****" $(SO_LIB)
