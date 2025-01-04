#pragma once

#include <iostream>
#include <cstdlib>
#include <filesystem>

class Plotter{    
    public:
        Plotter(const std::string& plotter_path="../Plotter/");
        void run(const std::string& file_path);
    
    private:
        std::string plotter_path;
    
};