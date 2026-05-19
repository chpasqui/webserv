#ifndef SESSION_HPP
# define SESSION_HPP


#include <map>
#include <iostream>

class Session {
    private :
        std::string _id;
        time_t      _createdAt;

    public:
        std::map<std::string, std::string> data;
        
        Session();
        static std::string generateId();
        static std::string extractSessionId(std::string cookieHeader);
        std::string toSetCookieString() const;

        void setId(const std::string& id);
        std::string getId() const;
        time_t getCreatedAt() const;

        ~Session();
};

#endif