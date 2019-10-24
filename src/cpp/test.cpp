#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>

#include "test.h"



struct SENDDATA
{
    uint8_t a:1;
    uint8_t b:1;
    uint8_t c:1;
    uint8_t d:1; // low 4 bit

    uint8_t :4; // high 4 bit
};


const char HOST[] = "localhost";
const int PORT = 1883;
const int KEEPALIVE = 1;
const int QOS = 1;

class MqttConnection mqttClient;


MqttConnection::MqttConnection(const char *id, bool clean_session)
{

}

MqttConnection::~MqttConnection()
{

}

void MqttConnection::on_connect(int rc)
{
    int mid = -1;
    subscribe(&mid, "lcc", 1);
}

void MqttConnection::on_disconnect(int rc)
{

}
void MqttConnection::on_publish(int rc)
{

}
void MqttConnection::on_message(const struct mosquitto_message* message)
{

    if (message == NULL) {
        printf("%s, %d, message is NULL \n", __func__, __LINE__);
    }
    else {
        printf("%s, %d, [mid = %d, qos = %d, retain = %d] \n", __func__, __LINE__, message->mid, message->qos, message->retain);
        printf("payloadlen = [%d] \n", message->payloadlen);
        if (message->topic != NULL) {
            printf("topic = [%s] \n", message->topic);
        }

        if (1 == message->retain && message->payload != NULL) {
            printf("payload = [%s] \n", (char*)message->payload);
        }
        else if (message->payload != NULL) {
            struct SENDDATA* data = (struct SENDDATA*)(message->payload);
            printf("payload = [%d-%d-%d] \n", data->a, data->b, data->c);
            printf("payload = [%d] \n", *((int*)(message->payload)));
            printf("payload = [0x%X] \n", *((int*)(message->payload)));
        }
    }
}
void MqttConnection::on_subscribe(int mid, int qos_count, const int* granted_qos)
{

}
void MqttConnection::on_unsubscribe(int mid)
{

}
void MqttConnection::on_log(int level, const char* str)
{

}
void MqttConnection::on_error()
{

}


void *pubishFun(void *threadid)
{
    int flag = 1;
    while (flag) {
        int num = 0;
        printf("please input send num(-1 is exit):\n");
        scanf("%d", &num);
        if (-1 == num)
        {
            flag = 0;
        }
        else {
            int mid = -1;
            struct SENDDATA sendData;
            memset(&sendData, 0, sizeof(struct SENDDATA));
            sendData.a = num;
            sendData.b = num;
            sendData.c = num;
            sendData.d = num;
            int res = mqttClient.publish(&mid, "lcc", sizeof(struct SENDDATA) , &sendData, QOS, false);
            printf("%s, %d, mosquitto_publish mid = %d, size = %d \n", __func__, __LINE__, mid, sizeof(struct SENDDATA));
        }
    }
}







int main(int argc, char* argv[])
{

    mqttClient.reinitialise("ldds", true);


    int res = mqttClient.connect(HOST);

    if (MOSQ_ERR_SUCCESS == res) {


        printf("%s, %d, mosquitto_connect success \n", __func__, __LINE__);

        pthread_t thread_t;
        pthread_create(&thread_t, NULL, pubishFun, NULL);
        while(true) {
            mqttClient.loop();
        }
        pthread_join(thread_t, NULL);

    }
    else  {
        printf("%s, %d, mosquitto_connect failed, err = %d \n", __func__, __LINE__, res);
    }

    return 0;
}
