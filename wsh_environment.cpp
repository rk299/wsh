#include <string>
#include <vector>
#include <wsh_environment.h>

namespace wsh {

Environment::EnvironmentVar::EnvironmentVar()
    : m_name()
    , m_value()
{
}

Environment::EnvironmentVar::~EnvironmentVar()
{
}

const std::string& Environment::EnvironmentVar::name() const
{
    return m_name;
}

const std::string& Environment::EnvironmentVar::value() const
{
    return m_value;
}

void Environment::EnvironmentVar::setLine(
        const std::string& line )
{
    size_t pos = line.find("=");

    if ( pos != std::string::npos )
    {
        m_name = line.substr(0,pos);
        m_value = line.substr(pos+1);
    }
}

Environment::Environment( char** envp )
    : m_ptr( envp )
    , m_vars()
{
    for ( char** c = envp; c != 0; c++ )
    {
        if ( *c )
        {
            Environment::EnvironmentVar v;

            v.setLine( std::string( *c ) );

            m_vars.push_back( v );
        }
        else
        {
            break;
        }
    }
}

Environment::~Environment()
{
}

char** Environment::ptr()
{
    return m_ptr;
}

size_t Environment::size() const
{
    return m_vars.size();
}

const Environment::EnvironmentVar&
        Environment::find( const std::string& name ) const
{
    for ( size_t i = 0; i < m_vars.size(); i++ )
    {
        if ( m_vars[i].name() == name )
        {
            return m_vars[i];
        }
    }
}

const Environment::EnvironmentVar&
        Environment::operator[](size_t i) const
{
    return m_vars[i];
}

}
