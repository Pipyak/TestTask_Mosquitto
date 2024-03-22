#include <iostream>
#include <string>
#include <string.h>
#include <mosquitto.h>

//соединение с сервером
void connect_callback(struct mosquitto* mosq, void *obj,int rc){
        if(rc != 0){
            std::cout<<"Client could not conect to client!"<<std::endl;
            mosquitto_destroy(mosq);
            exit(1);
        }
        else mosquitto_subscribe(mosq, NULL, "testtask/in", 2);
        std::cout<<"Client subscribed to server!"<<std::endl;
    }
//получение от сервера обработанного сообщения
void input_message_callback(struct mosquitto* mosq, void *obj, 
    const struct mosquitto_message* msg){
        std::string message = (char*)msg->payload;
        std::cout<<message; 
        std::cout<<"Client receive the message!"<<std::endl;
    }

int main(int argc, char*argv[]){

    int rc;
    std::string input;
    struct mosquitto* mosq;
    mosquitto_lib_init();
//создание клиента
    mosq = mosquitto_new("client-input", true, NULL);
//соединение и передача сообщения
    mosquitto_connect_callback_set(mosq, connect_callback);
    mosquitto_message_callback_set(mosq, input_message_callback);

    rc = mosquitto_connect(mosq, "localhost", 1883, 60);
    if(rc != 0){
        std::cout<<"Client could not conect to brocker!"<<std::endl;
        mosquitto_destroy(mosq);
        return -1;
    }
//ввод сообщение, требующего обработки, с консоли
    std::getline(std::cin, input);
    int issize = input.size()*sizeof(char);

    //так как mosquitto не работает со строками типа string переведём
    //ведённые с клавиатуры данные в статический массив
    char* buffer = new char[issize];
    strcpy(buffer, input.c_str());


    std::cout<<"Connect with brocker!"<<std::endl;
	//отправка сообщения серверу
    mosquitto_publish(mosq, NULL, "testtask/out", issize, buffer, 0, false);
	//ожидание в цикле принятия от сервера обработанного сообщения
    while (true)
    {
        mosquitto_loop(mosq, 10, 1);
    }

    std::cout<<"Client is over!"<<std::endl;

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);

    mosquitto_lib_cleanup();


    delete[] buffer;
    return 0;
}

