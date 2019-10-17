#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <mosquitto.h>

struct MQTTCFG
{
    char *id;
};

struct MQTTCFG gMqttCfg;

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



static void onConnect(struct mosquitto* client, void *usrData, int result)
{
    int mid;
    mosquitto_subscribe(client, &mid, "lcc", QOS);
    printf("%s, %d, result = %d \n", __func__, __LINE__, result);
}

static void onDisconnect(struct mosquitto* client, void *usrData, int result)
{
    printf("%s, %d, result = %d \n", __func__, __LINE__, result);
}

static void onPushlibAck(struct mosquitto* client, void *usrData, int mid)
{
    printf("%s, %d, mid = %d \n", __func__, __LINE__, mid);
}

static void onMessage(struct mosquitto* client, void *usrData, const struct mosquitto_message * message)
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

static void onSubscribeAck(struct mosquitto* client, void *obj, int mid, int qos_count, const int* granted_qos)
{
    printf("%s, %d, mid = %d \n", __func__, __LINE__, mid);
}

void *pubishFun(void *threadid)
{
    struct mosquitto* client = (struct mosquitto*)threadid;
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
            int res = mosquitto_publish(client, &mid, "lcc", sizeof(struct SENDDATA) , &sendData, QOS, false);
            printf("%s, %d, mosquitto_publish mid = %d, size = %d \n", __func__, __LINE__, mid, sizeof(struct SENDDATA));
        }
    }
}

int main(int argc, char* argv[])
{
    memset(&gMqttCfg, 0, sizeof(struct MQTTCFG));

    mosquitto_lib_init();
    int major = -1, minor = -1, revision = -1;
    mosquitto_lib_version(&major, &minor, &revision);
    printf("mosquitt version:[major=%d, minor=%d, revision=%d]\n", major, minor, revision);


    struct mosquitto* client = mosquitto_new(gMqttCfg.id, true, NULL);
    if (client == NULL) {
        printf("%s, %d, mosquitto_new failed \n", __func__, __LINE__);
        mosquitto_lib_cleanup(); // Call to free resources associated with the library.
    }
    else {
        mosquitto_connect_callback_set(client, &onConnect);
        mosquitto_disconnect_callback_set(client, &onDisconnect);
        mosquitto_publish_callback_set(client, &onPushlibAck);
        mosquitto_message_callback_set(client, &onMessage);
        mosquitto_subscribe_callback_set(client, &onSubscribeAck);
        int res = mosquitto_connect(client, HOST, PORT, KEEPALIVE);

        if (MOSQ_ERR_SUCCESS == res) {


            printf("%s, %d, mosquitto_connect success \n", __func__, __LINE__);

            int16_t a = 0xABCD;
            int8_t* b = (int8_t*)&a;
            printf("%s, %d, a = 0x%X, b= 0x%X \n", __func__, __LINE__, a, *b);


            pthread_t thread_t;
            pthread_create(&thread_t, NULL, pubishFun, client);
            mosquitto_loop_start(client);
            pthread_join(thread_t, NULL);
            //
        }
        else  {
            printf("%s, %d, mosquitto_connect failed, err = %d \n", __func__, __LINE__, res);
        }

        mosquitto_destroy(client);
        mosquitto_lib_cleanup();
    }


    return 0;
}
