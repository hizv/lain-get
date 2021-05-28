#include <iostream>
#include <string>
#include <fstream>

#include <curl/curl.h>
#include <json/json.h>

#include <boost/program_options.hpp>
#include <boost/assign/list_of.hpp>

#include "lain.hpp"

int main (int argc, char* argv[])
{
    namespace po = boost::program_options;

    po::options_description desc ("Usage: " + std::string (argv[0]) + " [options]  <board/thread>"); // help/description

    desc.add_options() // command line options

        ("help", "produce help message")
        ("chan,c", po::value<std::string>()->default_value ("lain"), "change imageboard ['h' to view available options]")
        ("thread,t", po::value<std::string>(), "thread in format <board/thread> e.g. lit/343")
        ("type,f", po::value<std::vector <std::string>>(), "download only files of specified types")
        ("quiet,q", "do not output download messages")
        ("interactive,i", "interactive ui to choose threads")
    ;

    po::positional_options_description p;
    p.add ("thread", -1); // makes specifying option 'thread' optional

    po::variables_map vm;

    try {
    po::store (po::command_line_parser (argc, argv)
               .options (desc)
               .positional (p)
               .run(), vm);

    po::notify (vm);
    }
    catch ( po::error& pex ){
        std::cerr << pex.what() << '\n';
        return 1;
    }

    if (vm.count ("help") ) // checks if help option is specified
    {
        std::cout << desc << '\n';
        return 0;
    }

    if (vm["chan"].as<std::string>() == "h")
    {
        for (auto elem : cmap)
        {
            std::cout << elem.first << "\t" << elem.second.src_url << "\n";
        }
        return 0;
    }

    Chan chan = cmap[vm["chan"].as<std::string>()]; // gets the respective Chan data from the chan option
    bool quiet = vm.count ("quiet");

    if (vm.count ("thread") )
    {
        URL_Info info (vm["thread"].as<std::string>(), chan);

        get_page (info.url, info.filename, quiet);

        string_map files;
        if (vm.count ("type") )
            files = get_files (info, vm["type"].as<std::vector <std::string>>() );
        else
            files = get_files (info, std::vector<std::string>() );

        std::string url, filename;

        unsigned total_files = files.size(),
            counter = 0;

        for (auto iter = files.begin(); iter != files.end(); iter++)
        {
            url = iter->second;
            filename = iter->first;

            std::cout << '[' << ++counter << '/' << total_files << "] ";
            get_page (url, filename, quiet);
        }

        bool deleted = (remove (info.filename.c_str())  == 0 );

        if (quiet)
            return 0;

        if (deleted)
            std::cout << "deleted " << info.filename << ".\n";
        else
            std::cerr << "error: could not delete " << info.filename << ".\n";
        return 0;

    }
    else
    {
        std::cout << desc << '\n';
        return 1;
    }

}

