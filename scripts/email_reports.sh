#!/bin/bash

sendMailTo="email@example.com"                                                                                                                                          
country="Canada"
excludeUser=""

yesterday_date=$(date "+%F" --date="yesterday")                                                                                                                      
access_log="/var/log/httpd/access_log"                                                                                                                                      
countryReport="/tmp/country_report_${yesterday_date}.htm"
summaryReport="/tmp/summary_report_${yesterday_date}.htm"

rm -f "$countryReport"
rm -f "$summaryReport"

lips -v -d ${yesterday_date} --html --exclude-user ${excludeUser} --country ${country} -O ${countryReport} -o ${summaryReport} "${access_log}"

if [ $? -ne 0 ]
then
	exit 1
fi

emailHeader="Content-Type: text/html\nMIME-Version: 1.0"

if [ -f "$summaryReport" ]
then
	report=$(cat "$summaryReport")
	echo -e "Subject: List of Countries Access to Your Website [$yesterday_date]\n${emailHeader}\n$report\n" | sendmail "$sendMailTo"
fi

if [ -f "$countryReport" ]
then
	report=$(cat "$countryReport")
	echo -e "Subject: Access to Your Website From $country [$yesterday_date]\n${emailHeader}\n$report\n" | sendmail "$sendMailTo"
fi

rm -f "$countryReport"
rm -f "$summaryReport"
