#include <wsh_command.h>
#include <wsh_environment.h>
#include <iostream>
#include <sstream>
#include <vector>

extern "C"
{
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
}

namespace wsh
{

Command::Command()
    : m_command()
    , m_executable()
    , m_arguments()
{
}

Command::~Command()
{
}

void Command::append(char c)
{
    std::ostringstream os;

    os << c;

    m_command.append( os.str() );
}

void Command::clear()
{
    m_command.clear();
    m_executable.clear();
    m_arguments.clear();
}

const std::string& Command::command() const
{
    return m_command;
}

const std::string& Command::executable() const
{
    return m_executable;
}

char** Command::cStyleArgs() const
{
    char** c = (char**)malloc( m_arguments.size() + 2 );

    if ( c )
    {
        c[0] = strdup( m_executable.c_str() );

        for ( size_t i = 0; i < m_arguments.size(); i++ )
        {
            c[i+1] = strdup( m_arguments[i].c_str() );
        }
    }

    c[ m_arguments.size() + 1 ] = 0;

    return c;
}

void Command::freeCStyleArgs(char** c) const
{
    if ( c )
    {
        unsigned int count = 0;
        for ( char** b = c; b != 0; b++, count++ )
        {
            if ( *b )
            {
                free( *b );
            }
            else
            {
                break;
            }
        }

        free( c );
    }
}

std::string Command::arguments() const
{
    std::string fullArgs("");

    for ( size_t i = 0; i < m_arguments.size(); i++ )
    {
        fullArgs += m_arguments[i] + " ";
    }

    return fullArgs;
}


bool Command::exists( const std::string& name ) const
{
    FILE *fp = fopen(name.c_str(),"r");

    if ( fp )
    {
        fclose(fp);
        return true;
    }

    return false;
}

void Command::expandVar( const Environment::EnvironmentVar& envVar,
                         std::string& input )
{
    size_t pos = std::string::npos;

    if ( (pos = input.find( envVar.name() )) != std::string::npos )
    {
        input.replace( pos, envVar.value().length(), envVar.value() );
    }
}

void Command::expandEnvironment( const Environment& e )
{
    for ( size_t i = 0; i < e.size(); i++ )
    {
        expandVar( e[i], m_executable );

        for ( size_t j = 0; j < m_arguments.size(); j++ )
        {
            expandVar( e[i], m_arguments[j] );
        }
    }
}

void Command::parse( const Environment& e )
{
    // split into executable and arguments
    char* com = strdup( m_command.c_str() );

    char* pch = strtok( com, " " );

    size_t count = 0;

    while ( pch != 0 )
    {
        if ( count == 0 )
        {
            m_executable = std::string( pch );
        }
        else
        {
            m_arguments.push_back( std::string( pch ) );
        }

        pch = strtok( 0, " " );
        count++;
    }

    free( com );
}

bool Command::findCommand( const Environment& e )
{
    std::string path = e.find("PATH").value();

    std::vector<std::string> paths;

    char* cpath = strdup( path.c_str() );

    char* pch = strtok( cpath, ":" );
    while ( pch != 0 )
    {
        paths.push_back( std::string(pch) + "/" );
        pch = strtok( 0, ":" );
    }

    free( cpath );

    // current directory
    paths.push_back("./");
    // root directory
    paths.push_back("/");

    size_t i = 0;

    for ( ; i < paths.size(); i++ )
    {
        if ( exists( paths[i] + m_executable ) )
        {
            break;
        }
    }

    if ( i < paths.size() )
    {
        m_executable = paths[i] + m_executable;
        return true;
    }

    return false;
}

void Command::execute(Environment& e)
{
    // split command into executable and arguments
    parse( e );

    // expand environment variables in executable and arguments
    expandEnvironment( e );

    // if expanded executable exists in PATH
    if ( findCommand( e ) )
    {
        int i = 0;

        char *c = strdup( m_executable.c_str() );
        char **a = cStyleArgs();
        char **e_p = e.ptr();

        if ( fork() == 0 )
        {
            i = execve( c,
                        a,
                        e_p );
            exit(1);
        }
        else
        {
            wait(0);
        }

        free( c );
        freeCStyleArgs(a);
        //@TODO: if env is modified, we need to handle that as well
    }
    else
    {
        std::cout << std::endl
                  << m_executable
                  << ": command not found"
                  << std::endl;
    }
}

}
