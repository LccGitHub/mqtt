
#include <mosquittopp.h>

class MqttConnection: public mosqpp::mosquittopp
{
public:
    MqttConnection(const char *id=NULL, bool clean_session = true);

    virtual ~MqttConnection();

    virtual void on_connect(int rc);

    virtual void on_disconnect(int rc);
    virtual void on_publish(int rc);
    virtual void on_message(const struct mosquitto_message* message);
    virtual void on_subscribe(int mid, int qos_count, const int* granted_qos);
    virtual void on_unsubscribe(int mid);
    virtual void on_log(int level, const char* str);
    virtual void on_error();
};
