#include"CNet.h"

typedef char charbuf[256];


int main(int argc, char* argv[]){
    // initialization and declarations 
    CNet::Init();
    CNetMessage messager;
    bool quit;

    //ip properties
    Uint16 port;
    std::cout<<"enter port name"<<std::endl;
    std::cin>>port;
    std::cin.get();
    CIpAddress ipc(port);

    //client/host socket initialization
    CHostSocket *host = new CHostSocket(ipc);
    CClientSocket client;
    
    //messages
    charbuf recieve_text; 
    charbuf send_text;

    
    //connecting
    quit = !(host->Ok_TCP());

    //client functions
    while(!quit){
        if(host->Accept(client)){     
            if(host->Recieve(messager)){
                messager.UnLoadBytes(recieve_text);
                std::cout<<"client = "<<recieve_text<<std::endl;
            }
            else{
                std::cout<<"recieve failed"<<std::endl;
            }
            std::cout<<"enter text: ";
            std::cin>>send_text;
            messager.LoadBytes(send_text, strlen(send_text));
            if(host->Send(messager)){
                std::cout<<"Message sent"<<std::endl;
            }
            else{
                std::cerr<<"host->Send(messager) = false"<<std::endl;
            }
                std::cout<<"end connection?(1 for yes, 0 for no)"<<std::endl;
                std::cin>>quit;
                if(quit){
                delete host;
                }
        }


        
    }

    //quiting
    delete host;
    std::cout<<"quiting sdl"<<std::endl;
    CNet::quit();
    std::cin.get();
    return 0;
}
