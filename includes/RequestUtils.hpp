/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestUtils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skunert <skunert@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 11:00:34 by skunert           #+#    #+#             */
/*   Updated: 2024/02/12 13:54:27 by skunert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "Statuscodes.hpp"
#include "MIME_type.hpp"
#include "Server.hpp"

std::string  storeFileIntoString(RequestParser& req, std::string path);

std::string get_first_name(std::string body);

std::string get_last_name(std::string body);

std::string get_filecontent(std::string boundary, std::string body);

std::string get_filename(std::string body);

void  handle_name_input(int fd, RequestParser& req);

void  handle_file_upload(int fd, RequestParser& req, MIME_type& data, Statuscodes& codes);

std::string  handle_file_erasing(int fd, RequestParser& req, Statuscodes& codes);

std::string  check_and_add_header(int status, std::string const& type, std::string const& length, \
                                  Statuscodes& codes, RequestParser& req);
