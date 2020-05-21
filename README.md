# LIPS
LogIpStatistics (LIPS) is a simple program to fetch IP address information from Apache access log. Basically you filter log file based on criteria like date, user and then `lips` fetch IP information from Multiple sources and generate html reports. For example:

```
$ lips -d 2020/05/20 --html --country Canada --country-output /tmp/canada.htm --output /tmp/summary.htm /var/log/httpd/access_log
```

Retrieves IP information for all items in the log for date 2020/05/20 and save a summary like country names and the time of visits into `/tmp/summary.htm`. Also a detailed information, such as user-agent, city, ISP, for all Canadian IPs are stored in `/tmp/canada.htm`. For more information run the following command:

```
$ lips --help
```

## Requirements

* [OpenSSL](https://github.com/openssl/openssl)
* [Curl](https://github.com/curl/curl)
* [Boost](https://www.boost.org/) components: date_time program_options property_tree
* [Cmake](https://cmake.org/)

The minimum requirement for Cmake is version 2.8. In RedHat like OS (CentOS, Fedora, ...) you can install them by the following commands:

With `dnf`:

```
$ sudo dnf install libcurl-devel openssl-devel boost-devel boost-date-time boost-program-options cmake
```
With `yum`:

```
$ sudo yum install libcurl-devel openssl-devel boost-devel boost-date-time boost-program-options cmake 
```

Note that if your `Cmake` version is less than 3.1, it's a little tricky to enable C++11. At the moment only GCC is supported. Feel free to tweak `cmake/EnableCpp11.cmake` according to your platform.

## How to Compile

After cloning the repository run the following commands: 

```
$ git submodule update --init
mkdir build && cd build
cmake ..
make
```
