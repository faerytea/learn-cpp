#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <string>

using namespace std;

enum condition {
    STABLE, BORN, DIE
};

const char MAX_MARGIN = 20, MIN_MARGIN = 5;

class Map {
private:
    bool **map;
    long x0, y0;
    unsigned width, height, born, died, alive, stagnation;

    void changeCapacity (long addLeft, long addTop, long addRight, long addDown) {
        if (((long long)width + addLeft + addRight) <= 0) {
            addLeft = 0;
            addRight = width;
        }if (((long long)height + addDown + addTop) <= 0) {
            addTop = 0;
            addDown = height;
        }
        const unsigned newWidth = width + addLeft + addRight;
        const unsigned newHeight = height + addDown + addTop;
        const long newX0 = x0 + addLeft;
        const long newY0 = y0 + addTop;
        bool **newMap = new bool *[newHeight];
        for (long i = 0; i < newHeight; i++) {
            newMap[i] = new bool[newWidth];
            for (long j = 0; j < newWidth; j++) {
                if ((i >= addTop) && (j >= addLeft) && (i < min(static_cast <long long> (newHeight), static_cast <long long> (height + addTop))) && (j < min(static_cast <long long> (newWidth), static_cast <long long> (width + addLeft))))
                    newMap[i][j] = map[i - addTop][j - addLeft];
                else
                    newMap[i][j] = false;
            }
        }
        for (long i = height - 1; i != 0; i--) {
            delete(map[i]);
        }
        delete(map);
        map = newMap;
        height = newHeight;
        width = newWidth;
        x0 = newX0;
        y0 = newY0;
    }

    condition calcNextCondition (unsigned s, unsigned c) {
        char counter = 0;
        if ((s > 0) && (c > 0)) if (map[s - 1][c - 1])
            counter++;
        if ((s > 0)) if (map[s - 1][c])
            counter++;
        if ((s > 0) && (c + 1 < width)) if (map[s - 1][c + 1])
            counter++;
        if ((c + 1 < width)) if (map[s][c + 1])
            counter++;
        if ((s + 1 < height) && (c + 1 < width)) if (map[s + 1][c + 1])
            counter++;
        if ((s + 1 < height)) if (map[s + 1][c])
            counter++;
        if ((s + 1 < height) && (c > 0)) if (map[s + 1][c - 1])
            counter++;
        if ((c > 0)) if (map[s][c - 1])
            counter++;
        switch (counter) {
            case 2:
                return STABLE;
            case 3:
                return BORN;
            default:
                return DIE;
        }
    }

    void verifyAlive () {
        alive = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (map[i][j])
                    alive++;
            }
        }
    }

public:
    char symAlive, symDead;

    Map (long width = 10, long height = 10) {
        map = new bool *[height];
        for (int i = 0; i < height; i++) {
            map[i] = new bool [width];
        }
        this->height = height;
        this->width = width;
        x0 = 0;
        y0 = 0;
        symAlive = 'O';
        symDead = '+';
        born = 0;
        died = 0;
        stagnation = 0;
        verifyAlive();
    }

    Map (ifstream &file, char live = 'O', char dead = '+') {
        symAlive = live;
        symDead = dead;
        alive = 0;
        x0 = 0;
        y0 = 0;
        born = 0;
        died = 0;
        stagnation = 0;
        string line;
        getline(file, line);
        width = line.size();
        height = MAX_MARGIN;
        map = new bool *[height];
        int curLine = 0;
        do {
            bool *tmp = new bool[width];
            int i;
            for (i = 0; (i < width)&&(i < line.size()); i++) {
                tmp[i] = (line[i] == symAlive);
            }
            for (i; i < width; i++) {
                tmp[i] = false;
            }
            if (curLine == height) {
                height += MAX_MARGIN;
                bool **tmpMap = new bool *[height];
                for (int i = 0; i < height - MAX_MARGIN; i++) {
                    tmpMap[i] = map[i];
                }
                delete(map);
                map = tmpMap;
            }
            map[curLine] = tmp;
            getline(file, line);
            curLine++;
        } while ((line.size() > 0)&&(!file.eof()));
        for (curLine; curLine < height; curLine++) {
            map[curLine] = new bool[width];
            for (int i = 0; i < width; i++) {
                map[curLine][i] = false;
            }
        }
        verifyAlive();
    }

    ~Map () {
        for (long i = height - 1; i != 0; i--) {
            delete[] (map[i]);
        }
        delete(map);
    }

    void nextGen () {
        bool *tmpt = new bool[width], *tmpb = new bool[width];
        bool changeFlagTop = true, changeFlagLeft = true, stagnationFlag = true;
        long cntrUselessRight = 0, cntrUselessLeft = 0, UselessTop = 0, UselessLeft = 0, UselessRight = 0, UselessBottom = 0;
        born = 0;
        died = 0;
        for (unsigned j = 0; j < width; j++) {
            switch (calcNextCondition (0, j)) {
                case STABLE:
                    tmpt[j] = map[0][j];
                    break;
                case BORN:
                    tmpt[j] = true;
                    break;
                case DIE:
                    tmpt[j] = false;
                    break;
            }
            if ((tmpt[j]) || (map[0][j])) {
                changeFlagLeft = false;
                changeFlagTop = false;
                cntrUselessRight = 0;
                UselessBottom = 0;
            } else {
                if (changeFlagLeft)
                    cntrUselessLeft++;
                cntrUselessRight++;
            }
            if (tmpt[j] != map[0][j]) {
                stagnationFlag = false;
                if (tmpt[j]) {
                    born++;
                }
                else {
                    died++;
                }
            }
        }
        UselessRight = cntrUselessRight;
        UselessLeft = cntrUselessLeft;
        if (changeFlagTop) {
            UselessTop++;
            UselessBottom++;
        }
        for (unsigned i = 1; i < height; i++) {
            cntrUselessLeft = 0;
            cntrUselessRight = 0;
            changeFlagLeft = true;
            UselessBottom++;
            for (unsigned j = 0; j < width; j++) {
                switch (calcNextCondition (i, j)) {
                    case STABLE:
                        tmpb[j] = map[i][j];
                        break;
                    case BORN:
                        tmpb[j] = true;
                        break;
                    case DIE:
                        tmpb[j] = false;
                        break;
                }
                if ((tmpb[j]) || (map[i][j])) {
                    changeFlagLeft = false;
                    changeFlagTop = false;
                    cntrUselessRight = 0;
                    UselessBottom = 0;
                } else {
                    if (changeFlagLeft) {
                        cntrUselessLeft++;
                    }
                    cntrUselessRight++;
                }
                if (tmpb[j] != map[i][j]) {
                    stagnationFlag = false;
                    if (tmpb[j]) {
                        born++;
                    }
                    else {
                        died++;
                    }
                }
            }
            UselessLeft = min (UselessLeft, cntrUselessLeft);
            UselessRight = min (UselessRight, cntrUselessRight);
            if (changeFlagTop) {
                UselessTop++;
            }
            delete(map[i - 1]);
            map[i - 1] = tmpt;
            tmpt = tmpb;
            tmpb = new bool[width];
        }
        delete(tmpb);
        map[height-1] = tmpt;
        if ((UselessRight == 0) || (UselessTop == 0) || (UselessLeft == 0) || (UselessBottom == 0) || (UselessRight > MAX_MARGIN) || (UselessLeft > MAX_MARGIN) || (UselessTop > MAX_MARGIN) || (UselessBottom > MAX_MARGIN)) {
            changeCapacity (MIN_MARGIN - UselessLeft,  MIN_MARGIN - UselessTop, MIN_MARGIN - UselessRight, MIN_MARGIN - UselessBottom);
        }
        alive += born;
        alive -= died;
        if (stagnationFlag)
            stagnation++;
    }

    void print (ostream &stream) {
        for (long i = 0; i < height; i++) {
            for (long j = 0; j < width; j++) {
                if (map[i][j]) {
                    stream << symAlive;
                } else {
                    stream << symDead;
                }
            }
            stream << '\n';
        }
        stream.flush();
    }

    void print (long width, long height) {
        for (long i = y0; i < height + y0; i++) {
            for (long j = x0; j < width + x0; j++) {
                if ((i >= 0) && (i < this->height) && (j >= 0) && (j < this->width)) {
                    if (map[i][j]) {
                        cout << symAlive;
                    } else {
                        cout << symDead;
                    }
                } else {
                    cout << symDead;
                }
            }
            cout << '\n';
        }
        cout.flush ();
    }

    void fillR (unsigned seed, unsigned long long frequency, unsigned marginTop, unsigned marginLeft, unsigned marginRight, unsigned marginBottom) {
        mt19937 gen(seed);
        for (unsigned i = marginTop; i < height-marginBottom; i++) {
            for (unsigned j = marginLeft; j < width-marginRight; j++) {
                map[i][j] = (gen() < frequency);
            }
        }
        verifyAlive();
    }

    void fill (unsigned seed, unsigned long long number, unsigned marginTop, unsigned marginLeft, unsigned marginRight, unsigned marginBottom) {
        long long tmpWdth = width - marginLeft - marginRight,
                  tmpHght = height - marginBottom - marginTop;
        if ((tmpWdth < 0)||(tmpHght < 0)) {
            return;
        }
        if (tmpHght * tmpWdth <= number) {
            for (int i = marginTop; i < marginTop + tmpHght; i++) {
                for (int j = marginLeft; j < marginLeft + tmpWdth; j++) {
                    map[i][j] = true;
                }
            }
            return;
        }
        mt19937 gen(seed);
        uniform_int_distribution <> myDist(0, tmpWdth * tmpHght - 1);
        long x, y;
        for (int i = 0; i < number; i++) {
            do {
                x = y = myDist(gen);
                x = x / tmpWdth + marginTop,
                y = y % tmpWdth + marginLeft;
            } while (map[x][y]);
            map[x][y] = true;
        }
        verifyAlive();
    }

    void moveXY (long x, long y) {
        x0 += x;
        y0 += y;
    }

    unsigned getBorn () {
        return born;
    }

    unsigned getDied () {
        return died;
    }

    unsigned getAlive () {
        return alive;
    }

    unsigned getStagnation () {
        return stagnation;
    }
};

pair <long, long> get2args (const string &line, long defaultFirst, long defaultSecond) {
    int i = 1;
    long f = defaultFirst, s = defaultSecond;
    for (i; (i < line.size())&&((line[i] > '9')||(line[i] < '0')); i++);
    int j = i;
    for (j; (j < line.size())&&(line[j] <= '9')&&(line[j] >= '0'); j++);
    if (i != j) {
        f = atol(line.substr(i,j-i).c_str());
    }
    for (i = j; (i < line.size())&&(line[i] == ' '); i++);
    for (j = i; (j < line.size())&&(line[j] <= '9')&&(line[j] >= '0'); j++);
    if (i != j) {
        s = atol(line.substr(i,j-i).c_str());
    }
    return make_pair(f, s);
}

string getStrArg (const string &line) {
    int i = 0;
    for (i; (i < line.length())&&(line[i] != ' '); i++);
    i++;
    int j = i;
    for (j; (j < line.length())&&(line[j] != ' '); j++);
    if (j > line.size()) {
        return "";
    }
    else {
        return line.substr(i, j-i);
    }
}

int main () {
    cout.sync_with_stdio(false);
    cout << "This is Conway's game Life by mvv-1\nFor help type 'help' (or just 'h')" << endl;
    string command, lastcommand = "h";
    Map *life = nullptr;
    bool work = true;
    struct settings {
        unsigned delay = 500;
        unsigned numberOfSteps = 10;
        unsigned width = 20;
        unsigned height = 20;
        bool printBorn = true;
        bool printDied = true;
        bool printAlive = false;
        enum printMode {no, all, start} printPlane = no;
        const char *modes[3] = {"don't print", "print all actual plane", "print start position"};
    } setts;
    const string err = "Plane doesn't exist.";
    while (work) {
        cout<<"life > ";
        getline (cin, command);
        if (command == "") {
            command = lastcommand;
        }
        switch (command[0]) {
            case 'h':
                cout << "\thelp         - print this help\n" <<
                        "\tnew [W] [H]  - create new map\n" <<
                        "\t               W - width, H - height (W = H = 20, if undefined)\n" <<
                        "\tfill N M     - place N alive cells; M cols and M rows on edges will be empty\n" <<
                        "\tfillR C M    - fill map by random; C (by 0 to " << UINTMAX_MAX << ") - is chance of alive cell\n" <<
                        //"\tload FILE    - load from file (file must be created by this program)\n" <<
                        "\tget FILE     - get from file\n" <<
                        "\tprint [all]  - print map; if 'all' then print all actual map, else print start position\n" <<
                        //"\tsave FILE    - save for load later\n" <<
                        "\tput FILE     - save into FILE for reading (output like after 'print all')\n" <<
                        "\tnext [N]     - do N (0 if undefined) steps\n" <<
                        "\tcontinue [N] - do standart number of steps, print start position and stats, repeat N times\n" <<
                        "\tsettings [s] - print settings; if 's' then set settings\n" <<
                        "\texplore      - explore the plane with hjkl" <<
                        "\tquit         - quit\n" <<
                        "Note: h = help, l = load, g = get, p = print, s = save, n = next, c = continue, q = quit. \n" <<
                        "If you will not write any command, your last command will be executed." << endl;
                break;
            case 'n':
                if (command.substr (0,3) == "new") {
                    pair <int, int> tmp = get2args(command, 20, 20);
                    delete(life);
                    life = new Map(tmp.first, tmp.second);
                }
                else {
                    if (life == nullptr) {
                        cerr << err << endl;
                    }
                    else {
                        int arg = get2args(command, 1, 0).first;
                        for (int i = 0; i < arg; i++) {
                            life->nextGen();
                        }
                    }
                }
                break;
            case 'f':
                if (life == nullptr) {
                    cerr << err << endl;
                    break;
                }
                if (command.size() > 4) {
                    pair <int, int> tmp = get2args(command, UINTMAX_MAX/3, 5);
                    random_device rd;
                    if ((command[4] == 'R')||(command[4] == 'r')) {
                        life->fillR(rd(), tmp.first, tmp.second, tmp.second, tmp.second, tmp.second);
                    }
                    else {
                        life->fill(rd(), tmp.first, tmp.second, tmp.second, tmp.second, tmp.second);
                    }
                }
                break;
            case 'g':
                ifstream *inf;
                inf = new ifstream(getStrArg(command));
                if (inf->is_open()) {
                    char a = 'O', d = '+';
                    if (life != nullptr) {
                        a = life->symAlive;
                        d = life->symDead;
                        delete(life);
                    }
                    life = new Map(*inf, a, d);
                    inf->close();
                }
                else {
                    cerr << "Can't open file" << endl;
                }
                delete(inf);
                break;
            case '!':
                life->fillR(0, 0, 0, 0, 0, 0);
                life->fillR(1, 2000000000, 2, 2, 2, 2);
                break;
            case 'p':
                if (life == nullptr) {
                    cerr << err << endl;
                    break;
                }
                if ((command.size() >= 3)&&(command.substr(0, 3) == "put")) {
                    ofstream *ouf;
                    ouf = new ofstream(getStrArg(command));
                    if (ouf->is_open()) {
                        life->print(*ouf);
                        ouf->close();
                    }
                    else {
                        cerr << "Can't open file" << endl;
                    }
                    delete(ouf);
                }
                else {
                    if (getStrArg(command) == "all") {
                        life->print(cout);
                    }
                    else {
                        life->print(setts.width, setts.height);
                    }
                }
                break;
            case 's':
                if (getStrArg(command) == "") {
                    cout << "Width of output  " << setts.width << '\n' <<
                            "Height of output " << setts.height << '\n' <<
                            "Silent steps     " << setts.numberOfSteps << '\n' <<
                            "Print plane      " << setts.modes[setts.printPlane] << '\n' <<
                            "Stats:\n" <<
                            "\tBorn  " << (setts.printBorn  ? "true" : "false") << '\n' <<
                            "\tDied  " << (setts.printDied  ? "true" : "false") << '\n' <<
                            "\tAlive " << (setts.printAlive ? "true" : "false") << endl;
                    if (life != nullptr) {
                        cout << "Legend:\n" <<
                                "\tAlive " << life->symAlive << '\n' <<
                                "\tDead  " << life->symDead << endl;
                    }
                }
                else {
                    string tmp;
                    cout << "Width of output  ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        setts.width = atol(tmp.c_str());
                    }
                    cout << "Height of output ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        setts.height = atol(tmp.c_str());
                    }
                    cout << "Silent steps     ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        setts.numberOfSteps = atol(tmp.c_str());
                    }
                    cout << "Print plane:\n";
                    for (int i = 0; i < 3; i++)
                        cout << '\t' << setts.modes[i] << '\n';
                    cout << "(no/all/start):  ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        switch (tmp[0]) {
                            case 'n':
                                setts.printPlane = settings::no;
                                break;
                            case 'a':
                                setts.printPlane = settings::all;
                                break;
                            case 's':
                                setts.printPlane = settings::start;
                                break;
                        }
                    }
                    cout << "Stats:\n" <<
                            "\tBorn  ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        setts.printBorn = (tmp[0] == 't') || (tmp[0] == '1');
                    }
                    cout << "\tDied  ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        setts.printDied = (tmp[0] == 't') || (tmp[0] == '1');
                    }
                    cout << "\tAlive ";
                    getline(cin, tmp);
                    if (tmp != "") {
                        setts.printAlive = (tmp[0] == 't') || (tmp[0] == '1');
                    }
                    if (life != nullptr) {
                        cout << "Legend:\n" <<
                                "\tAlive ";
                        getline(cin, tmp);
                        if (tmp != "") {
                            life->symAlive = tmp[0];
                        }
                        cout << "\tDead  ";
                        getline(cin, tmp);
                        if (tmp != "") {
                            life->symDead = tmp[0];
                        }
                    }
                }
                break;
            case 'c':
                if (life == nullptr) {
                    cerr << err << endl;
                    break;
                }
                unsigned steps, sumBorn, sumDied;
                steps = get2args(command, 1, 1).first;
                while (steps != 0) {
                    sumBorn = 0;
                    sumDied = 0;
                    for (unsigned i = 0; i < setts.numberOfSteps; i++) {
                        life->nextGen();
                        sumBorn += life->getBorn();
                        sumDied += life->getDied();
                    }
                    switch (setts.printPlane) {
                        case settings::all:
                            life->print(cout);
                            break;
                        case settings::start:
                            life->print(setts.width, setts.height);
                            break;
                    }
                    if (setts.printAlive) {
                        cout << "Now lives " << life->getAlive() << " cell(s)" << endl;
                    }
                    if (setts.printBorn) {
                        cout << "Born " << sumBorn << endl;
                    }
                    if (setts.printDied) {
                        cout << "Died " << sumDied << endl;
                    }
                    if (setts.printBorn && setts.printDied) {
                        cout << "δ    " << (long long)sumBorn - (long long)sumDied << endl;
                    }
                    if (life->getStagnation() > 0) {
                        cout << "Plane haven't been changed for " << life->getStagnation() << " generation(s)" << endl;
                    }
                    steps--;
                }
                break;
            case 'e':
                if (life == nullptr) {
                    cerr << err << endl;
                    break;
                }
                cout << "Use hjkl for navigation; q for quit" << endl;
                command = "-";
                int action;
                action = '\n';
                do {
                    life->print(setts.width, setts.height);
                    action = cin.get();
                    switch (action) {
                        case 'h':
                            life->moveXY(-1, 0);
                            break;
                        case 'j':
                            life->moveXY(0, 1);
                            break;
                        case 'k':
                            life->moveXY(0, -1);
                            break;
                        case 'l':
                            life->moveXY(1, 0);
                            break;
                    }
                } while (action != 'q');
                cout << endl;
                break;
            case 'q':
                work = false;
                break;
            case '-':
                cout << endl; // do nothing, just because 
                break;
            default:
                cout<<"Wrong!"<<endl;
        }
        lastcommand = command;
    }
    return 0;
}
