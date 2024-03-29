#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

size_t  ft_strlen(const char* str){
  size_t  len = 0;
  while (str[len])
    len++;
  return (len);
}

std::string ft_itos(size_t num)
{
  std::ostringstream oss;
  oss << num;
  return (oss.str());
}

std::string  check_and_add_header(std::string const& length){
  std::ostringstream header;
  header << "HTTP/1.1 " << 201 << " Created"  << "\r\n";
  header << "Content-Type: text/html\r\n";
  header << "Content-Length: " << length << "\r\n";
  header << "Connection: keep-alive" << "\r\n";
  header << "\r\n";
  return (header.str());
}

std::string  storeFileIntoString(std::string path)
{
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

std::string get_basic_html(std::string first_name, std::string last_name){
    if (first_name.find('%') != std::string::npos || last_name.find('%') != std::string::npos){
      std::string msg = storeFileIntoString("responseFiles/teapot.html");
      msg = check_and_add_header(ft_itos(msg.size())) + msg;
      return (msg);
    }
    std::string str =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>You're next!</title>\n"
    "    <style>\n"
    "        body {\n"
    "        margin: 0;\n"
    "        overflow: hidden;\n"
    "        display: flex;\n"
    "        align-items: center;\n"
    "        justify-content: center;\n"
    "        height: 100vh;\n"
    "        }\n"
    "        img {\n"
    "            width: 100%;\n"
    "            height: 100%;\n"
    "            object-fit: cover;\n"
    "            position: absolute;\n"
    "            top: 0;\n"
    "            left: 0;\n"
    "            z-index: -1;"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">\n"
    "<body>\n"
    "\n"
    "    <h1>You're next ";
    str.append(first_name);
    str.append(" ");
    str.append(last_name);
    std::string str2 = "!</h1>\n"
    "    <img src=\"https://www.icegif.com/wp-content/uploads/2023/02/icegif-1549.gif\" alt=\"Full Screen GIF\">\n"
    "\n"
    "</body>\n"
    "</html>\n";
    std::string msg = str.append(str2);
    msg = check_and_add_header(ft_itos(msg.size())) + msg;
    return (msg);
}

int main (int argc, char** argv) {
  if (argc != 4)
    return (1);
  std::string str = get_basic_html(argv[2], argv[3]);
  char const*msg = str.c_str();
  send(std::atoi(argv[1]), msg, ft_strlen(msg), 0);
  return 0;
}
