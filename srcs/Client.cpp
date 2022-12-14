/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:13:06 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/17 20:02:16 by nsartral         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void)
{
	return ;
}

Client::Client(std::pair<int, unsigned long> const & interface): _event(), _socketFD(0), _portNumber(), _ipAddress(), _request(), _response(), _keepAlive(60), _nbRequest(0), _lastConnection(std::time(NULL)), _interface(interface)
{
	return ;
}

Client::~Client(void)
{
	return ;
}

Client::Client(Client const & src): _event(src._event), _socketFD(src._socketFD), _portNumber(src._portNumber), _ipAddress(src._ipAddress), _request(src._request), _response(src._response), _keepAlive(src._keepAlive), _nbRequest(src._nbRequest), _lastConnection(src._lastConnection), _interface(src._interface)
{
	return ;
}

Client & Client::operator=(Client const & rhs)
{
	if (this == &rhs)
		return (*this);
	this->_event = rhs._event;
	this->_socketFD = rhs._socketFD;
	this->_portNumber = rhs._portNumber;
	this->_ipAddress = rhs._ipAddress;
	this->_request = rhs._request;
	this->_response = rhs._response;
	this->_keepAlive = rhs._keepAlive;
	this->_nbRequest = rhs._nbRequest;
	this->_lastConnection = rhs._lastConnection;
	this->_interface = rhs._interface;
	return (*this);
}

std::pair<int, unsigned long>	Client::getInterface(void)
{
	return (this->_interface);
}

void	Client::setInterface(std::pair<int, unsigned long> interface)
{
	this->_interface = interface;
}

struct epoll_event &	Client::getEvent(void)
{
	return (this->_event);
}

void	Client::setEvent(struct epoll_event & event)
{
	this->_event = event;
}

int &	Client::getSocketFD(void)
{
	return (this->_socketFD);
}

void	Client::setSocketFD(int fd)
{
	this->_socketFD = fd;
}

int &	Client::getPortNumber(void)
{
	return (this->_portNumber);
}

void	Client::setPortNumber(int portNumber)
{
	this->_portNumber = portNumber;
}

unsigned long &	Client::getIpAddress(void)
{
	return (this->_ipAddress);
}

void	Client::setIpAddress(unsigned long address)
{
	this->_ipAddress = address;
}

Request &	Client::getRequest(void)
{
	this->_lastConnection = std::time(NULL);
	return (this->_request);
}

void	Client::resetRequest(void)
{
	Request	resetter;
	this->_request = resetter;
}

Response &	Client::getResponse(void)
{
	this->_lastConnection = std::time(NULL);
	return (this->_response);
}

void	Client::resetResponse(void)
{
	Response	resetter;
	this->_response = resetter;
}

int	Client::addToRequest(std::string packet)
{
	this->_lastConnection = std::time(NULL);
	return (this->_request.parseChunk(packet));
}

int &	Client::getKeepAlive(void)
{
	return (this->_keepAlive);
}

void	Client::setKeepAlive(int keepAlive)
{
	this->_keepAlive = keepAlive;
}

unsigned int	Client::getNbRequest(void)
{
	return (this->_nbRequest);
}

void	Client::addNbRequest(void)
{
	this->_nbRequest++;
}
std::time_t	Client::getLastConnection(void)
{
	return (this->_lastConnection);
}
