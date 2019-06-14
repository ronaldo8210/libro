CUR_DIR=/work_space/libro

SCHEDULER_DIR=${CUR_DIR}/scheduler
TASK_DIR=${CUR_DIR}/task
SAMPLE_DIR=${CUR_DIR}/sample
COMMON_DIR=${CUR_DIR}/common

INC_DIR= -I${CUR_DIR} \
		 -I${SCHEDULER_DIR} \
		 -I${TASK_DIR} \
		 -I${SAMPLE_DIR} \
		 -I${COMMON_DIR}

SRC = ${wildcard ${SCHEDULER_DIR}/*.cpp} \
	  ${wildcard ${TASK_DIR}/*.cpp} \
      ${wildcard ${SAMPLE_DIR}/*.cpp}
OBJ = ${patsubst %.cpp, %.o, ${SRC}}

TARGET=main
CC=g++
CCFLAGS=-g -std=c++11 -Wall ${INC_DIR}

${TARGET}: ${OBJ}
	${CC} ${OBJ} -o $@
	@echo "compile success!!"

${OBJ}:%.o:%.cpp
	@echo "compiling $< ==> $@"
	${CC} ${CCFLAGS} -c $< -o $@

clean:
	@rm -f ${OBJ}
	@echo "clean object files done."

	@rm -f *~
	@echo "clean tempreator files done."

	@rm -f ${TARGET}
	@echo "clean target files done."

	@echo "clean complete."
