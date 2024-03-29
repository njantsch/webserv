/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njantsch <njantsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 14:35:15 by njantsch          #+#    #+#             */
/*   Updated: 2024/02/25 14:15:27 by njantsch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

int   Server::getFreeSocket()
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (this->_clientPollfds[i].fd == -1
        && this->_clientDetails[i].getFdStatus() == UNUSED)
      return (i);
	}
  return (-1);
}

void  Server::cleanUpClientFds()
{
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (this->_clientDetails[i].getFdStatus() != UNUSED)
      close(this->_clientPollfds[i].fd);
  }
}

void  Server::removeFd(int i, nfds_t& nfds)
{
  if (close(this->_clientPollfds[i].fd) == -1)
    perror("removeFd: close");
  this->_clientDetails[i].cleanUp();
  this->_clientDetails[i].cleanUpResponse();
  this->_clientDetails[i].setFdStatus(UNUSED);
  this->_clientDetails[i].setConStatus(KEEPALIVE);
  this->_clientPollfds[i].fd = -1;
  this->_clientPollfds[i].events = POLLIN;
  this->_clientPollfds[i].revents = 0;
  nfds--;
  std::cout << "Connection closed at idx: " << i << std::endl;
}

MIME_type& Server::getMimeType(void) {return (this->_data);}

Statuscodes& Server::getStatuscodes(void) {return (this->_codes);}
