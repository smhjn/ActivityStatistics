#include <iostream>
#include <vector>
#include <string>
#include <boost\lexical_cast.hpp>
#include "CommandLineInterpreter.h"
#include "DataReader.h"
#include "DataStore.h"
#include <fstream>

#include "Report.h"

// would probably be a good idea to read the file URIs from command line ...
static const std::string inputDataFile = "example_input.txt";
static const std::string outputDataFile = "output.txt";

// open file and read space separated values and insert into data store
void ReadInputFile(const std::string& datafile)
{
	std::ifstream file(datafile);
	DataReader row;
	DataStore& dataStore = DataStore::GetInstance();
	while (file >> row)
	{
		try
		{
			dataStore.Insert(row.at(0), row.at(1), row.at(2));
		}
		catch(...)
		{
			// do nothing on exception but allow rest of data to ingested
		}
	}
}

int main(int argc, char** argv)
{
	try
	{
		// store required reports in vector
		std::vector<Report*> reports;

		// put build array out of command line args and process
		{
			std::vector<std::string> argVector(argv+1, argv + argc);
			if (argVector.empty())
			{
				throw std::runtime_error("invalide number of arguements (" + boost::lexical_cast<std::string>(argVector.size()) + ")\n");
			}
			CommandLine::ParseCommandLineInstructions(argVector, reports);
		}

		// read data file into store
		ReadInputFile(inputDataFile);

		// run reports
		std::string output;
		for (auto report : reports)
		{
			output += report->Generate() + "\n";
		}
		// output to standard out and file
		std::cout << output; 
		std::ofstream output_file(outputDataFile);
		std::ostream_iterator<char> output_iterator(output_file);
		std::copy(output.begin(), output.end(), output_iterator);
		output_file.close();	
    }
	catch (std::logic_error& ex)
	{
		std::cerr << "A systematic error has occured: " << ex.what() << "\nExiting...\n" << std::endl;
	}
	catch (std::runtime_error& ex)
	{
		std::cerr << "A runtime error has occured: " << ex.what() << "\nExiting...\n" << std::endl;
	}
	catch (std::exception& ex)
	{
		std::cerr << "A unexpected error has occured: " << ex.what() << "\nExiting...\n" << std::endl;
	}
	catch (...)
	{
		std::cerr << "An unexpected error occured: Exiting...\n" << std::endl;
	}

	return 0;
}