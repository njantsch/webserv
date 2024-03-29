/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rnauke <rnauke@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 18:12:35 by rnauke            #+#    #+#             */
/*   Updated: 2024/02/26 16:25:16 by rnauke           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/Config.hpp"

// static int ft_stoi(const std::string& str)
// {
// 	int num;
// 	std::stringstream ss(str);

// 	ss >> num;
// 	return num;
// }

// bool Config::locationExists(const std::string& uri)
// {
// 	for (std::vector<std::map<std::string, std::string> >::iterator i = _locations.begin(); i != _locations.end(); ++i)
// 	{

// 	}
// }

bool lessThanShort(const std::string& str)
{
	if (ft_stoll(str) > 0)
		return isLessThanMaxValue(ft_stoll(str), short());
	else
		return false;
}

bool lessThanSizet(const std::string& str)
{
	return isLessThanMaxValue(ft_stosize(str), size_t());
}

void addToMap(std::map<std::string, std::string>& map, const std::string& key, const std::string& value)
{
	if (map.find(key) != map.end())
		throw std::runtime_error("duplicate parameter");
	map.insert(std::make_pair(key, value));
}

// std::string trim(const std::string& line)
// {
// 	std::string::const_iterator start = std::find_if_not(line.begin(), line.end(), isspace);
// 	std::string::const_iterator end = std::find_if_not(line.rbegin(), line.rend(), isspace).base();
// 	if (start >= end)
// 		return std::string();
// 	return std::string(start, end);
// }


std::string trim(const std::string& line)
{
    std::string trimmed = line;

    size_t start = 0;
    while (start < trimmed.length() && std::isspace(trimmed[start])) {
        ++start;
    }
    trimmed.erase(0, start);
    size_t end = trimmed.length();
    while (end > 0 && std::isspace(trimmed[end - 1])) {
        --end;
    }
    trimmed.erase(end);
    return trimmed;
}

// advances file descriptor until theres a opening bracket of a directive
void Config::findOpeningBracket(std::ifstream& input, std::string& line)
{
	while (input.good())
	{
		std::getline(input, line);
		line = trim(line);
		if (line.empty() || line.at(0) == '#')
			continue;
		if (line.find('{') != std::string::npos)
		{
			if (!trim(line.substr(0, line.find('{'))).empty())
				throw std::runtime_error("invalid config throw");
			return;
		}
		else
			throw std::runtime_error("missing opening bracket");
	}
}

bool isNumeric(const std::string& str)
{
    if (str.empty())
        return false;
    for (std::string::const_iterator c = str.begin(); c != str.end(); ++c)
        if (!std::isdigit(*c))
            return false;
    return true;
}


bool Config::locationExists(std::string uri)
{
	if (uri.find('{') != std::string::npos)
		uri.erase(uri.length() - 1);
	uri = trim(uri);
	for (t_confVector::iterator i = _locations.begin(); i < _locations.end(); ++i)
	{
		std::map<std::string, std::string>::iterator a = (*i).find("uri");
		if (a != (*i).end())
			if (a->second == uri)
				return true;
	}
	return false;
}

void addErrorCodes(std::map<std::string, std::string>& map)
{
	std::string arr[] = {"400", "403", "404", "405", "413", "500", "501", "505", "508"};
	std::vector<std::string> ec(arr, arr + sizeof(arr)/sizeof(std::string));
    for (std::vector<std::string>::iterator code = ec.begin(); code != ec.end(); ++code)
        if (map.find(*code) == map.end())
            map.insert(std::make_pair(*code, *code + ".html"));
}

// verifies the config and makes sure nothing is undefined
void Config::checkLocation(std::map<std::string, std::string>& map)
{
	if (map.find("max_client_body") == map.end())
		map.insert(std::make_pair("max_client_body", _config.find("max_client_body")->second));
	if (map.find("autoindex") == map.end())
		map.insert(std::make_pair("autoindex", "off"));
	else if (map.find("autoindex")->second != "on")
		map.insert(std::make_pair("autoindex", "off"));
	if (map.find("redirect") == map.end())
		map.insert(std::make_pair("redirect", ""));
	if (map.find("root") == map.end())
		map.insert(std::make_pair("root", _config.find("root")->second));
	if (map.find("index") == map.end())
		map.insert(std::make_pair("index", ""));
	if (map.find("deny_methods") == map.end())
		map.insert(std::make_pair("deny_methods", ""));
	if (map.find("enable_cgi") == map.end())
		map.insert(std::make_pair("enable_cgi", ""));
	addErrorCodes(map);
}

// verifies the config and makes sure nothing is undefined
void Config::checkConf()
{
	if (_config.find("listen") == _config.end())
		_config.insert(std::make_pair("listen", "80"));
	else if (!lessThanShort(_config.find("listen")->second))
		throw std::runtime_error("invalid port provided");
	if (_config.find("max_client_body") == _config.end())
		_config.insert(std::make_pair("max_client_body", "1844674407370954"));
	else if (!lessThanSizet(_config.find("listen")->second))
		throw std::runtime_error("invalid port provided");
	if (_config.find("server_name") == _config.end())
		_config.insert(std::make_pair("server_name", ""));
	if (_config.find("root") == _config.end())
		_config.insert(std::make_pair("root", "./responseFiles"));
	if (_config.find("index") == _config.end())
		_config.insert(std::make_pair("index", "index.html"));
	addErrorCodes(_error_pages);
}

void errorPageName(std::map<std::string, std::string>& map, std::string line)
{
	size_t delim;
	if ((delim = line.find(' ')) == std::string::npos)
		throw std::runtime_error("Config: expected 2 arguments: error_page <number> <filename>");
	addToMap(map, line.substr(0, delim), line.substr(delim +1, delim-line.length()));
}

// handles parsing of the location directive
std::map<std::string,std::string> Config::locationDirective(std::ifstream& input, std::string& line)
{
	std::string a[] = {"autoindex ", "deny_methods ", "root ", "index ", "redirect ", "max_client_body ", "error_page "};
	std::vector<std::string> params(a, a + sizeof(a)/sizeof(std::string));
	std::vector<std::string>::iterator i;
	std::map<std::string,std::string> map;

	std::string key = "location ";
	if (line.find('{') != std::string::npos)
		addToMap(map, "uri", trim(line.substr(key.length(), line.length() - key.length() - 1)));
	else
	{
		addToMap(map, "uri", trim(line.substr(key.length(), line.length() - key.length())));
		findOpeningBracket(input, line);
	}
	while (input.good())
	{
		std::getline(input, line);
		line = trim(line);
		if (line.empty() || line.at(0) == '#')
			continue;
		for (i = params.begin(); i != params.end(); ++i)
		{
			if ((line.compare(0, i.base()->length(), *i)) == 0 || line.find('}') != std::string::npos)
			{
				size_t delim = line.find(';');
				if (line.find('}') < delim)
					return map;
				if (line.substr(i.base()->length(), line.length()).find(';') == std::string::npos)
					throw std::runtime_error("missing semicolon in location directive");
				else if (trim(line.substr(0, line.find(' '))) == "error_page")
					errorPageName(map, trim(line.substr(i.base()->length(), delim-i.base()->length())));
				else
					addToMap(map, trim(*i), trim(line.substr(i.base()->length(), delim-i.base()->length())));
				break;
			}
			else if (i == params.end())
				throw std::runtime_error("invalid argument in location directive");
		}
	}
	throw std::runtime_error("missing closing bracket in location directive");
}

// handles parsing of the server directive
std::map<std::string, std::string> Config::serverDirective(std::ifstream& input)
{
	std::string line;
	std::string a[] = {"server_name ", "listen ", "location ", "root ", "index ", "max_client_body ", "error_page "};
	std::vector<std::string> params(a, a + sizeof(a)/sizeof(std::string));
	std::vector<std::string>::iterator i;
	std::map<std::string, std::string> map;

	while (input.good())
	{
		std::getline(input, line);
		line = trim(line);
		if (line.empty() || line.at(0) == '#')
			continue;
		for (i = params.begin(); i != params.end(); ++i)
		{
			if ((line.compare(0, i.base()->length(), *i)) == 0 || line.find('}') != std::string::npos)
			{
				size_t delim = line.find(';');
				if (line.find('}') < delim)
					return map;
				if (trim(line.substr(0, line.find(' '))) == "location")
				{
					if (locationExists(trim(line.substr((*i).length(), line.length() - (*i).length()))))
						throw std::runtime_error("duplicate location directive");
					_locations.push_back(locationDirective(input, line));
					break;
				}
				else if (line.substr(i.base()->length(), line.length()).find(';') == std::string::npos)
					throw std::runtime_error("missing semicolon in server directive");
				else if (trim(line.substr(0, line.find(' '))) == "error_page")
					errorPageName(_error_pages, trim(line.substr(i.base()->length(), delim-i.base()->length())));
				else
					addToMap(map, trim(*i), trim(line.substr(i.base()->length(), delim-i.base()->length())));
				break;
			}
			else if (i == params.end())
				throw std::runtime_error("invalid argument server directive");
		}
	}
	return map;
}

t_confVector& Config::getLocations()
{
	return _locations;
}

std::map<std::string, std::string>& Config::getConfig()
{
	return _config;
}

std::map<std::string, std::string>& Config::getErrorPages()
{
  return _error_pages;
}

bool Config::findNextServerDirective(std::ifstream& input, std::string& line)
{
	if (input.is_open() && input.good())
	{
		while (input.good())
		{
			std::getline(input, line);
			line = trim(line);
			if (line.empty() || line.at(0) == '#')
				continue;
			if (((line.find('{') == std::string::npos) && line == "server"))
			{
				findOpeningBracket(input, line);
				return true;
			}
			if ((line.find('{') != std::string::npos) && trim(line.substr(0, line.length()-1)) == "server")
				return true;
		}
	}
	return false;
}

Config::Config(std::ifstream& input)
{
	std::string line;

	try
	{
		if (findNextServerDirective(input, line))
		{
			_config = serverDirective(input);
			checkConf();
			for (t_confVector::iterator i = _locations.begin(); i < _locations.end(); ++i)
			{
				checkLocation(*i);
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

Config::~Config()
{
	// std::cout << "Config destructed" << std::endl;
}
