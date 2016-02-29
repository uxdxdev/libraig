/*

The MIT License (MIT)

Copyright (c) 2016 David Morton

https://github.com/damorton/libraig.git

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

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
