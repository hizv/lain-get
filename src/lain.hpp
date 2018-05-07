#pragma once

#include <iostream>
#include <string>
#include <map>
#include <boost/assign/list_of.hpp>

typedef std::map<std::string, std::string> string_map;

struct Chan
{
    std::string src_url;  // lainchan and similiar sites have /src/ and /res/ between board name and filename
    std::string json_url; // while fourchan has /thread/ in between and has different servers for images and json files
    bool has_src; // for checking if the site is "special" (fourchan) or lain-like

    Chan (std::string src_url, std::string json_url, bool has_src) // specially for fourchan
        :src_url (src_url), json_url (json_url), has_src (has_src) { }

    Chan (std::string url, bool has_src) // most of the sites will require this constructor as they do not have multiple servers
        :src_url (url), has_src (has_src) { }

    Chan() {}
};


struct URL_Info
{
    std::string board;
    std::string thread;
    std::string url;
    std::string filename;

    Chan chan;

    URL_Info (std::string url, Chan chan);
};


// list of chans (to be expanded)
static std::map<std::string, Chan> cmap = boost::assign::map_list_of

        ("4",       Chan ("https://i.4cdn.org", "https://a.4cdn.org", false) )
        ("lain",    Chan ("https://lainchan.org",       true) ) 
        ("wiz",     Chan ("https://wizchan.org",        true) ) 
        ("arisu",   Chan ("https://arisuchan.jp",       true) ) 
        ("8",       Chan ("https://8ch.net",            true) ) 
        ;

// function declarations
void get_page (std::string& url, std::string& filename, bool quiet); 
string_map get_files (URL_Info& info, std::vector<std::string> filetypes);
