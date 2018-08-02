CPPFLAG = -std=c++11 -I../include -Wall  -Wno-unused-command-line-argument -pthread
CPPFLAG += -g
CC=clang++

# 定义构建目录
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
BINDIR = $(BUILDDIR)/bin
LIBDIR = $(BUILDDIR)/lib

# 获取当前目录下所有目录
DIRS = $(shell find $(shell pwd)  -type d)
# 该目录下所有源文件
SRC_FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
# 源文件对应的build/obj 下的obj目标名(exam: project_name/src/test.cpp --> build/obj/project_name/src/test.o)
SRC_OBJ = $(patsubst $(shell dirname `pwd`)/%.cpp, $(OBJDIR)/%.o, $(SRC_FILES))
# 头文件依赖
DEPS:=$(SRC_OBJ:%.o=%.d)
#注释:'-'号的作用在于加载错误时，会继续执行make,其主要是指首次make时，目录中还不存在'*.d'文件，所以此时加载会产生错误
-include $(DEPS)

# 生成的可执行程序名称
BIN = message_passing_frame
# 生成的库名(静态库以lib为前缀,.a为后缀)注释则不生成
# LIB = libhttp_server.a


all: mk_build_dir $(SRC_OBJ) $(BIN) $(LIB)


mk_build_dir:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
	mkdir -p $(BUILDDIR)

$(SRC_OBJ): $(OBJDIR)/%.o : $(shell dirname `pwd`)/%.cpp
	mkdir -p $(shell dirname $@)
	$(CC) -c $(CPPFLAG) $< -o $@ -MD -MF $(patsubst %.o, %.d, $@) -MP 

$(BIN):$(SRC_OBJ)
	echo $^
	$(CC) $(CPPFLAG) $^ -o $(BINDIR)/$@

$(LIB):$(SRC_OBJ)
	ar rcs  $(LIBDIR)/$(LIB) $^

# 只生成可执行文件
bin:$(BIN)

# 只生成库文件
lib:$(LIB)

run:bin
	@echo "run  $(BIN)..............."
	$(BINDIR)/$(BIN)

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	rm -rf $(BUILDDIR)
