#include"CNet.h"

typedef char charbuf[256];


int main(int argc, char* argv[]){
    // initialization and declarations 
    CNet::Init();
    CNetMessage messager;
    bool quit;

    //ip properties
    char *serverhostname = new char;
    std::cout<<"enter host name"<<std::endl;

    std::cin>>serverhostname;
    Uint16 port;
    std::cout<<"enter port name"<<std::endl;
    std::cin>>port;
    std::cin.get();

    //client socket initialization
    CClientSocket *client1 = new CClientSocket(serverhostname, port);
    //message variables
    charbuf recieve_text; 
    charbuf send_text;

    //connecting
    quit = !(client1->Ok_TCP());
    
    //client functions
    while(!quit){
        std::cout<<"ENTER 'q' to exit the program"<<std::endl;
        std::string q;
        bool rec = true;
        bool send = true;
        if(rec){    
            if(client1->Recieve(messager)){
                messager.UnLoadBytes(recieve_text);
                std::cout<<"server = "<<recieve_text<<std::endl;
            }
            else{
                std::cout<<"recieve failed"<<std::endl;
            }
            rec = false;
        }
        if(send){
            std::cout<<"enter text: ";
            std::cin>>send_text;
            q = send_text;
            if(q == "quit"){
                std::cout<<"conversation ended"<<std::endl;
                delete client1;
                break;
            }
            std::cout<<"you: "<<send_text<<std::endl;
            messager.LoadBytes(send_text, strlen(send_text));
            if(client1->Send(messager)){
                std::cout<<"Message sent"<<std::endl;
            }
            else{
                std::cerr<<"client1->Send(messager) = false"<<std::endl;
            }
            send = false;
        }
    }

    //quiting
    serverhostname = NULL;
    delete serverhostname;
    std::cout<<"quiting sdl"<<std::endl;
    CNet::quit();
    std::cin.get();
    return 0;
}
