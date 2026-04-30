#ifndef SESSION_HPP
# define SESSION_HPP


#include <map>
#include <iostream>

class Session {
    private :
        std::string _id;

    public:
        std::map<std::string, std::string> data;
        

        Session();
        static std::string generateId();
        static std::string extractSessionId(std::string cookieHeader);
        std::string toSetCookieString() const;

        void setId(const std::string& id);
        std::string getId() const;

        ~Session();
};

#endif