#include <server/udp.hpp>

#include <mutex>
#include <chrono>

#include <ncurses.h>

cookie::server::udp server(5000);
std::mutex client_mutex;

std::map<uint16_t, std::string> client_types;
std::map<uint16_t, std::chrono::steady_clock::time_point> client_last_seen;

std::string car_register(std::string message, sockaddr_in client) {
    std::lock_guard<std::mutex> client_lock(client_mutex);
    for (const auto & client_address : server.client_addresses) {
        if (client_address.sin_addr.s_addr == client.sin_addr.s_addr && client_address.sin_port == client.sin_port) {
            break;
        }
    }

    server.client_addresses.push_back(client);
    client_types[client.sin_port] = "CAR";
    client_last_seen[client.sin_port] = std::chrono::steady_clock::now();

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
    client_types[client.sin_port] = "TRUCK";
    client_last_seen[client.sin_port] = std::chrono::steady_clock::now();

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
            const std::string type = client_types[client_address.sin_port];
            const long seconds_ago = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - client_last_seen[client_address.sin_port]).count();
            mvwprintw(client_win, line++, 2, "%s ON PORT : %d, last info : %ld ms", type.c_str(), client_address.sin_port, seconds_ago);

            if (line > 10) break;
        }
        client_lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}