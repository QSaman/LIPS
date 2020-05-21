# LIPS
LogIpStatistics (LIPS) is a simple program to fetch IP address information from Apache access log. Basically you filter log file based on criteria like date, user and then `lips` fetch IP information from Multiple web services, such as [Free IP Geolocation API](https://freegeoip.app/) and [ip-api](https://ip-api.com/). Then it generates HTML reports. For example:

```
$ lips -d 2020/05/20 --html --country Canada --country-output /tmp/canada.htm \ 
       --output /tmp/summary.htm /var/log/httpd/access_log
```

Retrieves IP information for all items in the log for date 2020/05/20 and save a summary like country names and the time of visits into `/tmp/summary.htm`. Also a detailed information, such as user-agent, city, ISP, for all Canadian IPs are stored in `/tmp/canada.htm`. For more information run the following command:

```
$ lips --help
```

## Requirements

* [OpenSSL](https://github.com/openssl/openssl)
* [Curl](https://github.com/curl/curl)
* [Boost](https://www.boost.org/) components: `date_time`, `program_options`, `property_tree`
* [Cmake](https://cmake.org/)

The minimum requirement for Cmake is version 2.8. In RedHat-like OS (CentOS, Fedora, ...) you can install them by the following commands:

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

## Design

### Web Service Management
Since most web services limit the number of requests for a duration in time (minutes, hour, ...), I've written a simple timer which doesn't follow [Hollywood Principle](https://en.wikipedia.org/wiki/Inversion_of_control). It's a template class:

```
template<typename Duration, std::size_t MaxRequests>
class WebServiceTimer;
```

So you can customize it according to the web service. For example [Free IP Geolocation API](https://freegeoip.app/) allows 15,000 queries per hour. So we have:

```
static WebServiceTimer<Hours, 15000> _timer;
```

In this case the timer keeps track of the number of requests in an hour to make sure `lips` respect that restriction. After one hour, it reset the counter (number of requests).

By having a timer like this, it's easy to write another class to have a list of different web services, when `lips` reaches the limit for that web service, it tries the next available web service.

[Free IP Geolocation API](https://freegeoip.app/) doesn't support ISP name. So I've used [ip-api](https://ip-api.com/) which allow 45 queries per minute. It's easy to add other web services.

### Cache Management

Since the number of queries is limited, I implement a cache mechanism to avoid more than one query for the same IP address. At the moment it's a simple `std::onordered_map`. But it's easy to add a server-less database like [SQLite](https://www.sqlite.org/index.html)

## TODO

* Implement persistent cache management like [SQLite](https://www.sqlite.org/index.html)
* Use batch API for [ip-api](https://ip-api.com/docs/api:batch). You can send 100 queries in one HTTP request.
