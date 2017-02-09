NAME		=	spatch

MAKE		+=	VERBOSE=1

RM		=	rm -rfv

BUILD_DIR	=	build


all:		build package

${NAME}:	build

build:
	@echo "Building spatch from source..."
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR} && \
	cmake .. && \
	${MAKE}

package:
	@echo "Packaging spatch for debian 64bits..."
	@echo "(Did nothing)"

deployment:
	@echo "Deploying package inside testing environment..."
	@echo "(Did nothing)"

clean:
	@echo "Soft cleaning of build directory..."
	if [ -f ${BUILD_DIR}/Makefile ]; then \
		cd build && \
		${MAKE} clean; \
	fi

fclean:
	@echo "Hard cleaning of build directory..."
	${RM} ${BUILD_DIR}

re:		fclean all

.PHONY:		all ${NAME} build package clean fclean re
