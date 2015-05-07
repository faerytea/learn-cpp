//
// Created by mvv-1 on 28.03.2015.
//
#include "settings.hpp"

namespace std {
    string to_string(long a) {
        if (a == 0) {
            return "0";
        }
        bool minlong = false;
        string res = "";
        if (a < 0) {
            res = "-";
            a *= -1;
            if (a < 0) {
                minlong = true;
                a++;
                a *= -1;
            }
        }
        while (a > 0) {
            res = static_cast <char> (a % 10 + '0') + res;
            a /= 10;
        }
        if (minlong) {
            res[res.size() - 1]++;
        }
        return res;
    }
}

std::string downcase (std::string str) {
    std::string res;
    res.resize(str.size());
    for (int i = 0; i < res.size(); i++) {
        res[i] = str[i] - ((str[i] >= 'A') && (str[i] <= 'Z') ? ('A' - 'a') : 0);
    }
    return res;
}

settings::param::param () {
    string = "";
    is_integer = false;
    is_floating = false;
    is_boolean = false;
    instant_sync = true;
}

settings::param::param (settings::param const &a) {
    string = a.string;
    boolean = a.boolean;
    integer = a.integer;
    floating = a.floating;
    is_boolean = a.is_boolean;
    is_integer = a.is_integer;
    is_floating = a.is_floating;
    instant_sync = a.instant_sync;
}

settings::param::operator std::string () const {
    return this->string;
}

settings::param::operator int () const {
    if (this->is_integer) {
        return this->integer;
    }
    else {
        throw cast_error("int", this);
    }
}

settings::param::operator bool () const {
    if (this->is_boolean) {
        return this->boolean;
    }
    else {
        throw cast_error("bool", this);
    }
}

settings::param::operator double () const {
    if (this->is_floating) {
        return this->floating;
    }
    else {
        throw cast_error("double", this);
    }
}

settings::param &settings::param::operator= (std::string const &a) {
    this->string = a;
    if (this->instant_sync) {
        this->set_string();
    }
    return *this;
}

settings::param & settings::param::operator= (int a) {
    this->integer = a;
    this->is_integer = true;
    if (this->instant_sync) {
        this->set_integer();
    }
    return *this;
}

settings::param &settings::param::operator= (bool a) {
    this->boolean = a;
    this->is_boolean = true;
    if (this->instant_sync) {
        this->set_boolean();
    }
    return *this;
}

settings::param &settings::param::operator= (double a) {
    this->is_floating = true;
    this->floating = a;
    if (this->instant_sync) {
        this->set_floating();
    }
    return *this;
}

void settings::param::set_string () {
    this->is_boolean = false;
    this->is_integer = false;
    this->is_floating = false;
    if (this->string != "") {
        const std::string downcase_str = downcase(this->string);
        if ((downcase_str == "true") || (downcase_str == "1")) {
            this->boolean = true;
            this->is_boolean = true;
        }
        if ((downcase_str == "false") || (downcase_str == "0")) {
            this->boolean = false;
            this->is_boolean = true;
        }
        bool point = false, number = true;
        if ((downcase_str[0] == '-')||((downcase_str[0] >= '0') && (downcase_str[0] <= '9'))) {
            if (downcase_str[0] != '-') {
                this->integer = downcase_str[0] - '0';
                this->floating = static_cast <double> (downcase_str[0] - '0');
            }
            else {
                this->integer = 0;
                this->floating = 0;
            }
            double position_in_fraction = 0.1;
            for (int i = 1; i < downcase_str.length(); i++) {
                if (((downcase_str[i] < '0') || (downcase_str[i] >= '9')) && (downcase_str[i] != '.') && (downcase_str[i] != ',')) {
                    number = false;
                }
                else {
                    if ((downcase_str[i] == '.') || (downcase_str[i] == ',')) {
                        if (point) {
                            number = false;
                        }
                        else {
                            point = true;
                        }
                    }
                    else {
                        if (point) {
                            this->floating += (static_cast <double> (downcase_str[downcase_str.size() - 1 - i])) * position_in_fraction;
                            position_in_fraction /= 10;
                        }
                        else {
                            this->integer *= 10;
                            this->integer += downcase_str[i] - '0';
                            this->floating *= 10;
                            this->floating += static_cast <double> (downcase_str[i] - '0');
                        }
                    }
                }
            }
            if (downcase_str[0] == '-') {
                this->integer *= -1;
                this->floating *= -1;
            }
        }
        else {
            number = false;
        }
        if (number) {
            this->is_floating = true;
            if (!point) {
                this->is_integer = true;
            }
        }
    }
}

void settings::param::set_boolean () {
    this->is_integer = true;
    this->is_floating = false;
    if (this->boolean) {
        this->integer = 1;
        this->string = "true";
    }
    else {
        this->integer = 0;
        this->string = "false";
    }
}

void settings::param::set_integer () {
    this->string = std::to_string(this->integer);
    this->is_boolean = false;
    if (this->integer == 0) {
        this->is_boolean = true;
        this->boolean = false;
    }
    if (this->integer == 1) {
        this->is_boolean = true;
        this->boolean = true;
    }
    this->is_floating = true;
    this->floating = static_cast <double> (this->integer);
}

void settings::param::set_floating () {
    this->is_boolean = false;
    this->is_integer = false;
    this->string = std::to_string(this->floating);
    if (static_cast <double> (static_cast <int> (this->floating)) == this->floating) {
        this->is_integer = true;
        this->integer = static_cast <int> (this->floating);
    }
}

settings::param &settings::param::operator+= (std::string const &a) {
    this->string += a;
    if (this->instant_sync) {
        set_string();
    }
    return *this;
}

settings::param &settings::param::operator+= (int a) {
    if (this->is_integer) {
        this->integer += a;
        if (this->instant_sync) {
            this->set_integer();
        }
    }
    else {
        throw cast_error("int", this);
    }
    return *this;
}

settings::param &settings::param::operator+= (double a) {
    if (this->is_floating) {
        this->floating += a;
        if (this->instant_sync) {
            this->set_floating();
        }
    }
    else {
        throw cast_error("double", this);
    }
    return *this;
}

settings::param &settings::param::operator-= (int a) {
    if (this->is_integer) {
        this->integer -= a;
        if (this->instant_sync) {
            this->set_integer();
        }
    }
    else {
        throw cast_error("int", this);
    }
    return *this;
}

settings::param &settings::param::operator-= (double a) {
    if (this->is_floating) {
        this->floating -= a;
        if (this->instant_sync) {
            this->set_floating();
        }
    }
    else {
        throw cast_error("double", this);
    }
    return *this;
}

settings::param &settings::param::operator*= (int a) {
    if (this->is_integer) {
        this->integer *= a;
        if (this->instant_sync) {
            this->set_integer();
        }
    }
    else {
        throw cast_error("int", this);
    }
    return *this;
}

settings::param &settings::param::operator*= (double a) {
    if (this->is_floating) {
        this->floating *= a;
        if (this->instant_sync) {
            this->set_floating();
        }
    }
    else {
        throw cast_error("double", this);
    }
    return *this;
}

settings::param &settings::param::operator/= (int a) {
    if (this->is_integer) {
        this->integer /= a;
        if (this->instant_sync) {
            this->set_integer();
        }
    }
    else {
        throw cast_error("int", this);
    }
    return *this;
}

settings::param &settings::param::operator/= (double a) {
    if (this->is_floating) {
        this->floating /= a;
        if (this->instant_sync) {
            this->set_floating();
        }
    }
    else {
        throw cast_error("double", this);
    }
    return *this;
}

settings::param &settings::param::operator%= (int a) {
    if (this->is_integer) {
        this->integer %= a;
        if (this->instant_sync) {
            this->set_integer();
        }
    }
    else {
        throw cast_error("int", this);
    }
    return *this;
}

settings::param &settings::param::operator|= (bool a) {
    if (this->is_boolean) {
        this->boolean |= a;
        if (this->instant_sync) {
            this->set_boolean();
        }
    }
    else {
        throw cast_error("bool", this);
    }
    return *this;
}

settings::param &settings::param::operator&= (bool a) {
    if (this->is_boolean) {
        this->boolean &= a;
        if (this->instant_sync) {
            this->set_boolean();
        }
    }
    else {
        throw cast_error("bool", this);
    }
    return *this;
}

bool settings::param::is_empty () const {
    return string.size() == 0;
}

settings::settings (std::string const &filename) {
    this->filename = filename;
    this->reload();
}

std::string const &settings::get (std::string const &name, std::string const &def) const {
    try {
        return params.at(name);
    }
    catch (std::out_of_range) {
        return def;
    }
}

void settings::set (std::string const &name, std::string const &value) {
    params[name] = value;
    save();
}

void settings::reload () {
    std::ifstream inf (filename);
    if (inf.is_open()) {
        std::string line;
        getline(inf, line);
        do {
            int i;
            for (i = 0; (i < line.size()) && (line[i] != ' '); i++);
            params[line.substr(0, i)] = line.substr(i + 1, line.size() - i - 1);
            getline(inf, line);
        } while (line != "!!");
        inf.close();
    }
    else {
        throw file_not_found(filename, this);
    }
}

void settings::save () {
    std::ofstream ouf(filename);
    for (auto i = params.begin(); i != params.end(); i++) {
        ouf << (*i).first << ' ' << static_cast <std::string> ((*i).second) << '\n';
    }
    ouf << "!!";
    ouf.close();
}
//
//settings::param const settings::operator[] (std::string const &name) const {
//    return static_cast<param>(params.at(name));
//}
//
//settings::param settings::operator[] (std::string const &name) {
//    return static_cast<param>(params[name]);
//}

void settings::reset () {
    params.clear();
    save();
}
