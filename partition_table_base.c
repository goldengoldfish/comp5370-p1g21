#include "mbr.h"
#include "partition_table.h"
#include "stream.h"
#include <assert.h>
#include <stdlib.h>

// *Ideally* this will be written in an extensible way to eventually support
// GPT, but I'm doing just MBR for now.

PTable PTable_getTable(Stream *stream) {
	MBRInfo *mbr = malloc(sizeof(MBRInfo));
	read_mbr(stream, mbr);
}
