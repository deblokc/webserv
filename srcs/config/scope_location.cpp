#include "library.hpp"
#include "scope_location.hpp"

LocationScope::LocationScope(){}
LocationScope::~LocationScope(){}
LocationScope::LocationScope(std::string str){
    _chunk = str;
    _has_root = 0;
    extract_main_path();
    extract_location_blocks();
    extract_lines();
    _directive_types[0] = "error_page";
    _directive_types[1] = "client_body_buffer_size";
    _directive_types[2] = "root";
    _directive_types[3] = "allow_method";
    _directive_types[4] = "cgi";
    _directive_types[5] = "index";
    _directive_types[6] = "autoindex";
    _directive_types[7] = "limit_upload";
    _directive_types[8] = "rewrite";
    _directive_types[9] = "upload_pass";
    exec[0] = &LocationScope::extract_default_error_pages;
    exec[1] = &LocationScope::extract_client_body_buffer_size;
    exec[2] = &LocationScope::extract_root;
    exec[3] = &LocationScope::extract_allow_method;
    exec[4] = &LocationScope::extract_cgi;
    exec[5] = &LocationScope::extract_index;
    exec[6] = &LocationScope::extract_autoindex;
    exec[7] = &LocationScope::extract_limit_upload;
    exec[8] = &LocationScope::extract_rewrite;
    exec[9] = &LocationScope::extract_upload_pass;
    extract_directives();
    for(std::vector<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
    {
        _locations.push_back(LocationScope(*it));
    }
}
LocationScope& LocationScope::operator=(LocationScope const &other) {
    if (this != &other)
    {
        _main_path = other._main_path;
        _locations = other._locations;
        _index = other._index;
        _client_body_buffer_size = other._client_body_buffer_size;
        _autoindex = other._autoindex;
        _limit_upload = other._limit_upload;
        _root = other._root;
        _allow_method = other._allow_method;
        _cgi = other._cgi;
        _default_error_pages = other._default_error_pages;
        _rewrite = other._rewrite;
        _rewrite_location = other._rewrite_location;
        _upload_pass = other._upload_pass;
    }
    return *this;
}
void LocationScope::extract_client_body_buffer_size(std::string directive) {
    char    multiplier;

    directive.erase(0, directive.find("client_body_buffer_size") + 23);
    _client_body_buffer_size = directive.substr(directive.find_first_not_of("\t\v\n\r\f "), directive.find_first_of("\t\v\n\r\f ", directive.find_first_not_of("\t\v\n\r\f ")));
    multiplier = _client_body_buffer_size[_client_body_buffer_size.size() - 1];
    switch (multiplier)
    {
        case 'K' :
            _client_body_buffer_size.erase((_client_body_buffer_size.end() - 1));
            _client_body_buffer_size = multiplyStringNumbers(_client_body_buffer_size, std::string("1024"));
            break ;
        case 'M' :
            _client_body_buffer_size.erase((_client_body_buffer_size.end() - 1));
            _client_body_buffer_size = multiplyStringNumbers(_client_body_buffer_size, std::string("1048576"));
            break ;
        case 'G' :
            _client_body_buffer_size.erase((_client_body_buffer_size.end() - 1));
            _client_body_buffer_size = multiplyStringNumbers(_client_body_buffer_size, std::string("1073741824"));
            break ;
        default :
            break ;
    }
}
void LocationScope::extract_upload_pass(std::string directive) {
    directive.erase(0, directive.find("upload_pass") + 11);
    directive.erase(0, directive.find_first_not_of("\t\v\n\r\f "));
    _upload_pass = directive.substr(0, directive.find_first_of("\t\v\n\r\f "));
}
void LocationScope::extract_limit_upload(std::string limit_upload_dir) {
    limit_upload_dir.erase(0, limit_upload_dir.find("limit_upload ") + 9);
    _limit_upload = limit_upload_dir.substr(limit_upload_dir.find_first_not_of("\t\v\n\r\f ", 0), limit_upload_dir.find_first_of("\t\v\n\r\f ", limit_upload_dir.find_first_not_of("\t\v\n\r\f ", 0)));
    if (_limit_upload.compare("on") != 0)
        _limit_upload = "off";
}
void LocationScope::extract_rewrite(std::string directive) {
    directive.erase(0, directive.find("rewrite") + 7);
    directive.erase(0, directive.find_first_not_of("\t\v\n\r\f "));
    _rewrite_location = directive.substr(0, directive.find_first_of("\t\v\n\r\f "));
    directive.erase(0, _rewrite_location.size());
    if (directive.find("permanent") == std::string::npos && directive.find("redirection") == std::string::npos)
    {
        _rewrite_location.clear();
        return;        
    }
    else
    {
        if (directive.find("permanent") != std::string::npos)
            _rewrite = "permanent";
        if (directive.find("redirection") != std::string::npos)
            _rewrite = "redirection";
    }
}
void LocationScope::extract_root(std::string directive) {
    _has_root = 1;
    directive.erase(0, directive.find_first_of("root") + 4);
    directive.erase(0, directive.find_first_not_of("\t\v\n\r\f "));
    _root = directive.substr(0, directive.find_last_not_of("\t\v\n\r\f ;") + 1);
}
void LocationScope::extract_allow_method(std::string directive) {
    directive.erase(0, directive.find("allow_method") + 12);
    while (directive.find_first_of(" \t\v\n\r\f") == 0)
        directive.erase(0, 1);
    while (directive.size() != 0)
    {  
        if (directive.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            _allow_method.push_back(directive.substr(0, directive.find_first_of(" \t\v\n\r\f")));
            directive.erase(0, directive.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            _allow_method.push_back(directive.substr(0, directive.size()));
            directive.erase(0, directive.size());
        }
        while(directive.find_first_of(" \t\v\n\r\f") == 0)
            directive.erase(0, 1);
    }
}
void LocationScope::extract_cgi(std::string cgi_dir) {
    std::vector<std::string> content;
    cgi_dir.erase(0, cgi_dir.find_first_of("cgi") + 3);
    while (cgi_dir.find_first_of(" \t\v\n\r\f") == 0)
        cgi_dir.erase(0, 1);
    while (cgi_dir.size() != 0)
    {  
        if (cgi_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            content.push_back(cgi_dir.substr(0, cgi_dir.find_first_of(" \t\v\n\r\f")));
            cgi_dir.erase(0, cgi_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            content.push_back(cgi_dir.substr(0, cgi_dir.size()));
            cgi_dir.erase(0, cgi_dir.size());
        }
        while(cgi_dir.find_first_of(" \t\v\n\r\f") == 0)
            cgi_dir.erase(0, 1);
    }
    for (std::vector<std::string>::iterator it = content.begin(); it != content.end(); ++it)
    {
        if (it + 1 != content.end())
        {
            std::string tmp = *it;
            _cgi.insert(std::make_pair(tmp, *(++it)));
        }
    }
}
void LocationScope::extract_index(std::string index_dir) {
    index_dir.erase(0, index_dir.find("index") + 5);
    while (index_dir.find_first_of(" \t\v\n\r\f") == 0)
        index_dir.erase(0, 1);
    while (index_dir.size() != 0)
    {  
        if (index_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            _index.push_back(index_dir.substr(0, index_dir.find_first_of(" \t\v\n\r\f")));
            index_dir.erase(0, index_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            _index.push_back(index_dir.substr(0, index_dir.size()));
            index_dir.erase(0, index_dir.size());
        }
        while(index_dir.find_first_of(" \t\v\n\r\f") == 0)
            index_dir.erase(0, 1);
    }
}
void LocationScope::extract_default_error_pages(std::string error_page_dir) {
    error_page_dir.erase(0, error_page_dir.find("error_page") + 10);
    while (error_page_dir.find_first_of(" \t\v\n\r\f") == 0)
        error_page_dir.erase(0, 1);
    std::vector<std::string> number;
    std::string path;
    while (error_page_dir.size() != 0)
    {  
        if (error_page_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            number.push_back(error_page_dir.substr(0, error_page_dir.find_first_of(" \t\v\n\r\f")));
            error_page_dir.erase(0, error_page_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            number.push_back(error_page_dir.substr(0, error_page_dir.size()));
            error_page_dir.erase(0, error_page_dir.size());
        }
        while(error_page_dir.find_first_of(" \t\v\n\r\f") == 0)
            error_page_dir.erase(0, 1);
    }
    path = number.back();
    number.pop_back();
    for (std::vector<std::string>::iterator it = number.begin(); it != number.end(); ++it)
        _default_error_pages.insert(std::make_pair(*it, path));
}
void LocationScope::extract_autoindex(std::string autoindex_dir) {
    autoindex_dir.erase(0, autoindex_dir.find("autoindex") + 9);
    _autoindex = autoindex_dir.substr(autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0), autoindex_dir.find_first_of("\t\v\n\r\f ", autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0)));
    if (_autoindex.size() != 0)
        _autoindex = _autoindex.substr(0, _autoindex.find_last_not_of("\t\v\n\r\f ") + 1);
    if (_autoindex.compare("on") != 0)
        _autoindex = "off";
}
void LocationScope::extract_location_blocks() {  
    std::string copy = _chunk;
    while (copy.find(';', 0) != std::string::npos || copy.find('}') != std::string::npos)
    {
        if (copy.find_first_not_of("\t\v\n\r\f ") == copy.find("location") && copy.find("location") != std::string::npos)
        {
            std::string::iterator it = copy.begin();
            for (unsigned int i = 0; i < copy.find("location", 0) + 1; ++i)
                ++it;
            std::string::iterator tmp = it;
            while (it != copy.end() && *it != '{')
                ++it;
            ++it;
            int n = 1;
            while (it != copy.end() && n > 0)
            {
                if (*it == '{')
                    ++n;
                if (*it == '}')
                    --n;
                ++it;
            }
            n = 0;
            while (tmp != it)
            {
                ++n;
                ++tmp;
            }
            _location_blocks.push_back(copy.substr(copy.find("location", 0), n + 1));
            copy.erase(copy.find("location", 0), n);
        }
        if (copy.find('}', 0) == std::string::npos)
        {
            while (copy.find(';') > 0 && copy.find(';') != std::string::npos)
                copy.erase(0, 1);
            if (copy.size() > 0)
                copy.erase(0, 1);
        }
        else if (copy.find(';', 0) == std::string::npos)
        {
            while (copy.find('}') > 0 && copy.find('}') != std::string::npos)
                copy.erase(0, 1);
            if (copy.size() > 0)
                copy.erase(0, 1);
        }
        else 
        {
            if (copy.find(';', 0) < copy.find('}', 0))
            {
                while (copy.find(';') > 0 && copy.find(';') != std::string::npos)
                    copy.erase(0, 1);
                if (copy.size() > 0)
                    copy.erase(0, 1);
            }
            else
            {
                while (copy.find('}') > 0 && copy.find('}') != std::string::npos)
                    copy.erase(0, 1);   
                if (copy.size() > 0)
                    copy.erase(0, 1);
            }
        }
    }
    for (std::vector<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
        _chunk.erase(_chunk.find(*it), it->size());
}
void LocationScope::extract_lines() {
    while (_chunk.find(';', 0) != std::string::npos)
    {
        _directives.push_back(_chunk.substr(0, _chunk.find(';')));
        _chunk.erase(0, _chunk.find(';', 0) + 1);
    }
}
void LocationScope::extract_rules(std::string rule)
{
    for (unsigned int i = 0; i < 10; ++i)
    {
        if (rule.find(_directive_types[i]) == rule.find_first_not_of("\t\v\n\r\f "))
        {
            if (rule.find_first_of("\t\v\n\r\f ", rule.find(_directive_types[i])) == (rule.find(_directive_types[i]) + _directive_types[i].size()))
                (this->*exec[i])(rule);
        }
    }
}
void LocationScope::extract_main_path()
{
    _chunk.erase(0, _chunk.find_first_not_of("\t\v\n\r\f ", _chunk.find("location") + 8));
    _main_path = _chunk.substr(0, _chunk.find_last_not_of("\t\v\n\r\f {", _chunk.find('{')) + 1);
    _chunk.erase(0, _chunk.find('{') + 1);
    _chunk.erase(_chunk.find_last_of('}'), 1);
}
void LocationScope::extract_directives() {
    while (_directives.size() != 0)
    {
        extract_rules(_directives.back());
        _directives.pop_back();
    }
}
void LocationScope::apply_default() {
    if(_autoindex.size() == 0)
        _autoindex = "off";
}
void LocationScope::print_result() {
    std::ofstream ofs;
    ofs.open("./configurations/parsed.txt", std::ios_base::app);
    ofs << " \n()()()()()()()()()()()()()()()()\n" << std::endl;
    ofs << "SCOPE: LOCATION\n" << std::endl;
    ofs << "main path: " << _main_path << std::endl;
    for (std::vector<std::string>::iterator it = _index.begin(); it != _index.end(); ++it)
        ofs << "index: " << *it << std::endl;
    for (std::map<std::string, std::string>::iterator it = _default_error_pages.begin(); it != _default_error_pages.end(); ++it)
        ofs << "error page " << it->first << " associated path " << it->second << std::endl;
    ofs << "body max: " << _client_body_buffer_size << std::endl;
    ofs << "root: " << _root << std::endl;
    ofs << "allowed method: ";
    for (std::vector<std::string>::iterator it = _allow_method.begin(); it != _allow_method.end(); ++it)
        ofs << *it << ", ";
    ofs << std::endl;
    for (std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); ++it)
        ofs << "cgi: " << it->first << " associated to path " << it->second << std::endl;
    ofs << "autoindex: " << _autoindex << std::endl;
    for (std::vector<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
        ofs << "location block: " << *it << std::endl;
    ofs << std::endl;
}
