#ifndef __G1_HAL_MQTT_H__
#define __G1_HAL_MQTT_H__




//TODO
int hal_mqtt_register();
int hal_mqtt_heartbeat();

int hal_mqtt_recv_data_from_server_thread();
int hal_mqtt_send_data_to_server();


#endif/*__G1_HAL_MQTT_H__*/
