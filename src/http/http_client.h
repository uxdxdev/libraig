// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.
// https://github.com/damorton/libraig.git

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <memory>
#include <string>

namespace http{

class HttpDao {
public:
	HttpDao();
	void Create(std::string username, std::string password);
	void Read();
	void Update();
	void Delete();
};

} // namespace http

#endif
