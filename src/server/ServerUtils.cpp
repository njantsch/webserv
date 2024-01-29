/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njantsch <njantsch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 14:35:15 by njantsch          #+#    #+#             */
/*   Updated: 2024/01/29 14:15:35 by njantsch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void  Server::cleanUpClientFds()
{
  for (size_t i = 0; i < this->_nfds; i++)
    close(this->_clientPollfds[i].fd);
}

void  Server::removeAndCompressFds(int i)
{
  close(this->_clientPollfds[i].fd);
  for (size_t j = i; j != 0 && j < this->_nfds - 1; j++)
  {
    this->_clientPollfds[j] = this->_clientPollfds[j + 1];
    this->_timestamp[j] = this->_timestamp[j + 1];
  }
  this->_nfds--;
}

void  Server::checkClientTimeout(int i)
{
  if (i != 0 && (std::time(NULL) - this->_timestamp[i]) >= CLIENT_TIMEOUT) {
    this->removeAndCompressFds(i);
    std::cout << "Client " << i << " timed out." << std::endl;
  }
}

std::string Server::ft_itos(int num)
{
  std::ostringstream oss;
  oss << num;
  return (oss.str());
}
