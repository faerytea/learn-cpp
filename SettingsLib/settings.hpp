//
// Created by mvv-1 on 28.03.2015.
//

#ifndef _SETTINGS_SETTINGS_H_
#define _SETTINGS_SETTINGS_H_

#include <exception>
#include <fstream>
#include <map>
#include <string>

class settings {
friend class settings_exception;
public:
    class param {
        friend class settings;
    private:
        param ();
        param (param const &);
        void set_string ();
        void set_boolean ();
        void set_integer ();
        void set_floating ();
        std::string string;
        bool boolean;
        int integer;
        double floating;
        bool is_boolean;
        bool is_integer;
        bool is_floating;
        bool instant_sync;
    public:
        operator std::string() const;
        operator int() const;
        operator bool() const;
        operator double() const;

        param & operator=(std::string const &);
        param & operator=(int);
        param & operator=(bool);
        param & operator=(double);

        param & operator+=(std::string const &);
        param & operator+=(int);
        param & operator+=(double);

        param & operator-=(int);
        param & operator-=(double);

        param & operator*=(int);
        param & operator*=(double);

        param & operator/=(int);
        param & operator/=(double);

        param & operator%=(int);

        param & operator|=(bool);
        param & operator&=(bool);
        bool is_empty() const;
    };
    /**
     * Construct settings store
     * and load data from file (if exists)
     * \param filename Path to file with settings
     */
    settings(std::string const & filename);
    /**
     * Get setting value
     * \param name Setting unique identifier
     * \param def Default setting value
     * \return Stored value for given name or default value
     */
    std::string const & get(std::string const & name, std::string const & def = "") const;
    /**
     * Set or replace setting value and save changes to file
     * \param name Setting unique identifier
     * \param value New setting value
     */
    void set(std::string const & name, std::string const & value);
    /**
     * Reset all settings
     */
    void reset();
    /**
     * Reload all settings from file
     */
    void reload();
    /**
      * Get constant setting wrapper
      * \param name Setting unique identifier
      */
    const param operator[](std::string const & name) const;
    /**
      * Get constant setting wrapper
      * \param name Setting unique identifier
      */
    param operator[](std::string const & name);
private:
    void save();
    std::string filename;
    std::map <std::string, std::string> params;
    //std::pair <settings::param, std::string> temp;
};

// Exceptions
class settings_exception : std::exception {
protected:
    const settings *ptr;
public:
    const settings * where () {
        return ptr;
    }
};

class file_not_found : settings_exception {
protected:
    std::string name;
public:
    file_not_found (const std::string filename, const settings *where) {
        ptr = where;
        name = filename;
    }
    const char *what () {
        return ("File '" + name + "' not found!").c_str();
    }
};

class param_exception : std::exception {
protected:
    const settings::param *ptr;
public:
    const settings::param * where () {
        return ptr;
    }
};

class cast_error : param_exception {
protected:
    const std::string *goal;
public:
    cast_error (const std::string goal, const settings::param *where) {
        ptr = where;
        this->goal = &goal;
    }
    const char *what () {
        return ("'" + (static_cast <std::string> (*ptr)) + "' can not be converted to " + *goal).c_str();
    }
};


#endif //_SETTINGS_SETTINGS_H_
