#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <mutex>
#include <thread>
#include <cassert>
#include <sstream>


// Developer: Anthony Das
// Logging functionality designed to be thread-safe and computationally lightweight.
// The logger does not buffer log messages but flushes them immediately.
// Only C++ and STL functionality required, (i.e. no Boost libraries).


#define DEBUG_LOGGING 0

// Log messages come in four severities.
enum log_severity_type { info, validation, warning, error, debug };

#define LOG_INF logger::instance()->print<log_severity_type::info>
#define LOG_WAR logger::instance()->print<log_severity_type::warning>
#define LOG_VAL logger::instance()->print<log_severity_type::validation>
#define LOG_ERR logger::instance()->print<log_severity_type::error>
#define LOG_DGB logger::instance()->print<log_severity_type::debug>

class policy_interface {

public:

	virtual void open_out_ostream() = 0;

	virtual void close_out_ostream() = 0;

	virtual void write(const std::string &) = 0;

	virtual ~policy_interface() = 0;
};

policy_interface::~policy_interface() {}


// Log to standard out policy.
class standard_out_log_policy : public policy_interface {

public:

	virtual void open_out_ostream() {}

	virtual void close_out_ostream() {}

	virtual void write(const std::string &msg) {
		std::cout << msg << std::endl;
	}
	
	virtual ~standard_out_log_policy() {}
};


// Log to standard out and file policy.
class standard_out_and_file_log_policy : public standard_out_log_policy {

	std::ofstream out_stream;

	const std::string path;

public:

	standard_out_and_file_log_policy(const std::string path_to_set) : path(path_to_set) {}

	virtual void open_out_ostream() {
		out_stream.open(path.c_str());
		if (!out_stream.is_open()) {
			// Logging facility is crucial - fail processing
			throw(std::runtime_error("STANDARD_OUT_AND_FILE_LOG_POLICY: Unable to open file output stream"));
		}
	}

	virtual void close_out_ostream() {
		if (out_stream.is_open()) {
			out_stream.close();
		}
	}

	virtual void write(const std::string &msg) {
		out_stream << msg << std::endl;
		// std::cout << msg << std::endl;
		standard_out_log_policy::write(msg);
	}

	virtual ~standard_out_and_file_log_policy() {
		if (out_stream.is_open()) {
			close_out_ostream();
		}
	}
};


// Main log class. After creating an instance, always get the logger via
// the pointer logger::instance(). Infact this is neatly done by using the
// #defines: LOG_INF, LOG_WAR, LOG_VAL and LOG_ERR.
class logger {

	static logger *instance_ptr;

	unsigned int line_number;
	
	policy_interface &policy;
	
	std::mutex write_mutex;

public:

	logger(policy_interface &policy_to_set) : line_number(0), policy(policy_to_set) {
		policy.open_out_ostream();
		print<log_severity_type::info>("Opened log output stream");

		instance_ptr = this;
	}

	~logger() {
		print<log_severity_type::info>("Closing log output stream");
		policy.close_out_ostream();
	}

	// Rule of Five - as destructor present:

	// Copy constructor
	logger(const logger &to_copy) = delete;

	// Move constructor
	logger(logger &&to_move) = delete;

	// Assigment operator
	logger& operator= (const logger &to_assign) = delete;

	// Move assigment operator
	logger& operator= (logger &&to_move_assign) = delete;

	static logger *instance() {
		if (!instance_ptr) {
			// Logging facility is crucial - fail processing
			throw(std::runtime_error("LOGGER: You must create a logger instance before attempting to use it"));
		}
		return instance_ptr;
	}

	template<log_severity_type severity, typename... Args>
	void print(Args... args) {

		std::lock_guard<std::mutex> lck(write_mutex); // Ensure thread safety

		std::stringstream log_stream;

		// For performance list more probable log_severity_types first
		switch (severity) {
		case log_severity_type::info:
			log_stream << "INF:";
			break;
		case log_severity_type::validation:
			log_stream << "VAL:";
			break;
		case log_severity_type::warning:
			log_stream << "WAR:";
			break;
		case log_severity_type::error:
			log_stream << "ERR:";
			break;
#if (DEBUG_LOGGING)
		case log_severity_type::debug:
			log_stream << "DBG:";
			break;
#endif
		};

		print_impl(log_stream, args...);
	};

private:

	std::string get_time() {
		time_t raw_time;
		time(&raw_time);
		
		std::string time_str = ctime(&raw_time);

		return time_str.substr(0, time_str.length() - 1); // Remove newline character at end
	}
	

	std::string get_header() {
		std::stringstream header;
		header.fill('0');
		header.width(8);
		header << line_number++ << ",";
		header << std::this_thread::get_id() << ",";
		header << get_time() << ",";
		header.fill('0');
		header.width(7);
		header << clock() << ","; // Processing time
		return header.str();
	}


	// Core printing functionality
	// - has two bodies to access the parameters in the parameter pack.
	void print_impl(std::stringstream &log_stream) {
		policy.write(get_header() + log_stream.str());
		// log_stream.str(""); // Empty
	}

	template<typename First, typename... Rest>
	void print_impl(std::stringstream &log_stream, First parm1, Rest... parm) {
		log_stream << parm1 << " ";
		print_impl(log_stream, parm...);
	}
};

// Allocating and initializing logger's static data member.
// The pointer is being allocated - not the object inself.
logger* logger::instance_ptr = NULL;


#endif // LOGGER_H
