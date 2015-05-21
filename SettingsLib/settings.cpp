//
// Created by mvv-1 on 28.03.2015.
//
#include "settings.hpp"

#ifdef MY_TO_STRING
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
#endif

class settings_exception : std::exception {
protected:
    const settings *ptr;
public:
    const settings * where () throw() {
        return ptr;
    }
    ~settings_exception () throw() {
        delete(ptr);
    }
};

class file_not_found : settings_exception {
protected:
    std::string name;
public:
    file_not_found (const std::string filename, const settings *where) throw() {
        ptr = where;
        name = filename;
    }
    const char *what () throw() {
        return ("File '" + name + "' not found!").c_str();
    }
    ~file_not_found () throw() {}
};

class param_exception : std::exception {
protected:
    const settings::param *ptr;
public:
    const settings::param * where () throw() {
        return ptr;
    }
};

class cast_error : param_exception {
protected:
    const std::string *goal;
public:
    cast_error (const std::string goal, const settings::param *where) throw() {
        ptr = where;
        this->goal = &goal;
    }
    const char *what () throw() {
        return ("'" + (static_cast <std::string> (*ptr)) + "' can not be converted to " + *goal).c_str();
    }
    ~cast_error () throw() {
        delete(ptr);
        delete(goal);
    }
};

std::string downcase (std::string str) {
    std::string res;
    res.resize(str.size());
    for (int i = 0; i < res.size(); i++) {
        res[i] = str[i] - ((str[i] >= 'A') && (str[i] <= 'Z') ? ('A' - 'a') : 0);
    }
    return res;
}

settings::param::param () {
    string = new std::string("");
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
    return *(this->string);
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
    delete(this->string);
    this->string = new std::string(a);
//    *(this->string) = a;
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
    if (*(this->string) != "") {
        const std::string downcase_str = downcase(*(this->string));
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
        *(this->string) = "true";
    }
    else {
        this->integer = 0;
        *(this->string) = "false";
    }
}

void settings::param::set_integer () {
//    delete(this->string);
//    this->string = new std::string(std::to_string(this->integer));
    *(this->string) = std::to_string(this->integer);
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
//    delete(this->string);
//    this->string = new std::string(std::to_string(this->floating));
    *(this->string) = std::to_string(this->floating);
    if (static_cast <double> (static_cast <int> (this->floating)) == this->floating) {
        this->is_integer = true;
        this->integer = static_cast <int> (this->floating);
    }
}

settings::param &settings::param::operator+= (std::string const &a) {
    *(this->string) += a;
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
    return (*(this->string)).empty();
}

settings::settings (std::string const &filename) {// throw (file_not_found) {
    this->filename = filename;
    try {
        this->reload();
    }
    catch (file_not_found) {
        std::ofstream ouf(filename.c_str());
        ouf << "!!";
        ouf.close();
    }
}

std::string const &settings::get (std::string const &name, std::string const &def) const {
    try {
        return params.at(name);
    }
    catch (const std::exception&) {
        return def;
    }
}

void settings::set (std::string const &name, std::string const &value) {
    params[name] = value;
    save();
}

void settings::reload () {// throw (file_not_found) {
    std::ifstream inf(filename.c_str());
    if (inf.is_open()) {
        std::string line;
        getline(inf, line);
        while (line != "!!") {
            int i;
            for (i = 0; (i < line.size()) && (line[i] != ' '); i++);
            params[line.substr(0, i)] = line.substr(i + 1, line.size() - i - 1);
            getline(inf, line);
        }
        inf.close();
    }
    else {
        throw file_not_found(filename, this);
    }
}

void settings::save () {
    std::ofstream ouf(filename.c_str());
    for (std::map<std::string, std::string>::iterator i = params.begin(); i != params.end(); i++) {
        ouf << (*i).first << ' ' << static_cast <std::string> ((*i).second) << '\n';
    }
    ouf << "!!";
    ouf.close();
}

settings::param const settings::operator[] (std::string const &name) const {
    return param(new std::string(params.at(name)));//static_cast<param>(params.at(name));
}

settings::param settings::operator[] (std::string const &name) {
//    param * p = new param(&params[name]);
//    return *p;
    return param(&params[name]);
}

void settings::reset () {
    params.clear();
    save();
}

settings::param::param (std::string *string) {
    is_integer = false;
    is_floating = false;
    is_boolean = false;
    instant_sync = true;
    this->string = string;
    this->set_string();
}

settings::param::~param () {
    delete(string);
}
