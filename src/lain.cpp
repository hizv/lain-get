#include <iostream>
#include <string>
#include <fstream>

#include <curl/curl.h>
#include <json/json.h>

#include <boost/program_options.hpp>
#include <boost/assign/list_of.hpp>

#include "lain.hpp"

#define not_in(x,y) std::find(x.begin(), x.end(), y) == x.end() // for checking if the extension will be in the filetypes specified 

URL_Info::URL_Info(std::string url, Chan chan)
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

void URL_Info::lain_expand() {
    if(board == "pr") // programming board
        board = "λ"; 
    else if(board == "g") // tech board
        board = "Ω";
    else if(board == "diy") 
        board = "Δ";
}


void get_page(std::string& url, std::string& filename, bool quiet)
{
    CURL* curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    FILE* file = fopen(filename.c_str(), "w");

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) ;
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    fclose(file);

    if(!quiet)
        std::cout << "downloaded " << filename << ".\n";
}


string_map get_files(URL_Info& info, std::vector<std::string> filetypes) // downloads all files from the URL using the url info
{
    string_map files;
    std::ifstream json_file (info.filename);
    bool filetype_specified = !filetypes.empty();
    
    Json::Value root;
    try {
        json_file >> root;  
        const Json::Value posts = root["posts"];

        std::string name, ext, tim, filename;

        for(auto post : posts)
        {
            name = post["filename"].asString();

            if(!name.empty())
            {
                ext = post["ext"].asString();
                if(filetype_specified && not_in(filetypes, ext.substr(1)))
                    goto extras;

                tim = post["tim"].asString();
                filename = name + ext;
                
                if(info.chan.has_src)
                    files[filename] = info.chan.src_url + "/" + info.board + "/src/"+ tim + ext;
                else
                    files[filename] = info.chan.src_url + tim + ext;
                
                extras:
                auto extras = post["extra_files"];

                if(!extras.isNull()) {
                    for(auto extra: extras){
                            ext = extra["ext"].asString();
                            if(filetype_specified && not_in(filetypes, ext.substr(1)))
                                continue;

                            filename = extra["filename"].asString();
                            filename = name + ext;

                            if(info.chan.has_src)
                                files[filename] = info.chan.src_url + "/" + info.board + "/src/"+ tim + ext;
                            else
                                files[filename] = info.chan.src_url + tim + ext;
                        }
                    }
                } 
            }
    } 
    catch(...) {
        std::cout << "error: invalid url\n";
    }

    json_file.close();
    return files;
}


