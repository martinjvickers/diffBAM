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
   int num_bam;
};

struct Stats
{
   uint64_t mapped = 0, unmapped = 0;
   vector<vector<uint64_t> > name_incommon;
   vector<vector<uint64_t> > seq_incommon;
   vector<vector<uint64_t> > pos_incommon;
};

void init(Stats &stats, ModifyStringOptions &opt)
{
   vector<vector<uint64_t> > vec(opt.num_bam);
   for(int i = 0; i < opt.num_bam; i++)
      for(int j = 0; j < opt.num_bam; j++)
         vec[i].push_back(0);
   stats.name_incommon = vec;
   stats.seq_incommon = vec;
   stats.pos_incommon = vec;
}

void print(Stats &stats, ModifyStringOptions &opt)
{
   cout << "Names in common :" << endl;
   for(int i = 0; i < opt.num_bam; i++){
      for(int j = 0; j < opt.num_bam; j++){
         cout << stats.name_incommon[i][j] << "\t";
      }
      cout << endl;
   }
   cout << endl;

   cout << "Seqs in common :" << endl;
   for(int i = 0; i < opt.num_bam; i++){
      for(int j = 0; j < opt.num_bam; j++){
         cout << stats.seq_incommon[i][j] << "\t";
      }
      cout << endl;
   }
   cout << endl;

   cout << "Pos in common :" << endl;
   for(int i = 0; i < opt.num_bam; i++){
      for(int j = 0; j < opt.num_bam; j++){
         cout << stats.pos_incommon[i][j] << "\t";
      }
      cout << endl;
   }
}

#endif
