#include "GELF.hpp"

#define UNUSED(expr) do { (void)(expr); } while (0)


int main( int argc, char** argv )
{
    UNUSED(argc);
    UNUSED(argv);

    gelf::initialize();
    gelf::configure( "127.0.0.1", 5000 );

    gelf::MessageBuilder message( gelf::Severity::Informational, "Hello world!" );
    gelf::post( message.build() );

    gelf::destroy();

    return 0;
}