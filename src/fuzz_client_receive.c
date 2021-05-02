#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTClient.h"

static int initialized = 0;

size_t fuzz_size = 0;
const uint8_t *fuzz_data = NULL;
int listenfd = 0;
pthread_t thread1;
char *fuzztopic = "fuzztopic";

void *ServerThread(void *ptr)
{
    int connfd;
    while(1) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        write(connfd, fuzz_data, fuzz_size);
        usleep(10);
        close(connfd);
    }
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (initialized == 0) {
        struct sockaddr_in serv_addr;
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(1883);
        bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        listen(listenfd, 2);
        pthread_create(&thread1, NULL, *ServerThread, NULL);
        initialized = 1;
    }
    fuzz_data = Data;
    fuzz_size = Size;
    MQTTClient client;
    int topiclen = strlen(fuzztopic);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message *pubmsg = malloc(sizeof(MQTTClient_message));
    MQTTClient_deliveryToken token;
    MQTTClient_create(&client, "tcp://localhost:1883", "FuzzedClient",
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (MQTTClient_connect(client, &conn_opts) == MQTTCLIENT_SUCCESS) {
        MQTTClient_receive(client, &fuzztopic, &topiclen, &pubmsg, 100);
    }
    MQTTClient_disconnect(client, 100);
    MQTTClient_destroy(&client);
    free(pubmsg);
    return 0;
}
