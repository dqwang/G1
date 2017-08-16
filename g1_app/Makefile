CROSS = mipsel-openwrt-linux-uclibc-
GCC   = $(CROSS)gcc
EXEC  = g1_app
INC   = ./inc/
SRC   = ./src/
OBJ   = ./obj/

OBJS  =						\
	$(OBJ)g1_app_main.o			\
	$(OBJ)g1_hal_keyboard.o			\
	$(OBJ)g1_hal_led_screen.o		\
	$(OBJ)g1_hal_log.o			\
	$(OBJ)g1_hal_ttyS.o                     \
	$(OBJ)g1_hal_thread.o
default:
	$(GCC) -Wall  -I$(INC) -g -c $(SRC)g1_app_main.c -o $(OBJ)g1_app_main.o
	$(GCC) -Wall  -I$(INC) -g -c $(SRC)g1_hal_keyboard.c -o $(OBJ)g1_hal_keyboard.o
	$(GCC) -Wall  -I$(INC) -g -c $(SRC)g1_hal_led_screen.c -o $(OBJ)g1_hal_led_screen.o
	$(GCC) -Wall  -I$(INC) -g -c $(SRC)g1_hal_log.c -o $(OBJ)g1_hal_log.o
	$(GCC) -Wall  -I$(INC) -g -c $(SRC)g1_hal_ttyS.c -o $(OBJ)g1_hal_ttyS.o
	$(GCC) -Wall  -I$(INC) -g -c $(SRC)g1_hal_thread.c -o $(OBJ)g1_hal_thread.o
	$(GCC) -O0 -o $(EXEC) $(OBJS)
clean:
	rm -rf $(EXEC) 
	rm -rf $(OBJ)*.o
