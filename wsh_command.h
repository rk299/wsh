#ifndef INCLUDED_WSH_COMMAND
#define INCLUDED_WSH_COMMAND

#include <string>
#include <vector>
#include <wsh_environment.h>

namespace wsh {

class Command
{
    public:

    Command();
    ~Command();

    void append(char c);
    void clear();
    void execute(Environment& e);

    const std::string& command() const;
    const std::string& executable() const;
    std::string arguments() const;
    char** cStyleArgs() const; // caller must freeCStyleArgs()
    void freeCStyleArgs(char**) const;

    private:

    void parse( const Environment& e );
    bool findCommand( const Environment& e );
    void expandEnvironment( const Environment& e );
    void expandVar( const Environment::EnvironmentVar& e,
                    std::string& input );
    bool exists(const std::string& name) const;

    std::string                     m_command;
    std::string                     m_executable;
    std::vector<std::string>        m_arguments;
};

}
#endif
