#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <mosquitto.h>
#include <iomanip>

//соединение с клиентом
void connect_callback(struct mosquitto* mosq, void *obj,int rc){
        if(rc != 0){
            std::cout<<"Client could not conect to client!"<<std::endl;
            mosquitto_destroy(mosq);
            exit(1);
        }
        else mosquitto_subscribe(mosq, NULL, "testtask/out", 2);
        std::cout<<"Server subscribed to client!"<<std::endl;
    }
    
//передача клиенту обработанного сообщения
void input_message_callback(struct mosquitto* mosq, void *obj, 
    const struct mosquitto_message* msg){

        std::cout<<"Messege received!"<<(char*)msg->payload<<std::endl;

        if(strcmp((char*)msg->payload, "quit")==0){
            mosquitto_disconnect(mosq);
        }
	//перевод запятых в точки для разпознания чисел с плавающей точкой
        std::string commatodot = (char*)msg->payload;
        for(int i=0; i<commatodot.size(); i++){
        	if(commatodot[i]==',')
        		commatodot[i]='.';
        }
        std::stringstream input_data;
        std::string output_data = "";
        input_data << commatodot; 

        std::string temp;
        float found, sum;
        std::vector<float> vdata;

        while(!input_data.eof()){

            input_data>>temp;

            if(std::stringstream(temp)>>found){
                vdata.push_back(found*1.00);
            }
            temp = "";   
        }
        //сортировка при помощи контейнера vector
        std::sort(vdata.begin(), vdata.end());


        for(auto i: vdata){
            if(i-(int)i==0){
                output_data+=std::to_string((int)i);
            }
            else output_data+=std::to_string(i);
            output_data+=' ';
        }
        //вычисление суммы
        sum = std::accumulate(vdata.begin(), vdata.end(), 0.0);
        output_data+='\n';
        output_data+=std::to_string(sum);

        int issize = output_data.size()*sizeof(char);
        char* buffer = new char[issize];
        
        strcpy(buffer, output_data.c_str());
	//отправка обработанного сообщения клиенту
        mosquitto_publish(mosq, NULL, "testtask/in", issize, buffer, 0, false);
        std::cout<<"Messege send"<<output_data<<std::endl;

        delete []buffer;
    }

int main(int argc, char*argv[]){

    int rc;
    std::string input;

    struct mosquitto* mosq; 
	//инициализация библиотеки mosquitt o
    mosquitto_lib_init();
	//создание сервера
    mosq = mosquitto_new("server-input", true, NULL);
	//соединение и обработка сообщения
    mosquitto_connect_callback_set(mosq, connect_callback);
    mosquitto_message_callback_set(mosq, input_message_callback);

    rc = mosquitto_connect(mosq, "localhost", 1883, 60);

    if(rc != 0){
        std::cout<<"Client could not conect to client!"<<std::endl;
        mosquitto_destroy(mosq);
        return -1;
    }

    std::cout<<"Connect with client!"<<std::endl;

    //ожидание в цикле получения сообщения
    while(true){
        mosquitto_loop(mosq, 10, 1);
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);

    mosquitto_lib_cleanup();


    return 0;
}
