/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njantsch <njantsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 12:35:23 by njantsch          #+#    #+#             */
/*   Updated: 2024/02/22 18:39:09 by njantsch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ServerManager.hpp"

std::vector<Config> parseConfigFile(std::string path)
{
	std::ifstream input(path, std::ifstream::in);
	std::vector<Config> configs;


	if (input.is_open() && input.good())
	{
		while (input.good())
		{
			Config tmp = Config(input);
			if (tmp.getConfig().size())
				configs.push_back(tmp);
		}
	}
	return configs;
}

ServerManager::ServerManager(std::string path)
{
  std::vector<Config> configs = parseConfigFile(path);
  // for (std::vector<Config>::iterator cfg = configs.begin(); cfg < configs.end(); ++cfg)
  // {
	// std::map<std::string, std::string> servercon = cfg->getConfig();
	// for (std::map<std::string, std::string>::iterator i = servercon.begin(); i != servercon.end(); ++i)
	// 	std::cout << "server: " << i->first << "->" << i->second << std::endl;
	// std::vector<std::map<std::string, std::string> > locations = cfg->getLocations();
	// for (std::vector<std::map<std::string, std::string> >::iterator i = locations.begin(); i != locations.end(); ++i)
	// 	for (std::map<std::string, std::string>::iterator j = i.base()->begin(); j != i.base()->end(); ++j)
	// 		std::cout << "location: " << j->first << "->" << j->second << std::endl;
  // }
  if (configs.empty())
	throw std::runtime_error("no valid server configs");
  this->clientsInit();
	for (std::vector<Config>::iterator i = configs.begin(); i < configs.end(); ++i)
	{
    	this->_servers.push_back(Server(this->_clientPollfds, this->_clientDetails, *i.base()));
	}
  this->serverLoop();
  this->cleanUpClientFds();
}

ServerManager::~ServerManager() {}

bool  ServerManager::checkRevents(size_t index)
{
  int error = 0;

  if (this->_clientPollfds[index].revents & POLLHUP)
    error = 1;
  else if (this->_clientPollfds[index].revents & POLLERR)
    error = 1;
  else if (this->_clientPollfds[index].revents & POLLNVAL)
    error = 1;
  else if (this->_clientPollfds[index].revents & POLLPRI)
    error = 1;
  if (error == 1)
  {
    this->_currentServer.removeFd(index);
    return (true);
  }
  return (false);
}

void ServerManager::handleRequest(size_t i)
{
  char buffer[10000];
  ssize_t bytesRead = recv(this->_clientPollfds[i].fd, buffer, sizeof(buffer) - 1, O_NONBLOCK);

  if (bytesRead < 0) {
    perror("recv");
    this->_currentServer.removeFd(i);
    return;
  }
  if (bytesRead == 0 && this->_clientDetails[i].getPendingReceive() == false) {
    std::cout << "Client has closed the connection" << std::endl;
    this->_currentServer.removeFd(i);
    return;
  }
  buffer[bytesRead] = '\0';
  std::cout << buffer << std::endl;
  this->_clientDetails[i].parseRequestBuffer(buffer, bytesRead);
  if (this->_clientDetails[i].getPendingReceive() == false)
    this->_clientPollfds[i].events = POLLOUT;
}

void ServerManager::serverLoop()
{
	while (true)
	{
		if (poll(this->_clientPollfds, MAX_CLIENTS, 5000) < 0)
		{
			perror("poll");
			// there needs to be a function that closes all open fds in case of a crash
			// close(this->_serverSocket);
			throw(std::runtime_error(""));
		}
		for (size_t i = 0; i < MAX_CLIENTS; i++)
		{
			if (this->_clientPollfds[i].revents == 0 || checkRevents(i) == true)
			  continue;

			if (this->_clientPollfds[i].revents == POLLIN)
			{
				if (isServerSocket(this->_clientPollfds[i].fd))
					this->_currentServer.acceptConnections();
				else
				{
					this->handleRequest(i);
					matchRequestToServer(i);
				}
			}
			else if (this->_clientPollfds[i].revents == POLLOUT)
			{
				this->_currentServer.sendAnswer(i);
			}
		} // * END OF CLIENT LOOP *
	}
}
