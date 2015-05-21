//
// Created by mvv-1 on 28.03.2015.
//

#ifndef _SETTINGS_SETTINGS_H_
#define _SETTINGS_SETTINGS_H_

#include <exception>
#include <fstream>
#include <map>
#include <string>

// Exceptions
class settings_exception;
class file_not_found;
class param_exception;
class cast_error;

class settings {
friend class settings_exception;
public:
    class param {
        friend class settings;
    private:
        param ();
        param (param const &);
        param (std::string *);
        void set_string ();
        void set_boolean ();
        void set_integer ();
        void set_floating ();
        std::string * string;
        bool boolean;
        int integer;
        double floating;
        bool is_boolean;
        bool is_integer;
        bool is_floating;
        bool instant_sync;
        //std::map<std::string, std::string>::iterator rs;
    public:
        ~param ();
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
    settings(std::string const & filename);// throw (file_not_found);
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
    void reload();// throw (file_not_found);
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

#endif //_SETTINGS_SETTINGS_H_
