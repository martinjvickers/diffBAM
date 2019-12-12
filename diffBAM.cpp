#include "diffBAM.h"

ArgumentParser::ParseResult parseCommandLine(ModifyStringOptions & options,
                                             int argc, char const ** argv)
{
   ArgumentParser parser("diffBAM");
   setVersion(parser, "0.0.1");
   setDate(parser, "December 2019");
   addUsageLine(parser, "-i mapped1.bam -i mapped2.bam");
   setShortDescription(parser, "diffBAM: compare two or more BAM files.");

   addOption(parser, ArgParseOption("i", "input-file",
                                    "Path to the input file",
                                    ArgParseArgument::INPUT_FILE, "IN", true));

   ArgumentParser::ParseResult res = parse(parser, argc, argv);

   // If parsing was not successful then exit with code 1 if there were errors.
   // Otherwise, exit with code 0 (e.g. help was printed).
   if (res != seqan::ArgumentParser::PARSE_OK)
      return res;

   int num_bams = getOptionValueCount(parser, "input-file");

   for(int i = 0; i < num_bams; i++)
   {
      CharString tmp;
      getOptionValue(tmp, parser, "input-file", i);
      options.inputFileName.push_back(tmp);
   }

   return ArgumentParser::PARSE_OK;
}

string basicName(CharString name)
{
   std::vector<std::string> tokens;
   std::istringstream iss(toCString(name));
   std::string token;
   while(std::getline(iss, token, '_'))
      tokens.push_back(token);
   
   return tokens[0];
}

// return true if all bam files have been read
bool emptyBAMs(ModifyStringOptions &opt, BamFileIn *bamFiles)
{
   for(int i = 0; i < opt.num_bam; i++)
   {
      if(!atEnd(bamFiles[i]))
         return false;
   }
   return true;
}

// Given a set of reads (one read/readpair from each bam)
int compareRecords(ModifyStringOptions &opt, Stats &stats, 
                   map<CharString, pair<BamAlignmentRecord, BamAlignmentRecord>> *buffer)
{
   int comp = 0;
   for(int i = 0; i < opt.num_bam; i++)
      if(buffer[i].size() == 0)
         comp++;

   // Find reads that are in common in all BAMs
   // and add the key to delVec
   map<CharString,int> countMap;
   vector<CharString> delVec;
   for(int i = 0; i < opt.num_bam; i++)
   {  
      if(buffer[i].size() < 1) continue;

      for(auto r = buffer[i].begin(); r != buffer[i].end(); r++)
      {
         countMap[r->first]++;
         if(countMap[r->first] == opt.num_bam - comp)
         {
            delVec.push_back(r->first);
         }
      }
   }

   // delete and count all common vectors
   for(auto d : delVec)
   {
      for(int i = 0; i < opt.num_bam; i++)
      {
         if(buffer[i].size() > 0)
         {
            auto it = buffer[i].find(d);
            buffer[i].erase(it->first) > 0;
         }
      }
   }

   // Prune algorithm;
   // For each bam buffer, find the smallest name
   // then of those, find the largest.
   // Finally, remove all names from all buffers that 
   // are smaller than that name.
   // Just before we delete, we need to do the counting
   vector<CharString> smallestVec;
   for(int i = 0; i < opt.num_bam; i++)
   {
      if(!buffer[i].empty())
      {
         smallestVec.push_back(buffer[i].begin()->first);
      }
   }
   sort(smallestVec.begin(), smallestVec.end());

   // record what we want
   

   // remove
   if(!smallestVec.empty())
   {
      for(int i = 0; i < opt.num_bam; i++)
      {
         int c = 0;
         for(auto it = buffer[i].begin(); it != buffer[i].end(); )
         {
            if(it->first < smallestVec.back())
            {
               c += buffer[i].erase(it->first);
               it++;
               break;
            }
            else
            {
               ++it;
            }
         }
      }
   }

   return 0;
}

int process(ModifyStringOptions &opt, BamFileIn *bamFiles, Stats &stats, 
            map<CharString, pair<BamAlignmentRecord, BamAlignmentRecord>> *buffer)
{
   int v[opt.num_bam];
   for(int i = 0; i < opt.num_bam; i++)
      v[i] = 0;

   int m = 0;
   int max_buf_size = 10;

   while(!emptyBAMs(opt, bamFiles))
   {
      pair<BamAlignmentRecord,BamAlignmentRecord> current[opt.num_bam];

      for(int i = 0; i < opt.num_bam; i++)
      {
         if(!atEnd(bamFiles[i]))
         {
            if(buffer[i].size() > max_buf_size)
            {
               for(int j = 0; j < opt.num_bam; j++)
                  if(i != j && buffer[i].size() == 0)
                     max_buf_size += 10; // if buffer is being weird
                  
               continue;
            }
            else if(!atEnd(bamFiles[i]))
            {
               v[i]++;

               BamAlignmentRecord record;
               readRecord(record, bamFiles[i]);

               if(hasFlagMultiple(record) && !atEnd(bamFiles[i]) && 
                  hasFlagFirst(record))
               {
                  buffer[i][record.qName].first = record;
                  readRecord(record, bamFiles[i]);
                  buffer[i][record.qName].second = record;
               }
               else if(hasFlagMultiple(record) && !atEnd(bamFiles[i]) &&
                       hasFlagLast(record))
               {
                  buffer[i][record.qName].second = record;
                  readRecord(record, bamFiles[i]);
                  buffer[i][record.qName].first = record;
               }
               else if(!hasFlagMultiple(record))
               {
                  buffer[i][record.qName].first = record;
               }
            }
            else if(atEnd(bamFiles[i]))
            {
               cout << "[COMPLETED] " << opt.inputFileName[i];
               cout << " has been read." << endl;
            }
         }
         m++;
      }

      compareRecords(opt, stats, buffer);

      if(m % 10000 == 0 && m > 0)
      {
         cout << "Completed : " << m << "\t";
         cout << "Reads : ";
         for(int i = 0; i < opt.num_bam; i++)
         {
            cout << v[i] << "\t";
         }
         cout << endl;
         cout << "Buffers : ";
         for(int i = 0; i < opt.num_bam; i++)
         {
            cout << buffer[i].size() << "\t";
         }
         cout << endl;
         //print(stats, opt);
      }
   }

   return 0;
}

int main(int argc, char const ** argv)
{
   // Parse our options
   ModifyStringOptions opt;
   ArgumentParser::ParseResult res = parseCommandLine(opt, argc, argv);

   if (res != ArgumentParser::PARSE_OK)
      return res == ArgumentParser::PARSE_ERROR;

   opt.num_bam = opt.inputFileName.size();

   if(opt.num_bam < 2)
   {
      cerr << "[ERROR] diffBAM requires more than one BAM file." << endl;
      return 1;
   }

   BamFileIn bamFiles[opt.num_bam];
   BamHeader bamHeaders[opt.num_bam];

   map<CharString, pair<BamAlignmentRecord, BamAlignmentRecord>> buffer[opt.num_bam];

   for(int i = 0; i < opt.num_bam; i++)
   {
      if(!open(bamFiles[i], toCString(opt.inputFileName[i])))
      {
         cerr << "[ERROR] Cannot read file ";
         cerr << opt.inputFileName[i] << endl;
         return 1;
      }
      else
      {
         readHeader(bamHeaders[i], bamFiles[i]);
      }
   }

   Stats stats;
   init(stats, opt);

   process(opt, bamFiles, stats, buffer);

   for(int i = 0; i < opt.num_bam; i++)
   {
      close(bamFiles[i]);
   }

   print(stats, opt);

   cout << "Buffer size : " << endl;
   for(int i = 0; i < opt.num_bam; i++)
   {
      cout << buffer[i].size() << " ";
   }
   cout << endl;

   return 0;
}
