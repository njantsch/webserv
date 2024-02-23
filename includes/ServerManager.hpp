/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njantsch <njantsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 12:35:35 by njantsch          #+#    #+#             */
/*   Updated: 2024/02/23 19:51:01 by njantsch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "Config.hpp"
#include <poll.h>
#include <vector>

class ServerManager
{
private:
  std::vector<std::map<std::string, std::string> > _configs;
  std::vector<Server>                              _servers;
  struct pollfd                                    _clientPollfds[MAX_CLIENTS];
  Clients                                          _clientDetails[MAX_CLIENTS];
  Server                                           _currentServer;

  void  handleRequest(size_t i);
  bool  checkRevents(size_t index);

  // manager utils
  bool     isServerSocket(int fd);
  void     matchRequestToServer(size_t index);
  void     clientsInit();
  void     cleanUpClientFds();

public:
  ServerManager(std::string path);
  ~ServerManager();

  void  serverLoop();
};


