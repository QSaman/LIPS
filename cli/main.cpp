#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <curlpp/cURLpp.hpp>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/date_time/gregorian/gregorian.hpp> 

#include <apache_access_log.hpp>
#include <cli_options.hpp>

namespace
{
	boost::program_options::options_description visible("Options");
}

void showHelp()
{

	const std::string intro = 
	"lips [Options] /path/to/log\n"
	"lips can have at most two types of reports:\n\n"
	"\t1. Summary reports: The list of countries with their frequency of\n"
	"\tvisits\n"
	"\t2. country reports: Reports for a specific country with a lot of\n"
	"\tfields including ISP and user agents\n\n"
	"For the first type of report:\n"
	"  lips -d log_date --html -o summary_report.htm access_log\n"
	"For the sconde type of report:\n"
	"  lips -d log_date --html -o summary.htm -c Canada -O canada.htm access_log\n";
	;
	std::cout << intro << visible << std::endl;
}

void parseCLI(int argc, const char* argv[])
{
	using namespace boost::program_options;
	using namespace boost::gregorian;

	options_description generalDesc("General Options");
	generalDesc.add_options()
		("date,d", value<std::string>()->value_name("[YYYY-MM-DD]"), "Get IP information for that specific date. The format should be [YYYY-MM-DD]")
		("from", value<std::string>()->value_name("[YYYY-MM-DD]"), "Get IP information --from to --to. The format should be [YYYY-MM-DD]")
		("to", value<std::string>()->value_name("[YYYY-MM-DD]"), "Get IP information --from to --to. The format should be [YYYY-MM-DD]")
		("exclude-user,u", value<ApacheAccessLog::UserList>()->value_name("[user]")->composing(), "Exclude entries with [user]. Note that you can use this option more than once. For example: -u foo -u bar -u baz")
		("verbose,v", "Increse verbosity. Useful for debugging")
		("help,h", "Show this help screen")
		;
	options_description countryDesc("Country Options");
	countryDesc.add_options()
		("country,c", value<std::string>()->value_name("[country_name]"), "Detailed report for the country [country_name]. By enabling this option, it's mandatory to provide --country-file. Only html reports are supported")
		("country-output,O", value<std::string>()->value_name("[file_name]"), "Path to [file_name] which the result of --country write into")
		;
	options_description summaryDesc("Summary Options");
	summaryDesc.add_options()
		("html,H", "Show result as html report.")
		("output,o", value<std::string>()->value_name("[output_file]"), "If specified, the output will be written into [output_file] instead of stdout")
		;
	options_description hiddenDesc("Hidden Options");
	hiddenDesc.add_options()
		("input-file,i", value<std::string>()->value_name("[file_name]"), "Path to the log file")
		;
	positional_options_description p;
	p.add("input-file", 1);

	variables_map vm;
	options_description cli;
	cli.add(generalDesc).add(summaryDesc).add(countryDesc).add(hiddenDesc);
	visible.add(generalDesc).add(summaryDesc).add(countryDesc);

	store(command_line_parser(argc, argv).options(cli).positional(p).run(), vm);
	notify(vm);

	if (vm.count("help"))
	{
		showHelp();
		std::exit(1);
	}

	verbose = vm.count("verbose");
	bool html = vm.count("html");

	std::string inputFileStr;
	if (vm.count("input-file"))
	{
		inputFileStr = vm["input-file"].as<std::string>();
	}
	else
	{
		std::cerr << "No input file is specified" << std::endl;
		showHelp();
		std::exit(1);
	}
	date start, end;
	if (vm.count("date"))
	{
		if (vm.count("to") || vm.count("from"))
		{
			std::cerr << "You must use --date or (--from and --to)" << std::endl;
			std::exit(1);
		}

		start = from_simple_string(vm["date"].as<std::string>());
		end = start;
	}
	if (vm.count("from") && vm.count("to"))
	{
		start = from_simple_string(vm["from"].as<std::string>());
		end   = from_simple_string(vm["to"].as<std::string>());
	}
	else if ((vm.count("from") > 0) ^ (vm.count("to") > 0))
	{
		std::cerr << "You must provide both --from and --to" << std::endl;
		std::exit(1);
	}

	ApacheAccessLog logAccess;

	const std::string countryFile = [&logAccess, &vm]()
	{
		std::string file;
		if (vm.count("country"))
		{
			logAccess.setCountry(vm["country"].as<std::string>());
			if (!vm.count("country-output"))
			{
				std::cerr << "You must provide --country-output" << std::endl;
				std::exit(1);
			}
			file = vm["country-output"].as<std::string>();
		}
		return file;
	}();

	if (verbose)
		std::cout << "The country is set to " << logAccess.country() << std::endl;

	const std::string summaryFile = [&vm]()
	{
		std::string file;
		if (vm.count("output"))
			file = vm["output"].as<std::string>();
		return file;
	}();

	ApacheAccessLog::UserList excludedUsers;
	if (vm.count("exclude-user"))
		excludedUsers = vm["exclude-user"].as<ApacheAccessLog::UserList>();
	logAccess.setExcludedUsers(excludedUsers);

	if (verbose)
		std::cout << "Reading \"" << inputFileStr << "\"" << std::endl;
	if (!logAccess.processFile(inputFileStr, start, end))
	{
		std::cerr << "Error in processing log file. Run with --verbose for more information" << std::endl;
		std::exit(2);
	}
	if (verbose)
		std::cout << "Fetch all IP information in \"" << inputFileStr << "\"" << std::endl;

	if (logAccess.size() == 0)
		return;
	const auto summaryReport = [&logAccess, html]()
	{
		std::string report;
		if (html)
			report = logAccess.getSummaryByCountryHtml();
		else
			report = logAccess.getSummaryByCountry();
		return report;
	}();

	if (summaryFile.empty())
		std::cout << summaryReport << std::flush;
	else
	{
		std::ofstream fout;
		fout.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fout.open(summaryFile);
		fout << summaryReport << std::flush;
	}

	const auto countryReport = logAccess.getItemsHtml();

	if (!countryReport.empty())
	{
		std::ofstream fout;
		fout.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fout.open(countryFile);
		fout << countryReport << std::flush;
	}
}

int main(int argc, const char* argv[])
{
	curlpp::initialize();
	try
	{
		parseCLI(argc, argv);
	}
	catch(const boost::program_options::error& error)
	{
		std::cerr << error.what() << std::endl;
	}
	curlpp::terminate();
}
