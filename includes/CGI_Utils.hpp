/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI_Utils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skunert <skunert@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 10:05:38 by skunert           #+#    #+#             */
/*   Updated: 2024/02/26 12:42:41 by skunert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "CGI.hpp"

std::string get_exec_type(std::string const& file_path);

std::string check_exec_type(std::string const& exec_type);

std::string  add_header_cgi(int status, Statuscodes& codes);

std::string  storeFileIntoString_cgi(std::string path);

void  get_path_info_get(std::string&  exec_name, std::string& path_info, std::string& body);

void  get_path_info_post(std::string&  exec_name, std::string& path_info);
