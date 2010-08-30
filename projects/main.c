#include <ruby.h>
#include <unistd.h>

int main( int argc, char *argv[] )	{
		
    ruby_init();
	
	//	Change working directory to script
	chdir( "../.." );
	
	//	Ruby Options are just like /usr/bin/ruby
	//	interpreter name, script name, argv ...
	char*	options[]	=	{ "", "sender_test.rb",  };
	void*	node		=	ruby_options( 2, options );
	
    return ruby_run_node( node );
}
