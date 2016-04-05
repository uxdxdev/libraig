// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.

#include "http_client.h"

#include <sstream>

//#include "curl/curl.h"

namespace http{

HttpDao::HttpDao()
{

}

void HttpDao::Create(std::string username, std::string password)
{
	/*
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	std::stringstream ss;
	ss << "text=" << username << "&" << "complete=" << password;
	std::string data = ss.str();

	if(curl) {
	// First set the URL that is about to receive our POST. This URL can just as well be a https:// URL if that is what should receive the data. 
	curl_easy_setopt(curl, CURLOPT_URL, "https://raig-dashboard.herokuapp.com/api/v1/raig");
	// Now specify the POST data 
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

	// Perform the request, res will get the return code 
	// res = curl_easy_perform(curl);
	// Check for errors 
	if(res != CURLE_OK)
	  fprintf(stderr, "curl_easy_perform() failed: %s\n",
			  curl_easy_strerror(res));

	// always cleanup 
	curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	*/
}

void HttpDao::Read()
{

}

void HttpDao::Update()
{

}

void HttpDao::Delete()
{

}

} // namespace http
