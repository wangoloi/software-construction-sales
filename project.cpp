#include "Data.h"
#include "logging.h"
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;

static const char *kLog = "resms";

static void on_sigint(int) {
    sales_log::warning(kLog, "KeyboardInterrupt — exiting (code 130)");
    std::_Exit(130);
}

static bool parse_data_line(const std::string &STRING, Data &datos) {
    if (STRING.empty()) {
        return false;
    }
    std::size_t count = 0;
    for (char c : STRING) {
        if (c == ':') {
            ++count;
        }
    }
    if (count < 5) {
        sales_log::warning(kLog, "skipped malformed data.in row (expected 5 ':' delimiters)");
        return false;
    }

    std::size_t encontro = STRING.find(':', 0);
    if (encontro == std::string::npos) {
        return false;
    }
    string agentid = STRING.substr(0, encontro);
    datos.setAgentId(agentid);

    std::size_t anterior = encontro + 1;
    encontro = STRING.find(':', anterior);
    if (encontro == std::string::npos) {
        return false;
    }
    string contact = STRING.substr(anterior, encontro - anterior);
    datos.setContact(contact);

    anterior = encontro + 1;
    encontro = STRING.find(':', anterior);
    if (encontro == std::string::npos) {
        return false;
    }
    string size = STRING.substr(anterior, encontro - anterior);
    datos.setPropertySize(size);

    anterior = encontro + 1;
    encontro = STRING.find(':', anterior);
    if (encontro == std::string::npos) {
        return false;
    }
    string roomtype = STRING.substr(anterior, encontro - anterior);
    datos.setRoomType(roomtype);

    anterior = encontro + 1;
    encontro = STRING.find(':', anterior);
    if (encontro == std::string::npos) {
        return false;
    }
    string location = STRING.substr(anterior, encontro - anterior);
    datos.setLocation(location);

    anterior = encontro + 1;
    string sellingprice = STRING.substr(anterior, STRING.length() - anterior);
    datos.setSellingPrice(sellingprice);
    return true;
}

void addProperty(std::list<Data> &l) {
    Data datos;
    string agentid, contact, size, roomtype, location, sellingprice;
    cout << "welcome to the propperty adding menu" << endl;

    cout << "Please insert your Agent ID:" << endl;
    cin >> agentid;
    datos.setAgentId(agentid);
    cout << "please insert your Contact:" << endl;
    cin >> contact;
    datos.setContact(contact);
    cout << "please insert your Property Size:" << endl;
    cin >> size;
    datos.setPropertySize(size);
    cout << "please insert your RoomType:" << endl;
    cin >> roomtype;
    datos.setRoomType(roomtype);
    cout << "please insert your Location:" << endl;
    cin >> location;
    datos.setLocation(location);
    cout << "please insert your Selling Price:" << endl;
    cin >> sellingprice;
    if (sellingprice.empty()) {
        sales_log::warning(kLog, "empty selling price rejected");
        return;
    }
    datos.setSellingPrice(sellingprice);
    l.push_back(datos);
    sales_log::info(kLog, "property record appended in memory (persist when save is implemented)");
}

void OrderList(std::list<Data> &l) {
    char resp = 0;
    do {
        cout << "What order do you want? ascending=a or descending=d" << endl;
        if (!(cin >> resp)) {
            sales_log::error(kLog, "input stream failure while reading sort order");
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        if (resp != 'a' && resp != 'd') {
            sales_log::warning(kLog, "invalid sort choice (expected 'a' or 'd')");
            cout << "ERROR, Please be careful" << endl;
        }
    } while (resp != 'a' && resp != 'd');
    l.sort();
    if (resp == 'd') {
        l.reverse();
    }
    sales_log::info(kLog, "list sorted");
}

bool findpass(const string &pass, const string &id) {
    std::ifstream passwords("passwords.in");
    if (!passwords.is_open()) {
        sales_log::error(kLog, "cannot open passwords.in for login verification");
        return false;
    }
    string pas, ag;
    while (passwords >> pas >> ag) {
        if (pass == pas && ag == id) {
            passwords.close();
            return true;
        }
    }
    passwords.close();
    return false;
}

bool login(const string &agentid, const string &password) {
    bool granted = false;
    if (findpass(password, agentid)) {
        cout << "access granted" << endl;
        granted = true;
        sales_log::info(kLog, "login succeeded");
    } else {
        cout << "\nLogin Failed!!\n";
        sales_log::warning(kLog, "login failed (bad credentials)");
    }
    return granted;
}

void changepass(const string &id) {
    std::vector<std::pair<string, string>> rows;
    {
        std::ifstream passwords("passwords.in");
        if (!passwords.is_open()) {
            sales_log::error(kLog, "cannot open passwords.in for password change");
            return;
        }
        string pas, ag;
        while (passwords >> pas >> ag) {
            rows.push_back({pas, ag});
        }
        if (passwords.bad()) {
            sales_log::error(kLog, "read error while loading passwords.in");
            return;
        }
    }

    cout << "just for security i will need you to re-login" << endl;
    cout << "what is your current pass" << endl;
    string pas;
    if (!(cin >> pas)) {
        sales_log::error(kLog, "input failure reading current password");
        return;
    }
    if (!login(id, pas)) {
        return;
    }

    cout << "access granted , please input the new password" << endl;
    string newpass;
    if (!(cin >> newpass)) {
        sales_log::error(kLog, "input failure reading new password");
        return;
    }

    bool updated = false;
    for (auto &row : rows) {
        if (row.second == id) {
            row.first = newpass;
            updated = true;
            break;
        }
    }
    if (!updated) {
        sales_log::warning(kLog, "agent id not found in password file; no update written");
        return;
    }

    std::ofstream out("passwords.in", std::ios::trunc);
    if (!out.is_open()) {
        sales_log::error(kLog, "cannot open passwords.in for writing");
        return;
    }
    for (const auto &row : rows) {
        out << row.first << ' ' << row.second << '\n';
    }
    if (!out.good()) {
        sales_log::error(kLog, "failed writing passwords.in");
        return;
    }
    sales_log::info(kLog, "password file updated");
}

char menu() {
    char option = 0;
    cout << "Menu" << endl;
    cout << "(a) Add new property" << endl;
    cout << "(b) Remove property " << endl;
    cout << "(c) Edit sales entries" << endl;
    cout << "(d) Inquire or search property sales" << endl;
    cout << "(e) Monthly property sales transaction report" << endl;
    cout << "(f) List property sale items in ascending or descending order" << endl;
    cout << "(g) Administration - add / remove property agent" << endl;
    cout << "(h) Administration - change password" << endl;
    cout << "(i) Quit" << endl;
    if (!(cin >> option)) {
        sales_log::error(kLog, "input stream failure reading menu option");
        cin.clear();
        cin.ignore(10000, '\n');
        return ' ';
    }
    return option;
}

void load(std::list<Data> &l) {
    std::ifstream infile("data.in");
    if (!infile.is_open()) {
        sales_log::error(kLog, "cannot open data.in — starting with empty list");
        return;
    }
    std::string STRING;
    int loaded = 0;
    while (std::getline(infile, STRING)) {
        Data datos;
        if (!parse_data_line(STRING, datos)) {
            continue;
        }
        l.push_back(datos);
        ++loaded;
    }
    if (infile.bad()) {
        sales_log::error(kLog, "read error while loading data.in");
    }
    infile.close();
    sales_log::info(kLog, "loaded " + std::to_string(loaded) + " property row(s) from data.in");
}

void show() {
    char option = 0;
    do {
        cout << "please insert (a) to see the list in ascending order" << endl;
        cout << "please insert (b) to see the is in 	descending order" << endl;
        cout << "else for returning to main menu" << endl;
        if (!(cin >> option)) {
            sales_log::error(kLog, "input failure in show() menu");
            cin.clear();
            cin.ignore(10000, '\n');
            option = ' ';
            break;
        }
        switch (option) {
        case 'a':
            sales_log::info(kLog, "show: ascending selected (stub)");
            break;
        case 'b':
            sales_log::info(kLog, "show: descending selected (stub)");
            break;
        default:
            option = ' ';
        }
    } while (option != ' ' && option != 'a' && option != 'b');
}

int main() {
    std::signal(SIGINT, on_sigint);
    sales_log::info(kLog, "RESMS starting");

    std::list<Data> l;
    string agentid, password;
    cout << "please insert your AgentID" << endl;
    if (!(cin >> agentid)) {
        sales_log::error(kLog, "input failure reading AgentID");
        sales_log::info(kLog, "exiting");
        return 1;
    }
    cout << " please insert your password" << endl;
    if (!(cin >> password)) {
        sales_log::error(kLog, "input failure reading password");
        sales_log::info(kLog, "exiting");
        return 1;
    }

    if (!login(agentid, password)) {
        sales_log::info(kLog, "exiting after failed login");
        return 1;
    }

    load(l);

    char option = 0;
    do {
        option = menu();
        sales_log::info(kLog, std::string("menu option: ") + option);

        switch (option) {
        case 'a':
            addProperty(l);
            break;
        case 'b':
            sales_log::info(kLog, "remove property — not implemented");
            break;
        case 'c':
            sales_log::info(kLog, "edit sales — not implemented");
            break;
        case 'd':
            sales_log::info(kLog, "search — not implemented");
            break;
        case 'e':
            sales_log::info(kLog, "monthly report — not implemented");
            break;
        case 'f':
            show();
            break;
        case 'g':
            sales_log::info(kLog, "admin agents — not implemented");
            break;
        case 'i':
            cout << "good bye" << endl;
            break;
        case 'h':
            changepass(agentid);
            break;
        default:
            sales_log::warning(kLog, "wrong selection");
            cout << "wrong selection" << endl;
        }
    } while (option != 'i');

    sales_log::info(kLog, "normal exit");
    return 0;
}
