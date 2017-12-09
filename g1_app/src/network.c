#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <zlog.h>
#include <MQTTAsync.h>
#include "../basic/monitor.h"
#include "../json/cJSON.h"
#include "network.h"

// 订阅
#define SUB_DEDICATE_TOPIC                     "receive/controller-%s"
#define SUB_COMMUNITY_TOPIC                    "receive/xiaoqu-%s/login-controller"
#define SUB_LOGIN_TOPIC                        "receive/login-controller"
#define SUB_UNLOGIN_TOPIC                      "receive/unlogin"
// 发布
#define PUB_TOPIC_AFTER_LOGIN                  "report/protocol-GogenCtlP-V2/controller-%s/action-%d"
#define PUB_TOPIC_BEFORE_LOGIN                 "report/protocol-GogenCtlP-V2/unlogin/action-%d"
// 服务器
//#define MQTT_BROKER_ADDRESS                     "tcp://192.168.6.89:1883"
#define MQTT_BROKER_ADDRESS                     "tcp://192.168.5.43:1883"
// 服务质量
#define MQTT_MESSAGE_QOS                        0
// connection配置
#define KEEP_ALIVE_INTERVAL                     60
#define CLEAN_SESSION_OPTION                    0
#define USER_NAME_FOR_LOGIN                     "Gogenius"
#define USER_PASSWD_FOR_LOGIN                   "Gogenius"
#define CERTIFICATES_FOR_COMMUNICATE            "/vendor/Gogenius/Gogenius.pem"

enum TOPIC_POS {
    DEDICATE_TOPIC,
    COMMUNITY_TOPIC,
    LOGIN_TOPIC,
    UNLOGIN_TOPIC
};

void normalMessageFromHost(clientServiceDef *clientService, uint8_t action, uint8_t *data, int32_t bytes)
{
    netPrivateInfoDef *netPrivateInfo = &clientService->privateInfo;
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    MQTTAsync regFd = mqttNetClient->regFd;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    zlog_category_t *cg = clientService->cg;

    int32_t rc;
    char topic[128];

    if (0 == mqttNetClient->isConnected) {
        zlog_warn(cg, "Is not connected with broker");
        return;
    }

    snprintf(topic, 127, PUB_TOPIC_AFTER_LOGIN, netPrivateInfo->controllerCode, action);

    pubmsg.payload = data;
    pubmsg.payloadlen = bytes;
    pubmsg.qos = MQTT_MESSAGE_QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTAsync_sendMessage(regFd, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Send register message fail: %d", rc);
    }
}

void onMessageDeliveryComplete(void *context, MQTTAsync_token dt)
{

}

int32_t onMessageArrived(void *context, char *topicName, int32_t topicLen, MQTTAsync_message *message)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    netClientDef *client = &clientService->client;
    readWriteMonitorDef *libPaho = &client->libPaho;
    zlog_category_t *cg = clientService->cg;

    zlog_notice(cg, "Receive message from topic: %s, data len %ld", topicName, message->payloadlen);

    uint8_t *msg = nn_allocmsg(message->payloadlen + 2, 0);
    if (NULL == msg) {
        zlog_fatal(cg, "Alloc mem fail");
        exit(-1);
    }

    msg[0] = NET_MESSAGE_FROM_BROKER;
    memcpy(msg + 1, message->payload, message->payloadlen);
    msg[message->payloadlen + 1] = '\0';

    nn_send(libPaho->nfd, &msg, NN_MSG, NN_DONTWAIT);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return 1;
}


void onSubscribeUnloginTopicSuccess(void *context, MQTTAsync_successData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    zlog_category_t *cg = clientService->cg;
    zlog_info(cg, "Subscribe unlogin topic success");
}

void onSubscribeUnloginTopicFailure(void *context, MQTTAsync_failureData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    zlog_category_t *cg = clientService->cg;
    zlog_info(cg, "Subscribe unlogin topic fail");
}

void subscribeUnloginTopicAfterConnected(void *context, char *cause)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    readWriteMonitorDef *libPaho = &client->libPaho;
    MQTTAsync regFd = mqttNetClient->regFd;
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 1;

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    int32_t rc;

    opts.onSuccess = onSubscribeUnloginTopicSuccess;
    opts.onFailure = onSubscribeUnloginTopicFailure;
    opts.context = clientService;

    if ((rc = MQTTAsync_subscribe(regFd, SUB_UNLOGIN_TOPIC, MQTT_MESSAGE_QOS, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Fail to subscribe unlogin topic");
        goto err;
    }

    // 通知主线程注册前的连接成功,主机将发送注册信息，注意重复发送到服务器
    uint8_t *msg = nn_allocmsg(4, 0);
    if (NULL == msg) {
        zlog_fatal(cg, "Alloc mem fail");
        exit(-1);
    }

    msg[0] = NET_MESSAGE_FROM_CLIENT;
    msg[1] = NET_CONNECTED_BROKER_WITH_SERIAL;
    msg[2] = 0x0;
    msg[3] = '\0';

    nn_send(libPaho->nfd, &msg, NN_MSG, NN_DONTWAIT);

err:
    return;
}

void onConnectWithDeviceCodeFailure(void *context, MQTTAsync_failureData *cause)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    mqttNetClientDef *mqttNetClient = &clientService->client.mqttNetClient;
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 0;

    zlog_warn(cg, "Network connect fail after register");
}

void onConnectWithSerialFailure(void *context, MQTTAsync_failureData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    mqttNetClientDef *mqttNetClient = &clientService->client.mqttNetClient;
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 0;

    zlog_warn(cg, "Network connect fail before register");
}

void onConnectWithSerialLost(void *context, char *cause)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    mqttNetClientDef *mqttNetClient = &clientService->client.mqttNetClient;
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 0;

    zlog_warn(cg, "Network connection lost before register");
}

void connectOptionSet(MQTTAsync_connectOptions *opts, MQTTAsync_SSLOptions *sslopts, MQTTAsync_onSuccess *onSuccess, MQTTAsync_onFailure *onFailure, void *context)
{
    opts->context = context;
    opts->username = USER_NAME_FOR_LOGIN;
    opts->password = USER_PASSWD_FOR_LOGIN;
    opts->keepAliveInterval = KEEP_ALIVE_INTERVAL;
    opts->cleansession = CLEAN_SESSION_OPTION;
    opts->onSuccess = onSuccess;
    opts->onFailure = onFailure;
    opts->automaticReconnect = 1;
    opts->minRetryInterval = 3;
    opts->maxRetryInterval = 3;
    /*opts->ssl = sslopts;*/
    /*sslopts->keyStore = CERTIFICATES_FOR_COMMUNICATE;*/
}

void connectBrokerWithSerial(clientServiceDef *clientService, uint8_t *data, int32_t bytes)
{
    netPrivateInfoDef *privateInfo = &clientService->privateInfo;
    mqttNetClientDef *mqttNetClient = &clientService->client.mqttNetClient;
    MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;

    int32_t rc;
    memcpy(privateInfo->deviceSn, data, bytes);

    zlog_category_t *cg = clientService->cg;

    // 创建
    if ((rc = MQTTAsync_create(&mqttNetClient->regFd, MQTT_BROKER_ADDRESS, privateInfo->deviceSn, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt create fail");
        goto errCreate;
    }

    // 设置回调
    if ((rc = MQTTAsync_setCallbacks(mqttNetClient->regFd, clientService, onConnectWithSerialLost, onMessageArrived, NULL)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt set callback fail");
        goto errSetCallback;
    }

    // 设置连接成功回调
    if ((rc = MQTTAsync_setConnected(mqttNetClient->regFd, clientService, subscribeUnloginTopicAfterConnected)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt set connect callback");
        goto errSetConnectCallback;
    }

    // 设置连接选项
    connectOptionSet(&opts, &sslopts, NULL, onConnectWithSerialFailure, clientService);

    // 连接
    if ((rc = MQTTAsync_connect(mqttNetClient->regFd, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt connect broker fail, error: %d", rc);
        goto errConnect;
    }

    mqttNetClient->regFdIsInit = 1;

    return;

errConnect:
errSetConnectCallback:
errSetCallback:
    MQTTAsync_destroy(&mqttNetClient->regFd);
errCreate:
    return;
}

void hostRegisterRequest(clientServiceDef *clientService, uint8_t action, uint8_t *data, int32_t bytes)
{
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    MQTTAsync regFd = mqttNetClient->regFd;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    zlog_category_t *cg = clientService->cg;

    int32_t rc;
    char topic[128];

    if (0 == mqttNetClient->isConnected) {
        zlog_warn(cg, "Is not connected with broker");
        return;
    }

    snprintf(topic, 127, PUB_TOPIC_BEFORE_LOGIN, action);

    pubmsg.payload = data;
    pubmsg.payloadlen = bytes;
    pubmsg.qos = MQTT_MESSAGE_QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTAsync_sendMessage(regFd, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Send register message fail: %d", rc);
    }
}

void onConnectWithDeviceCodeLost(void *context, char *cause)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    mqttNetClientDef *mqttNetClient = &clientService->client.mqttNetClient;
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 0;

    zlog_warn(cg, "Network connection lost after register");
}

void onSubscribeUsefulTopicSuccess(void *context, MQTTAsync_successData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    zlog_category_t *cg = clientService->cg;
    zlog_info(cg, "Subscribe all topic success");
}

void onSubscribeUsefulTopicFailure(void *context, MQTTAsync_failureData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    zlog_category_t *cg = clientService->cg;
    zlog_info(cg, "Subscribe all topic fail");
}

void subscribeUsefulTopicAfterConnected(void *context, char *cause)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    readWriteMonitorDef *libPaho = &client->libPaho;
    MQTTAsync loginFd = mqttNetClient->loginFd;
    char *topic[4] = {mqttNetClient->topic[0], mqttNetClient->topic[1], mqttNetClient->topic[2], mqttNetClient->topic[3]};
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 1;

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    int32_t rc;
    int32_t qoss[4] = {MQTT_MESSAGE_QOS, MQTT_MESSAGE_QOS, MQTT_MESSAGE_QOS, MQTT_MESSAGE_QOS};

    opts.onSuccess = onSubscribeUsefulTopicSuccess;
    opts.onFailure = onSubscribeUsefulTopicFailure;
    opts.context = clientService;

    if ((rc = MQTTAsync_subscribeMany(loginFd, 4, topic, qoss, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Start to subscribe all topic fail");
        goto err;
    }

    // 通知主线程注册前的连接成功
    uint8_t *msg = nn_allocmsg(4, 0);
    if (NULL == msg) {
        zlog_fatal(cg, "Alloc mem fail");
        exit(-1);
    }

    msg[0] = NET_MESSAGE_FROM_CLIENT;
    msg[1] = NET_CONNECTED_BROKER_WITH_DEVICE_CODE;
    msg[2] = 0x0;
    msg[3] = '\0';

    nn_send(libPaho->nfd, &msg, NN_MSG, NN_DONTWAIT);

err:
    return;
}

void onDisconnectSuccess(void *context, MQTTAsync_successData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    zlog_category_t *cg = clientService->cg;

    mqttNetClient->isConnected = 0;

    zlog_notice(cg, "Disconnect with broker success");
}

void connectBrokerWithDeviceCode(clientServiceDef *clientService, uint8_t *data)
{
    // 断开与broker的连接
    netClientDef *client = &clientService->client;
    netPrivateInfoDef *privateInfo = &clientService->privateInfo;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    MQTTAsync regFd = mqttNetClient->regFd;
    zlog_category_t *cg = clientService->cg;

    MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;
    MQTTAsync_disconnectOptions discOpts = MQTTAsync_disconnectOptions_initializer;

    char (*topic)[128] = mqttNetClient->topic;
    int32_t rc;
    if (MQTTAsync_isConnected(regFd)) {
        discOpts.onSuccess = onDisconnectSuccess;
        discOpts.context = clientService;
        if ((rc = MQTTAsync_disconnect(regFd, &discOpts)) != MQTTASYNC_SUCCESS) {
            zlog_error(cg, "Failed to start disconnect: %d", rc);
            return;
        }
        while (1) {
            if (0 == mqttNetClient->isConnected)
                break;
            usleep(5000);
        }
        MQTTAsync_destroy(&mqttNetClient->regFd);
        mqttNetClient->regFdIsInit = 0;
        mqttNetClient->isConnected = 0;
    }

    // 重新用主机的deviceCode作为clientId，重新连接
    cJSON *root = cJSON_Parse(data);
    if (NULL == root) {
        zlog_error(cg, "Get root node fail");
        goto errGetRoot;
    }

    cJSON *community = cJSON_GetObjectItem(root, "community");
    if (NULL == community || cJSON_String != community->type) {
        zlog_error(cg, "Get community fail");
        goto errGetCommunity;
    }

    cJSON *controllerCode = cJSON_GetObjectItem(root, "controllerCode");
    if (NULL == controllerCode || cJSON_String != controllerCode->type) {
        zlog_error(cg, "Get controller code fail");
        goto errGetCtrlCode;
    }

    memcpy(privateInfo->community, community->valuestring, strlen(community->valuestring));
    memcpy(privateInfo->controllerCode, controllerCode->valuestring, strlen(controllerCode->valuestring));
    snprintf(topic[DEDICATE_TOPIC], 127, SUB_DEDICATE_TOPIC, controllerCode->valuestring);
    snprintf(topic[COMMUNITY_TOPIC], 127, SUB_COMMUNITY_TOPIC, community->valuestring);

    // 创建
    if ((rc = MQTTAsync_create(&mqttNetClient->loginFd, MQTT_BROKER_ADDRESS, privateInfo->controllerCode, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt create fail");
        goto errCreate;
    }

    mqttNetClient->loginFdIsInit = 1;

    // 设置回调
    if ((rc = MQTTAsync_setCallbacks(mqttNetClient->loginFd, clientService, onConnectWithDeviceCodeLost, onMessageArrived, NULL)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt set callback fail");
        goto errSetCallback;
    }

    // 设置连接成功回调
    if ((rc = MQTTAsync_setConnected(mqttNetClient->loginFd, clientService, subscribeUsefulTopicAfterConnected)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt set connect callback");
        goto errSetConnectCallback;
    }

    connectOptionSet(&opts, &sslopts, NULL, onConnectWithDeviceCodeFailure, clientService);

    if ((rc = MQTTAsync_connect(mqttNetClient->loginFd, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Mqtt connect broker fail, error: %d", rc);
        goto errConnect;
    }

    cJSON_Delete(root);

    return;

errConnect:
errSetConnectCallback:
errSetCallback:
    MQTTAsync_destroy(&mqttNetClient->loginFd);
errCreate:
errGetCtrlCode:
errGetCommunity:
    cJSON_Delete(root);
errGetRoot:
    return;
}

void hostLoginRequest(clientServiceDef *clientService, uint8_t action, uint8_t *data, int32_t bytes)
{
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    MQTTAsync loginFd = mqttNetClient->loginFd;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    zlog_category_t *cg = clientService->cg;

    int32_t rc;
    char topic[128];

    if (0 == mqttNetClient->isConnected) {
        zlog_warn(cg, "Is not connected with broker");
        return;
    }

    snprintf(topic, 127, PUB_TOPIC_BEFORE_LOGIN, action);

    pubmsg.payload = data;
    pubmsg.payloadlen = bytes;
    pubmsg.qos = MQTT_MESSAGE_QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTAsync_sendMessage(loginFd, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Send register message fail: %d", rc);
    }
}

void onUnSubscribeUnloginTopicSuccess(void *context, MQTTAsync_successData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    netClientDef *client = &clientService->client;
    zlog_category_t *cg = clientService->cg;

    zlog_notice(cg, "Unsubscribe unlogin topic success");
}

void onUnSubscribeUnloginTopicFailure(void *context, MQTTAsync_failureData *response)
{
    clientServiceDef *clientService = (clientServiceDef *)context;
    netClientDef *client = &clientService->client;
    zlog_category_t *cg = clientService->cg;

    zlog_notice(cg, "Unsubscribe unlogin topic fail");
}

void hostLoginDone(clientServiceDef *clientService, uint8_t action)
{
    netClientDef *client = &clientService->client;
    mqttNetClientDef *mqttNetClient = &client->mqttNetClient;
    MQTTAsync loginFd = mqttNetClient->loginFd;
    char (*topic)[128] = mqttNetClient->topic;
    zlog_category_t *cg = clientService->cg;

    int32_t rc;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = onUnSubscribeUnloginTopicSuccess;
    opts.onFailure = onUnSubscribeUnloginTopicFailure;
    opts.context = clientService;

    if ((rc = MQTTAsync_unsubscribe(loginFd, topic[UNLOGIN_TOPIC], &opts)) != MQTTASYNC_SUCCESS) {
        zlog_error(cg, "Fail to start unsubscribe");
    }
}

void messageFromMajorService(struct ev_loop *loop, ev_io *w, int32_t revents)
{
    readWriteMonitorDef *netService = (readWriteMonitorDef *)w;
    clientServiceDef *clientService = netService->private;

    zlog_category_t *cg = clientService->cg;

    uint8_t *data;
    int32_t bytes;

    bytes = nn_recv(netService->nfd, &data, NN_MSG, NN_DONTWAIT);
    if (0 > bytes) {
        zlog_error(cg, "Receive form major service fail");
        return;
    }

    switch (data[0]) {
        case NET_HOST_NORMAL_MESSAGE: {
            normalMessageFromHost(clientService, data[1], data + 2, bytes - 3);
            nn_freemsg(data);
            break;
        }
        case NET_CONNECT_BROKER_WITH_SERIAL: {
            connectBrokerWithSerial(clientService, data + 1, bytes - 2);
            nn_freemsg(data);
            break;
        }
        case NET_HOST_REGISTER_REQUEST: {
            hostRegisterRequest(clientService, data[1], data + 2, bytes - 3);
            nn_freemsg(data);
            break;
        }
        case NET_CONNECT_BROKER_WITH_DEVICE_CODE: {
            connectBrokerWithDeviceCode(clientService, data + 1);
            nn_freemsg(data);
            break;
        }
        case NET_HOST_LOGIN_REQUEST: {
            hostLoginRequest(clientService, data[1], data + 2, bytes - 3);
            nn_freemsg(data);
            break;
        }
        case NET_HOST_LOGIN_DONE: {
            hostLoginDone(clientService, data[1]);
            nn_freemsg(data);
            break;
        }
        default:
            nn_freemsg(data);
    }
}

void messageToMajorService(struct ev_loop *loop, ev_io *w, int32_t revents)
{

}

int32_t mqttNetClientInit(mqttNetClientDef *mqttNetClient, netClientDef *client)
{
    mqttNetClient->private = client;
    clientServiceDef *clientService = client->private;

    char (*topic)[128] = mqttNetClient->topic;
    int32_t i;

    for (i = 0; i < 4; ++i) {
        memset(topic[i], 0, 128);
    }

    memcpy(topic[LOGIN_TOPIC], SUB_LOGIN_TOPIC, strlen(SUB_LOGIN_TOPIC));
    memcpy(topic[UNLOGIN_TOPIC], SUB_UNLOGIN_TOPIC, strlen(SUB_UNLOGIN_TOPIC));

    mqttNetClient->isConnected = 0;
    mqttNetClient->regFdIsInit = 0;
    mqttNetClient->loginFdIsInit = 0;

    return 0;
}

void mqttClientDeInit(mqttNetClientDef *mqttNetClient, MQTTAsync fd, uint8_t isInit)
{
    if (MQTTAsync_isConnected(fd)) {
        MQTTAsync_disconnect(fd, NULL);
        MQTTAsync_destroy(fd);
    } else {
        if (isInit) {
            MQTTAsync_destroy(fd);
        }
    }
}

void mqttNetClientDeInit(mqttNetClientDef *mqttNetClient)
{
    mqttClientDeInit(mqttNetClient, mqttNetClient->loginFd, mqttNetClient->loginFdIsInit);
    mqttClientDeInit(mqttNetClient, mqttNetClient->regFd, mqttNetClient->regFdIsInit);
}

int32_t netClientInit(netClientDef *client, clientServiceDef *clientService)
{
    client->private = clientService;
    zlog_category_t *cg = clientService->cg;

    if (-1 == readWriteMonitorInit(clientService->loop, &client->libPaho, NN_PAIR, 0, PAIR_NETCLIENT_TO_MAJOR, messageToMajorService, clientService)) {
        zlog_error(cg, "Monitor to major init fail");
        goto errToMajorInit;
    }

    if (-1 == readWriteMonitorInit(clientService->loop, &client->netService, NN_PAIR, 0, PAIR_NETCLIENT_FROM_MAJOR, messageFromMajorService, clientService)) {
        zlog_error(cg, "Monitor from major init fail");
        goto errFromMajorInit;
    }

    if (-1 == mqttNetClientInit(&client->mqttNetClient, client)) {
        zlog_error(cg, "Mqtt network client init fail");
        goto errMqttClient;
    }

    return 0;

errMqttClient:
    readWriteMonitorDeInit(clientService->loop, &client->netService);
errFromMajorInit:
    readWriteMonitorDeInit(clientService->loop, &client->libPaho);
errToMajorInit:
    return -1;
}

void netClientDeInit(netClientDef *client)
{
    clientServiceDef *clientService = client->private;

    mqttNetClientDeInit(&client->mqttNetClient);
    readWriteMonitorDeInit(clientService->loop, &client->netService);
    readWriteMonitorDeInit(clientService->loop, &client->libPaho);
}

clientServiceDef *netClientServiceInit(void)
{
    zlog_category_t *cg = zlog_get_category("netClient");
    if (NULL == cg) {
        goto end;
    }

    clientServiceDef *clientService = calloc(1, sizeof(clientServiceDef));
    if (NULL == clientService) {
        zlog_error(cg, "Client service create fail");
        goto end;
    }

    clientService->cg = cg;

    clientService->loop = ev_loop_new(0);
    if (NULL == clientService->loop) {
        zlog_error(cg, "Client event loop create fail");
        goto errLoopNew;
    }

    if (-1 == netClientInit(&clientService->client, clientService)) {
        zlog_error(cg, "Client Object init fail");
        goto errNetClientInit;
    }

    return clientService;

errNetClientInit:
    ev_loop_destroy(clientService->loop);
errLoopNew:
    free(clientService);
end:
    return NULL;
}

void netClientServiceDeInit(clientServiceDef *clientService)
{
    netClientDeInit(&clientService->client);
    ev_loop_destroy(clientService->loop);
    free(clientService);
}

void netClientServiceCleanup(void *arg)
{
    netClientServiceDeInit(arg);
}

void *netClientServiceStart(void *arg)
{
    clientServiceDef *clientService = arg;
    zlog_category_t *cg = clientService->cg;

    zlog_info(cg, "Network Service Start");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(netClientServiceCleanup, arg);
    ev_loop(clientService->loop, 0);
    pthread_cleanup_pop(0);

    return NULL;
}

