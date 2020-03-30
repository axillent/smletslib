#ifndef _STAVRP_LINUX_DRV_TCP_H
#define _STAVRP_LINUX_DRV_TCP_H

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
#include <cstring>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
//#include <iostream>

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Linux {

//---------------------------------------------------------------------
// Interface Server
//---------------------------------------------------------------------
template <class Config, uint16_t t_port=0>
class TCPServerDrv {
protected:
	static struct Data {
		std::vector<uint8_t>	rx_buffer;
		uint16_t				port;
		event_base*				ev_base;
	} data;
	static void connectedCB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int sock_len, void* arg );
	static void readCB( struct bufferevent *buf_ev, void *arg );
	static void eventCB( struct bufferevent *buf_ev, short events, void *arg );
	static void errorCB( struct evconnlistener *listener, void *arg );
public:
	static void Init(uint16_t p=t_port);
	static uint8_t RXGet();
	static void RXFlush();
	static bool RXAvailable();
	static void setPort(uint16_t p) { data.port = p; }
};

//---------------------------------------------------------------------
// Interface Client
//---------------------------------------------------------------------
template <class Config>
class TCPClientDrv {
protected:
	static struct Data {
		event_base*				ev_base;
		std::vector<uint8_t>	tx_buffer;
		uint16_t				port;
		uint8_t					ip[4];
	} data;
	static void eventCB(struct bufferevent *buf_ev, short events, void *ptr) {
		if( events & BEV_EVENT_CONNECTED )
		{
			Config::Logger::Debug() << "tcp-tx client connected\n";
		}
		else if( events & BEV_EVENT_ERROR )
		{
			Config::Logger::Error() <<  "* tcp-tx error\n";
		}
	}
	static void loop() {
		event_base_loop(data.ev_base, EVLOOP_NONBLOCK);
	}
public:
	static void Init() {
		data.ev_base = event_base_new();
		data.tx_buffer.clear();
	}
	static void TXPut(uint8_t byte) { data.tx_buffer.push_back(byte); }
	static bool TXAvailable() {
		loop();
		return data.tx_buffer.empty();
	}
	static bool TXFlush() {
		struct bufferevent *buf_ev;
		struct sockaddr_in sin;
		char ip_str[16];

		sprintf(ip_str, "%d.%d.%d.%d", data.ip[0], data.ip[1], data.ip[2], data.ip[3]);

		Config::Logger::Debug() <<  "tcp-tx client connecting ip=" << ip_str << " port=" << data.port << "";

		memset( &sin, 0, sizeof(sin) );
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(ip_str);
		sin.sin_port = htons(data.port);

		buf_ev = bufferevent_socket_new( data.ev_base, -1, BEV_OPT_CLOSE_ON_FREE );
		bufferevent_setcb( buf_ev, NULL, NULL, eventCB, NULL );

		if( bufferevent_socket_connect( buf_ev, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
		{
			Config::Logger::Error() << "* tcp-tx client error\n";
			bufferevent_free( buf_ev );
			data.tx_buffer.clear();
			return false;
		}

		// --- copy data to buffer
		struct evbuffer *buf_output = bufferevent_get_output( buf_ev );
		evbuffer_add(buf_output, data.tx_buffer.data(), data.tx_buffer.size());
		data.tx_buffer.clear();

//		event_base_dispatch(data.ev_base);
		loop();

		return true;
	}
	static void setIPPort(const uint8_t* ip, uint16_t port) {
		memcpy(data.ip, ip, sizeof(data.ip));
		data.port = port;
	}
};

//---------------------------------------------------------------------
// Interface Server + Client
//---------------------------------------------------------------------
template <class Config, uint16_t t_port=0>
class TCPDrv : public TCPServerDrv<Config, t_port>, public TCPClientDrv<Config> {
public:
	static void Init(uint16_t p=t_port) {
		TCPServerDrv<Config, t_port>::Init(p);
		TCPClientDrv<Config>::Init();
	}
};


//---------------------------------------------------------------------
// Implementation
//---------------------------------------------------------------------
template <class Config> typename TCPClientDrv<Config>::Data TCPClientDrv<Config>::data;


//---------------------------------------------------------------------
// Implementation TCPServerDrv
//---------------------------------------------------------------------
template <class Config, uint16_t t_port>
typename TCPServerDrv<Config, t_port>::Data TCPServerDrv<Config, t_port>::data;

template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::Init(uint16_t port) {

	if( port ) data.port = port;

	data.ev_base = event_base_new();

	// --- tcp server
	struct evconnlistener *listener;
	struct sockaddr_in sin;

	memset( &sin, 0, sizeof(sin) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl( INADDR_ANY );
	sin.sin_port = htons(data.port);

	listener = evconnlistener_new_bind(data.ev_base, connectedCB, NULL, (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE), -1, (struct sockaddr *)&sin, sizeof(sin) );

	if( !listener ) return;

	evconnlistener_set_error_cb( listener, errorCB );

}

template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::connectedCB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int sock_len, void* arg ) {

	struct event_base *base = evconnlistener_get_base( listener );
	struct bufferevent *buf_ev = bufferevent_socket_new( base, fd, BEV_OPT_CLOSE_ON_FREE );

	bufferevent_setcb( buf_ev, readCB, NULL, eventCB, NULL );
	bufferevent_enable( buf_ev, (EV_READ | EV_WRITE) );

	struct sockaddr_in sa;
	socklen_t len = sizeof(sa);
	if( getpeername(fd, (struct sockaddr*)&sa, &len) < 0 ) {
		Config::Logger::Error() << "tcp-rx connected [getpeer error]\n";
	} else {
		Config::Logger::Debug() << "tcp-rx connected " << inet_ntoa(sa.sin_addr) << "";
	}
}
template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::readCB( struct bufferevent *buf_ev, void *arg )
{
	// --- init
	struct evbuffer *buf_input = bufferevent_get_input( buf_ev );
	//struct evbuffer *buf_output = bufferevent_get_output( buf_ev );

	// --- read all available data
//	Config::Logger::Debug() << "readCB start index=" << data.rx_buffer.size() << "";
//	std::cout << "===\n\n";
//	for(uint16_t i=0; i<data.rx_buffer.size(); i++) std::cout << (char)data.rx_buffer[i];
//	std::cout << "\n\n";

	uint8_t buffer[200];
	for(int i; (i=evbuffer_remove(buf_input, buffer, sizeof(buffer)-1)) != -1 && i;) {
		data.rx_buffer.insert(data.rx_buffer.end(), buffer, buffer+i);
	}

//	std::cout << "###\n\n";
//	for(uint16_t i=0; i<data.rx_buffer.size(); i++) std::cout << (char)data.rx_buffer[i];
//	std::cout << "\n\n";
}

template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::eventCB( struct bufferevent *buf_ev, short events, void *arg )
{
	if( events & BEV_EVENT_ERROR )
		Config::Logger::Error() << "* tcp-rx bufferevent error\n";
	if( events & (BEV_EVENT_EOF | BEV_EVENT_ERROR) )
		bufferevent_free( buf_ev );
}

template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::errorCB( struct evconnlistener *listener, void *arg )
{
	struct event_base *base = evconnlistener_get_base( listener );
	//  int error = EVUTIL_SOCKET_ERROR();
	//  fprintf( stderr, "Ошибка %d (%s) в мониторе соединений. Завершение работы.\n", error, evutil_socket_error_to_string(error));
	Config::Logger::Error() << "* tcp-rx socket error\n";
	event_base_loopexit( base, NULL );
}


template <class Config, uint16_t t_port>
uint8_t TCPServerDrv<Config, t_port>::RXGet() {
	RXAvailable();

	if( data.rx_buffer.empty() ) return 0;

	uint8_t byte = data.rx_buffer.front();
	data.rx_buffer.erase(data.rx_buffer.begin());
	return byte;
}

template <class Config, uint16_t t_port>
bool TCPServerDrv<Config, t_port>::RXAvailable() {
	event_base_loop(data.ev_base, EVLOOP_NONBLOCK);
	return data.rx_buffer.size();
}

template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::RXFlush() {
	data.rx_buffer.clear();
}

}}

#endif
