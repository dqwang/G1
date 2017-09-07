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
	$(OBJ)g1_hal_thread.o			\
	$(OBJ)g1_hal_ut.o 			\
	$(OBJ)g1_hal_calc.o                     \
	$(OBJ)g1_hal_ntp.o                      \
	$(OBJ)g1_hal_uuid.o                     \
	$(OBJ)g1_hal_database.o                 \
	$(OBJ)g1_hal_mqtt.o			\
	$(OBJ)g1_hal_timer.o                       

default:
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_app_main.c -o $(OBJ)g1_app_main.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_keyboard.c -o $(OBJ)g1_hal_keyboard.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_led_screen.c -o $(OBJ)g1_hal_led_screen.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_log.c -o $(OBJ)g1_hal_log.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_ttyS.c -o $(OBJ)g1_hal_ttyS.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_thread.c -o $(OBJ)g1_hal_thread.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_ut.c -o $(OBJ)g1_hal_ut.o
	#$(GCC) -Wall -I$(INC) -g -c $(SRC)g1_hal_ut.c -o $(OBJ)g1_hal_ut.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_calc.c -o $(OBJ)g1_hal_calc.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_ntp.c -o $(OBJ)g1_hal_ntp.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_uuid.c -o $(OBJ)g1_hal_uuid.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_mqtt.c -o $(OBJ)g1_hal_mqtt.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_database.c -o $(OBJ)g1_hal_database.o
	$(GCC) -Wall -fshort-wchar -I$(INC) -g -c $(SRC)g1_hal_timer.c -o $(OBJ)g1_hal_timer.o
	$(GCC) -O0 -o $(EXEC) $(OBJS) -lpthread -lm
clean:
	rm -rf $(EXEC) 
	rm -rf $(OBJ)*.o
