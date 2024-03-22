For work you need to
1.Open 3 terminals
2.Launch server in 1st terminal with g++ server.cpp -o server -lmosquitto
3.Launch client in 2nd terminal with g++ client.cpp -o client -lmosquitto
4.Write MQTT command "mosquitto_sub -t task/in" in 3rd terminal
5.In client terminal write message
6.Result will be received in 3rd terminal
