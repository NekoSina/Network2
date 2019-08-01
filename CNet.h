
#ifndef _CNET_H
#define _CNET_H

#include<iostream>
#include<cstring>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include"SDL_net.h"

typedef char charbuf[256];
class CNet{
    public:

    static bool Init();
    static void quit();
    };
class CNetMessage{
    protected:
    charbuf buffer;
    enum buffstates{
        EMPTY,
        READING,
        WRITING,
         FULL
        };
        buffstates state;
    public:
        void reset();
        CNetMessage();
        //virtual function that indicates how many bytes may have to be loaded onto the instance 
        virtual int NumToLoad();
        //virtual function that indicates how many bytes may have to be downloaded from the instance
        virtual int NumToUnLoad();

        //loads a char set into the message buffer
        void LoadBytes(charbuf& inputbuffer,int n);
        //unloads a char set from the message buffer
        void UnLoadBytes(charbuf& destbuffer);
        //finish(sets the buffstate to FULL. no more data to be loaded)
        void finish();
};
class CIpAddress{
    private:
        IPaddress m_Ip;//the IP address structure
    public:
        CIpAddress();//normal constructor
        CIpAddress(Uint16 port);//constructor with port set to the instance
        CIpAddress(std::string host, Uint16 port);//constructor with a port and a host set to the instance
        void SetIp(IPaddress sdl_ip);//set a CIpAddress object from an existing SDL IPaddress
        bool Ok()const;//true if instance has both port and host
        IPaddress GetIpAddress() const;//returns a SDL structured IP
        Uint32 GetHost() const;//returns the host
        Uint16 GetPort() const;//return the port
};
class CTcpSocket{
    protected:
        TCPsocket m_socket;//socket structure
        SDLNet_SocketSet set;//a set of sockets. Used here only to check existing packets
    public:
    CTcpSocket();
    virtual ~CTcpSocket();
    virtual void SetSocket(TCPsocket the_SDL_socket);
    bool Ok_TCP();//indicates if a Tcp socket is associated to the socket
    bool Ready();//indicates if there are bytes ready to be read
    virtual void OnReady();//pure virtual
};
class CClientSocket;
class CHostSocket : public CTcpSocket{
    public:
    CHostSocket(CIpAddress& the_ip_address);//creates and opens a new socket, with an existing CIP address object
    CHostSocket(Uint16 port);//creates and opens a new port with a desired port
    bool Accept(CClientSocket & );//set a client CTcpSocket object after listening to the port
    virtual void OnReady();//pure virtual
    bool Recieve(CNetMessage& rData);//recieves data and stores it in CNetMessage 
    bool Send(CNetMessage& sData);//sends data in CNetMessage object format(send data from a CNetMessage object)
};
class CClientSocket: public CTcpSocket{
    private: 
        CIpAddress m_remoteIp;//the CIpAddress object corresponding to the remote host
    public:
        CClientSocket();//
        CClientSocket(std::string host, Uint16 port);//creates and object and connects to a host in a given port
        bool Connect(CIpAddress &m_remoteIp);//make a connection to communicate with a remote host
        bool Connect(CHostSocket & the_listener_socket);//make a connection to communicate with a client
        void SetSocket(TCPsocket the_sdl_socket);//set a CTcpSocket object from an existing SDL_net socket
        CIpAddress getIpAddress()const;//return a CIpAddress object associated to the remote host
        virtual void OnReady();//pure virtual
        bool Recieve(CNetMessage& rData);//recieves data and stores it in CNetMessage 
        bool Send(CNetMessage& sData);//sends data in CNetMessage object format(send data from a CNetMessage object)
};
#endif
