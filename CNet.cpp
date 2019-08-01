#include"CNet.h"

//---------------------------------------------------------------------------
bool CNet::Init(){
    if(SDLNet_Init() < 0){
        std::cerr<<"initization error: "<<SDLNet_GetError()<<std::endl;
        return false;
    }
    else{
        return true;
    }
}
//---------------------------------------------------------------------------
void CNet::quit(){
    SDLNet_Quit();
}
//-------------------------CNetMessage---------------------------------------
CNetMessage::CNetMessage(){
    reset();
}
//---------------------------------------------------------------------------
void CNetMessage::reset(){
    for(int i = 0; i<256; i++){
        buffer[i] = 0;
    }
    state = EMPTY;
    if(state == EMPTY){
        std::cout<<"in function CNetMessage::reset(): state = "<< state <<std::endl;
    }
    else
    {
        std::cout<<state<<std::endl;
    }
    
}
//---------------------------------------------------------------------------
void CNetMessage::finish(){
    if(state == READING)
        state = FULL;     
}
//---------------------------------------------------------------------------
int CNetMessage::NumToLoad(){
    if(state == EMPTY){
        return 255;
    }
    else{
        return 0;
    }
}
//---------------------------------------------------------------------------
int CNetMessage::NumToUnLoad(){
    if(state == FULL){
        return strlen(buffer) + 1;
    }
    else{
        return 0;
    }
}
//---------------------------------------------------------------------------
void CNetMessage::LoadBytes(charbuf& inputbuffer, int n){
    for(int i = 0; i < n; i ++){
        buffer[i] = inputbuffer [i];
    }
    state = READING;
}
//---------------------------------------------------------------------------
void CNetMessage::UnLoadBytes(charbuf & destbuffer){
    for(int i = 0; i < this->NumToUnLoad(); i++){
        destbuffer[i] = buffer[i];
    }
    reset();
}
//-----------------------CIpAddress------------------------------------------
CIpAddress::CIpAddress(Uint16 port){
    if(SDLNet_ResolveHost(& m_Ip, NULL, port) < 0){
        std::cerr<<"SDLNET resolve host: "<<SDLNet_GetError()<<std::endl;
        m_Ip.host = 0;
        m_Ip.port = 0;
    }
    else{
        std::cout<<"RemoteEndpoint host name: "<<m_Ip.host<<"\nRemoteEndpoint port: "<<m_Ip.port<<std::endl;
    }
}
//---------------------------------------------------------------------------
CIpAddress::CIpAddress(std::string host,Uint16 port){
    if(SDLNet_ResolveHost(& m_Ip, host.c_str(), port)<0){
        std::cerr<<"SDLNET resolve host: "<<SDLNet_GetError()<<std::endl;
        m_Ip.host = 0;
        m_Ip.port = 0;
    }
    else{
        std::cout<<"your host: "<<m_Ip.host<<"\nyour port: "<<m_Ip.port<<std::endl;
    }
}
//---------------------------------------------------------------------------
CIpAddress::CIpAddress(){
    m_Ip.host = 0;
    m_Ip.port = 0;
}
//---------------------------------------------------------------------------
bool CIpAddress::Ok()const{
    return !(m_Ip.port == 0);
}
//---------------------------------------------------------------------------
void CIpAddress::SetIp(IPaddress sdl_ip){
    m_Ip = sdl_ip;
}
//---------------------------------------------------------------------------
IPaddress CIpAddress::GetIpAddress()const{
    return m_Ip;
}
//---------------------------------------------------------------------------
Uint32 CIpAddress::GetHost()const{
    return m_Ip.host;
}
//---------------------------------------------------------------------------
Uint16 CIpAddress::GetPort()const{
    return m_Ip.port;
}
//--------------------------CTcpSocket---------------------------------------
CTcpSocket::CTcpSocket(){
    m_socket = NULL;
    set = SDLNet_AllocSocketSet(1);
}
//---------------------------------------------------------------------------
CTcpSocket::~CTcpSocket(){
    if(!(m_socket == NULL)){
    std::cout<<"closing socket"<<std::endl;
    SDLNet_TCP_DelSocket(set, m_socket);
    SDLNet_FreeSocketSet(set);
    SDLNet_TCP_Close(m_socket);  
    std::cout<<CTcpSocket::Ok_TCP()<<std::endl;
    }
}
//---------------------------------------------------------------------------
void CTcpSocket::SetSocket(TCPsocket the_sdl_socket){
    if(!(m_socket == NULL)){
        SDLNet_TCP_DelSocket(set, m_socket);
        SDLNet_TCP_Close(m_socket);
    }
    m_socket = the_sdl_socket;
    if(SDLNet_TCP_AddSocket(set, m_socket) == -1){
        std::cerr<<"SDLNet_TCP_AddSocket: "<<SDLNet_GetError()<<std::endl;
        m_socket = NULL;
    }
}
//---------------------------------------------------------------------------
bool CTcpSocket:: Ok_TCP(){
    return !(m_socket == NULL);
}
//---------------------------------------------------------------------------
bool CTcpSocket::Ready() {
    bool rd = false;
    int numready = SDLNet_CheckSockets(set ,0);
    if(numready == -1){
        std::cerr<<"SDLNet_CheckSockets: "<<SDLNet_GetError()<<std::endl;
    }
    else{
        if(numready){
            rd = SDLNet_SocketReady(m_socket);
        }
        return rd;
    }
}
//---------------------------------------------------------------------------
void CTcpSocket::OnReady(){

}
//-------------------------CHostSocket---------------------------------------
CHostSocket::CHostSocket(CIpAddress& the_ip_address){
    CTcpSocket();
    IPaddress iph = the_ip_address.GetIpAddress();
    if(!(m_socket = SDLNet_TCP_Open(&iph))){
        SDLNet_FreeSocketSet(set);
        std::cerr<<"SDLNet_CP_Open(): "<<SDLNet_GetError();
    }
}
//---------------------------------------------------------------------------
CHostSocket::CHostSocket(Uint16 port){
    CIpAddress ipListener(port);
    if(!ipListener.Ok()){
        m_socket = NULL;
    }
    else{
        CTcpSocket();
        IPaddress iph = ipListener.GetIpAddress();
        if(!(m_socket = SDLNet_TCP_Open(&iph))){
            SDLNet_FreeSocketSet(set);
            std::cerr<<"SDLNet_TCP_Open(): "<<SDLNet_GetError();
        }
    }
}
//---------------------------------------------------------------------------
bool CHostSocket::Accept(CClientSocket& the_client_socket){
    TCPsocket cs;
    if(cs = SDLNet_TCP_Accept(m_socket)){
        the_client_socket.SetSocket(cs);
        return true;
    }
    else{
        return false;
    }
}
//---------------------------------------------------------------------------
bool CHostSocket::Recieve(CNetMessage& rData) {
//Firstly, check if there is a socket
    if (m_socket == NULL)
        return false;
    charbuf buf;
//Check if the instance can receive bytes, if it can, load the number of bytes specified by NumToLoad() virtual function
    std::cout<<"rData.NumToLoad(): "<<rData.NumToLoad()<<std::endl;
    while (rData.NumToLoad() > 0) {
        if (SDLNet_TCP_Recv(m_socket, buf, rData.NumToLoad()) > 0) {
            std::cout<<"true rData"<<std::endl;
            rData.LoadBytes(buf, rData.NumToLoad());
        }
        else {
            return false;
        }
    }
    rData.finish();
    return true;
}
 //---------------------------------------------------------------------------
bool CHostSocket::Send (CNetMessage& sData) {
//check if there is a socket
    if (m_socket == NULL)
        return false;
    charbuf buf;
    int len;
 
//Check if the instance can send bytes, if it can, unload the number of bytes specified by NumToLoad() virtual function
    while ((len = sData.NumToUnLoad()) > 0) {
        sData.UnLoadBytes (buf);
        if (SDLNet_TCP_Send(m_socket, (void *)buf, len) < len) {
            std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
void CHostSocket::OnReady(){

}
//----------------------------CClientSocket----------------------------------
CClientSocket::CClientSocket(){
    CTcpSocket();
}
//---------------------------------------------------------------------------
CClientSocket::CClientSocket(std::string host, Uint16 port){
    CIpAddress remoteip(host, port);
    if(!remoteip.Ok()){
        m_socket = NULL;
    }
    else
    {
        CTcpSocket();
        Connect(remoteip);
    }
    
}
//---------------------------------------------------------------------------
bool CClientSocket::Connect (CHostSocket& the_listener_socket) {
    return false;
}
//---------------------------------------------------------------------------
bool CClientSocket::Connect(CIpAddress &m_remoteIp){
    TCPsocket cs;
    IPaddress ip = m_remoteIp.GetIpAddress();
    if(cs = SDLNet_TCP_Open(&ip)){
        SetSocket(cs);
        return true;
    }
    else{
        std::cerr<<"SDLNet_TCP_Open(): "<<SDLNet_GetError()<<std::endl;
        return false;
    }
}
//---------------------------------------------------------------------------
void CClientSocket::SetSocket(TCPsocket the_sdl_socket){
    CTcpSocket::SetSocket(the_sdl_socket);
    IPaddress *ips;
    if(ips = SDLNet_TCP_GetPeerAddress(m_socket)){
        /* Print the address, converting it onto the host format */
        Uint32 hbo = m_remoteIp.GetHost();
        Uint16 pbo = m_remoteIp.GetPort();
        std::cout<<"Client Connected: "<<SDLNet_Read32(&hbo)<<' '<<SDLNet_Read16(&pbo)<<std::endl;
    }
    else{
        std::cerr<<"SDLNet_GetPeerAddress(): "<<SDLNet_GetError<<std::endl;
    }
}
//---------------------------------------------------------------------------
CIpAddress CClientSocket::getIpAddress () const {
    return m_remoteIp;
}
//---------------------------------------------------------------------------
bool CClientSocket::Recieve(CNetMessage& rData) {
//Firstly, check if there is a socket
    if (m_socket == NULL)
        return false;
    charbuf buf;
//Check if the instance can receive bytes, if it can, load the number of bytes specified by NumToLoad() virtual function
    std::cout<<"rData.NumToLoad(): "<<rData.NumToLoad()<<std::endl;
    while (rData.NumToLoad() > 0) {
        if (SDLNet_TCP_Recv(m_socket, buf, rData.NumToLoad()) > 0) {
            std::cout<<"true rData"<<std::endl;
            rData.LoadBytes(buf, rData.NumToLoad());
        }
        else {
            return false;
        }
    }
    rData.finish();
    return true;
}
 //---------------------------------------------------------------------------
bool CClientSocket::Send (CNetMessage& sData) {
//check if there is a socket
    if (m_socket == NULL)
        return false;
    charbuf buf;
    int len;
 
//Check if the instance can send bytes, if it can, unload the number of bytes specified by NumToLoad() virtual function
    while ((len = sData.NumToUnLoad()) > 0) {
        sData.UnLoadBytes (buf);
        if (SDLNet_TCP_Send(m_socket, (void *)buf, len) < len) {
            std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
            return false;
        }
    }
    return true;
}
 //---------------------------------------------------------------------------
void CClientSocket::OnReady() {
 
}
