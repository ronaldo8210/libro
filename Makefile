CUR_DIR=/work_space/libro

SCHEDULER_DIR=${CUR_DIR}/scheduler
TASK_DIR=${CUR_DIR}/task
COMMON_DIR=${CUR_DIR}/common
CONTEXT_DIR=${CUR_DIR}/context
NETWORK_DIR=${CUR_DIR}/network
LIB_DIR=${CUR_DIR}/lib

INC_DIR= -I${CUR_DIR} \
		 -I${SCHEDULER_DIR} \
		 -I${TASK_DIR} \
		 -I${COMMON_DIR} \
		 -I${CONTEXT_DIR} \
		 -I${NETWORK_DIR}

SRC = ${wildcard ${SCHEDULER_DIR}/*.cpp} \
	  ${wildcard ${TASK_DIR}/*.cpp} \
	  ${wildcard ${NETWORK_DIR}/*.cpp} 
OBJ = ${patsubst %.cpp, %.o, ${SRC}}

TARGET=libcoro.a
CC=g++
CCFLAGS=-g -std=c++11 -lpthread -Wall ${INC_DIR}

all:${OBJ}
	ar rcs $(TARGET) $^
	mv ${TARGET} ${LIB_DIR}
	@echo "compile success!!"

${OBJ}:%.o:%.cpp
	@echo "compiling $< ==> $@"
	${CC} ${CCFLAGS} -c $< -o $@

clean:
	@rm -f ${OBJ}
	@echo "clean object files done."

	@rm -f *~
	@echo "clean tempreator files done."

	@rm -f ${LIB_DIR}/${TARGET}
	@echo "clean .lib file done."

	@echo "clean complete."
