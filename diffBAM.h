#ifndef DIFFBAM_H
#define DIFFBAM_H

#include <stdio.h>
#include <zlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include <seqan/arg_parse.h>
#include <seqan/seq_io.h>
#include <seqan/bam_io.h>
#include <seqan/sequence.h>
#include <seqan/modifier.h>

using namespace seqan;
using namespace std;

struct ModifyStringOptions
{
   vector<CharString> inputFileName;
};

#endif
