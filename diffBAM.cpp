#include "diffBAM.h"

ArgumentParser::ParseResult parseCommandLine(ModifyStringOptions & options,
      int argc,
      char const ** argv)
{
   ArgumentParser parser("diffBAM");
   setVersion(parser, "0.0.1");
   setDate(parser, "November 2019");
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

//   getOptionValue(options.inputFileName, parser, "input-file");

   return ArgumentParser::PARSE_OK;
}

int main(int argc, char const ** argv)
{
   // Parse our options
   ModifyStringOptions options;
   ArgumentParser::ParseResult res = parseCommandLine(options, argc, argv);
   if (res != ArgumentParser::PARSE_OK)
      return res == ArgumentParser::PARSE_ERROR;

   if(options.inputFileName.size() < 2)
   {
      cerr << "[ERROR] diffBAM requires more than one BAM file." << endl;
      return 1;
   }

   for(int i = 0; i < options.inputFileName.size(); i++)
   {
      cout << options.inputFileName[i] << endl;
   }

   return 0;
}
