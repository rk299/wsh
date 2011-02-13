#ifndef INCLUDED_WSH_ENVIRONMENT
#define INCLUDED_WSH_ENVIRONMENT

#include <string>
#include <vector>

namespace wsh {

class Environment
{
    public:

    class EnvironmentVar
    {
        public:
        EnvironmentVar();
        ~EnvironmentVar();
        void setLine(const std::string& line);
        const std::string& name() const;
        const std::string& value() const;

        private:
        std::string         m_name;
        std::string         m_value;
    };

    Environment( char** envp );
    ~Environment();

    char** ptr();
    const EnvironmentVar& find( const std::string& name ) const;
    const EnvironmentVar& operator[](size_t i) const;
    size_t size() const;

    private:

    char**                              m_ptr;
    std::vector<EnvironmentVar>         m_vars;
};

}
#endif
