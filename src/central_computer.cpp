#include <server/udp.hpp>

#include <mutex>

#include <ncurses.h>

cookie::server::udp server(5000);
std::mutex client_mutex;

std::string car_register(std::string message, sockaddr_in client) {
    std::lock_guard<std::mutex> client_lock(client_mutex);
    for (const auto & client_address : server.client_addresses) {
        if (client_address.sin_addr.s_addr == client.sin_addr.s_addr && client_address.sin_port == client.sin_port) {
            break;
        }
    }

    server.client_addresses.push_back(client);

    return message;
}

std::string truck_register(std::string message, sockaddr_in client) {
    std::lock_guard<std::mutex> client_lock(client_mutex);
    for (const auto & client_address : server.client_addresses) {
        if (client_address.sin_addr.s_addr == client.sin_addr.s_addr && client_address.sin_port == client.sin_port) {
            break;
        }
    }

    server.client_addresses.push_back(client);

    return message;
}

[[noreturn]] int main() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    WINDOW *control_win = newwin(LINES - 12, COLS, 0, 0);
    keypad(control_win, TRUE);
    box(control_win, 0, 0);
    mvwprintw(control_win, 0, 2, "CONTROL WINDOW");

    WINDOW *client_win = newwin(12, COLS, LINES - 12, 0);
    keypad(client_win, TRUE);
    box(client_win, 0, 0);
    mvwprintw(client_win, 0, 2, "CLIENT WINDOW");


    server.map_response_word("CAR", car_register);
    server.map_response_word("TRUCK", truck_register);

    server.start();

    int speed = 0;
    std::unique_lock<std::mutex> client_lock(client_mutex);
    client_lock.unlock();

    while (true) {
        client_lock.lock();
        if (server.client_addresses.empty() == false)
            server.broadcast("SPEED " + std::to_string(speed));
        client_lock.unlock();

        refresh();
        wrefresh(control_win);
        wrefresh(client_win);

        const int ch = getch();

        if (ch == KEY_UP)
            speed = speed + 10;
        else if (ch == KEY_DOWN)
            speed = speed - 10;
        else if (ch == ' ')
            speed = 0;

        if (speed < -100)
            speed = -100;

        if (speed > 100)
            speed = 100;

        mvwprintw(control_win, 1, 2, "SPEED %d  ", speed);

        client_lock.lock();
        int line = 1;
        for (auto client_address : server.client_addresses) {
            mvwprintw(client_win, line++, 2, "CLIENT ON PORT : %d", client_address.sin_port);
        }
        client_lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}