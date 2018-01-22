#pragma once

#include <iostream>
#include <string>
#include <map>
#include <boost/assign/list_of.hpp>

typedef std::map<std::string, std::string> string_map;

struct Chan {
    std::string src_url;  // lainchan and similiar sites have /src/ and /res/ between board name and filename
    std::string json_url; // while fourchan has /thread/ in between and has different servers for images and json files
    bool has_src; // for checking if the site is "special" (fourchan) or lain-like

    Chan(std::string src_url, std::string json_url, bool has_src) // specially for fourchan
        :src_url(src_url), has_src(has_src), json_url(json_url) { }

    Chan(std::string url, bool has_src) // most of the sites will require this constructor as they do not have multiple servers
        :src_url(url), has_src(has_src) { }

    Chan() {}
};

struct URL_Info {
    Chan chan;
    std::string board; 
    std::string thread;
    std::string url;
    std::string filename;

    void lain_expand(); // makes it easier to type non-ascii characters by replacing with alternatives

    URL_Info(std::string url, Chan chan)
        :url(url), chan(chan)
    {
            const int last_slash = url.find_last_of("/"); // for splitting the board name and the thread number
            board = url.substr(0, last_slash); 

            if(chan.src_url == "https://lainchan.org/") // lainchan has some greek board names and therefore the user can type alternative names
                lain_expand();

            thread = url.substr(last_slash + 1);
            filename = thread + ".json";

            if(chan.has_src)
                this->url = chan.src_url + board + "/res/" + filename; // lain-like else this->url = chan.json_url + board + "/thread/" + filename; // fourchan
    }
};
  
// list of chans (to be expanded)
std::map<std::string, Chan> cmap = boost::assign::map_list_of("4", Chan("https://i.4cdn.org/", "https://a.4cdn.org/", false)) // fourchan
                                                            ("lain", Chan("https://lainchan.org/", true)) // lainchan
                                                            ("wiz", Chan("https://wizchan.org/", true)) // wizchan
                                                            ;


void get_page(std::string& url, std::string& filename, bool quiet); // function declarations
string_map get_files(URL_Info& info, std::vector<std::string> filetypes);

void URL_Info::lain_expand() {
    if(board == "pr") // programming board
        board = "λ"; 
    else if(board == "g") // tech board
        board = "Ω";
    else if(board == "diy") 
        board = "Δ";
}

#define not_in(x,y) std::find(x.begin(), x.end(), y) == x.end() // for checking if the extension will be in the filetypes specified 
