/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skunert <skunert@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 11:00:07 by skunert           #+#    #+#             */
/*   Updated: 2024/02/06 13:24:08 by skunert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../../includes/RequestUtils.hpp"

std::string  storeFileIntoString(RequestParser req, std::string path)
{
  if (req.getUri() == "/")
    path = req.getCurrdir() + "responseFiles/index.html";
  else
    path = req.getCurrdir() + path;
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open())
    return ("");

  std::string fileContent;
  std::ostringstream buffer;
  buffer << file.rdbuf();
  fileContent = buffer.str();
  file.close();
  return (fileContent);
}

std::string get_first_name(std::string body){
  size_t  start_pos = body.find_first_of("=") + 1;
  size_t  end_pos = body.find_first_of("&") - body.find_first_of("=") - 1;
  std::string ret_str = body.substr(start_pos, end_pos);

  return (ret_str);
}

std::string get_last_name(std::string body){
  size_t  start_pos = body.find_last_of("=") + 1;
  size_t  end_pos = body.find_last_of("&") - body.find_last_of("=") - 1;
  std::string ret_str = body.substr(start_pos, end_pos);

  return (ret_str);
}

std::string get_filecontent(std::string body){
  std::stringstream iss(body);
  std::string line;
  std::string trash;
  std::ostringstream fileContent;

  if (body[0] == '-'){
    for (int i = 0; i < 4; i++)
      std::getline(iss, trash);
  }
  else{
    for (int i = 0; !trash.find("------WebKitFormBoundary"); i++)
      (void) i;
      std::getline(iss, trash);
  }

  while (std::getline(iss, line)) {
    if (line.find("------WebKitFormBoundary") != std::string::npos)
      break;
    else if (line.empty())
      fileContent << std::endl;
    else
      fileContent << line << std::endl;
  }
  return (fileContent.str());
}

std::string get_filename(std::string body){
  std::string filename = body.substr(body.find("filename=") + 10);
  filename = filename.substr(0, filename.find('\n') - 2);
  return (filename);
}

void  handle_name_input(int fd, RequestParser req){
  char *argv[5];
  std::string cgi_filename = req.getUri().substr(req.getUri().find_last_of("/") + 1, req.getUri().size());
  std::string file_fd = std::to_string(fd);
  std::string first_name = get_first_name(req.getBody());
  std::string last_name = get_last_name(req.getBody());

  argv[0] = const_cast<char*>(cgi_filename.c_str());
  argv[1] = const_cast<char*>(file_fd.c_str());
  argv[2] = const_cast<char*>(first_name.c_str());
  argv[3] = const_cast<char*>(last_name.c_str());
  argv[4] = NULL;
  execve((req.getCurrdir() + req.getUri()).c_str(), argv, NULL);
}

void  handle_file_upload(int fd, RequestParser req, MIME_type& data, Statuscodes& codes){
  std::string filename = get_filename(req.getBody());
  if (access(("./responseFiles/Upload/" + filename).c_str(), F_OK) == 0){
    std::string msg = storeFileIntoString(req, "responseFiles/used_name.html");
    msg = check_and_add_header(201, "html", data, codes) + msg;
    if (msg != "")
      send(fd, msg.c_str(), msg.size(), 0);
    return ;
  }
  std::string filecontent = get_filecontent(req. getBody());
  std::ofstream upload(("./responseFiles/Upload/" + filename).c_str(), std::ios::binary);
  if (!upload.is_open())
    return ;
  upload.write(filecontent.c_str(), filecontent.size());
  upload.close();
  std::string msg = storeFileIntoString(req, "responseFiles/success.html");
  msg = check_and_add_header(201, "html", data, codes) + msg;
  if (msg != "")
    send(fd, msg.c_str(), msg.size(), 0);
}

std::string handle_file_erasing(RequestParser req){
  std::string msg;
  std::string filepath = req.getCurrdir() + req.getUri();
  if (access(filepath.c_str(), F_OK) != 0){
    return ("");
  }
  std::remove(filepath.c_str());
  return (filepath.substr(filepath.find_last_of('/') + 1, filepath.size() - filepath.find_last_of('/')));
}

void  handle_Request_post(int fd, RequestParser req, MIME_type& data, Statuscodes& codes){
  if (req.getUri() == "/responseFiles/first.cgi")
    handle_name_input(fd, req);
  else if (req.getUri() == "upload")
    handle_file_upload(fd, req, data, codes);
}

std::string  check_and_add_header(int status, std::string const& type, MIME_type data, Statuscodes codes){
  std::ostringstream header;
  header << "HTTP/1.1 " << status << " " << codes[status] << "\r\n";
  if (type != "No Content")
    header << "Content-Type: "<< data[type] << "\r\n";
  header << "\r\n";
  return (header.str());
}
