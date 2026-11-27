#include <assert.h>

void MEM_SET( void* buffer, int size, char value )
{
	char* p = (char*)buffer;
	while( size-- > 0 )
	{
		*p++ = value;
	}
}

typedef struct
{
	unsigned char len;
	unsigned char id;
	unsigned char seq_nr;
	unsigned char crc;
} Page;


void main()
{

	Page my_page;
	MEM_SET( &my_page, sizeof( Page ), 0xFF );
	assert( my_page.len == 255 );
}
// Imported from ESBMC regression/cbmc/01_cbmc_Struct_Bytewise2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
