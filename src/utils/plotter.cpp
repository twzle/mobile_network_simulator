#include "utils/plotter.hpp"

Plotter::Plotter(const std::string& plotter_path){
    this->plotter_path = plotter_path;
}

void Plotter::run(const std::string& file_path){
    try {
        std::filesystem::path original_path = std::filesystem::current_path();
        std::filesystem::current_path(plotter_path);

        std::string command = \
            ". ./venv/bin/activate && python3 main.py -f \"" + file_path + "\"";
        int result = std::system(command.c_str());

        if (result == -1) {
            std::cerr << "Ошибка запуска команды\n";
            return;
        }

        std::filesystem::current_path(original_path);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
    }
}