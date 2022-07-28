<<<<<<< HEAD
char* compress_buffer(char*);
char* uncompress_buffer(char*, ulong, ulong);
=======
// Compresses buffer using compress() from zlib, which requires buffer_size and buffer_byte_size.
//
// Returns compressed buffer.
char* compress_buffer(char* buffer, ulong buffer_size, ulong buffer_byte_size);

// Decompresses buffer using uncompress() from zlib, which requires uncompressed original_size and compressed_buffer_size.
//
// Returns uncompressed buffer.
char* uncompress_buffer(char* buffer, ulong original_size, ulong compressed_buffer_size);
>>>>>>> dev
