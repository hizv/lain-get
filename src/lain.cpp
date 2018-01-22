#include <iostream>
#include <string>
#include <fstream>

#include <curl/curl.h>
#include <json/json.h>

#include <boost/program_options.hpp>
#include <boost/assign/list_of.hpp>

#include "lain.hpp"

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;

    po::options_description desc("Usage: " + std::string(argv[0]) + " [options]  <board/thread>"); // help/description

    desc.add_options() // command line options
    ("help", "produce help message")
    ("chan,c", po::value<std::string>()->default_value("lain"), "change chan")
    ("thread,t", po::value<std::string>(), "thread in format <board/thread> e.g. lit/343")
    ("type,f", po::value<std::vector <std::string>>(), "download only files of specified types")
    ("quiet,q", "do not output download messages")
    ;

    po::positional_options_description p;
    p.add("thread", -1); // makes specifying option 'thread' optional

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
            .options(desc)
            .positional(p)
            .run(), vm);

    po::notify(vm);    

    if (vm.count("help")) { // checks if help option is specified
        std::cout << desc << "\n";
        return 1;
    }

    Chan chan = cmap[vm["chan"].as<std::string>()]; // gets the respective Chan data from the chan option
    bool quiet = vm.count("quiet");

    if(vm.count("thread")) {
        URL_Info info(vm["thread"].as<std::string>(), chan);

        get_page(info.url, info.filename, quiet);

        string_map files;
        if(vm.count("type")) 
            files = get_files(info, vm["type"].as<std::vector <std::string>>());
        else
            files = get_files(info, std::vector<std::string>());

        std::string url, filename;

        for(auto iter = files.begin(); iter != files.end(); iter++){
            url = iter->second;
            filename = iter->first;

            get_page(url, filename, quiet);
        }

        bool deleted = remove(info.filename.c_str()) == 0;

        if(quiet)
            return 0;

        if(deleted)
            std::cout << "deleted " << info.filename << ".\n";
        else 
            std::cerr << "error: could not delete " << info.filename << ".\n";
        return 0;

    } else {
        std::cout << desc << '\n';
        return 1;
    }

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


