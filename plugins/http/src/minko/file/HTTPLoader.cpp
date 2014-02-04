/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/file/HTTPLoader.hpp"

#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#else
#include "curl/curl.h"
#endif

using namespace minko;
using namespace minko::file;


struct MemoryStruct {
	char *memory;
	size_t size;
};

std::list<std::shared_ptr<HTTPLoader>>
HTTPLoader::_runningLoaders;

HTTPLoader::HTTPLoader()
{
}

void
HTTPLoader::completeHandler(void* arg, void* data, int size)
{
	std::cout << "HTTPLoader::completeHandler(): size: " << size << std::endl;
	auto iterator = std::find_if(HTTPLoader::_runningLoaders.begin(),
								 HTTPLoader::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPLoader> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPLoader::_runningLoaders.end())
	{
		std::cerr << "HTTPLoader::completeHandler(): cannot find loader" << std::endl;
		return;
	}

	std::cout << "HTTPLoader::completeHandler(): found loader" << std::endl;
	std::shared_ptr<HTTPLoader> loader = *iterator;

	std::cout << "HTTPLoader::completeHandler(): set data" << std::endl;
	loader->_data.assign(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + size);

	std::cout << "HTTPLoader::completeHandler(): call execute" << std::endl;
	loader->_complete->execute(loader);

	std::cout << "HTTPLoader::completeHandler(): remove loader" << std::endl;
	// HTTPLoader::_runningLoaders.remove(loader);
	std::cout << "HTTPLoader::completeHandler(): complete" << std::endl;
}

void
HTTPLoader::errorHandler(void* arg)
{
	std::cout << "HTTPLoader::errorHandler(): " << std::endl;
	auto iterator = std::find_if(HTTPLoader::_runningLoaders.begin(),
								 HTTPLoader::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPLoader> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPLoader::_runningLoaders.end())
	{
		std::cerr << "HTTPLoader::errorHandler(): cannot find loader" << std::endl;
		return;
	}

	std::cout << "HTTPLoader::errorHandler(): found loader" << std::endl;
	std::shared_ptr<HTTPLoader> loader = *iterator;

	std::cout << "HTTPLoader::errorHandler(): call execute" << std::endl;
	loader->_error->execute(loader);

	std::cout << "HTTPLoader::completeHandler(): remove loader" << std::endl;
	// HTTPLoader::_runningLoaders.remove(loader);
	std::cout << "HTTPLoader::errorHandler(): complete" << std::endl;
}

void
HTTPLoader::load(const std::string& filename, std::shared_ptr<Options> options)
{
	std::cout << "HTTPLoader::load(): " << filename << std::endl;
	_filename = filename;
    _resolvedFilename = options->uriFunction()(sanitizeFilename(filename));
	_options = options;
	
	auto loader = shared_from_this();

	_runningLoaders.push_back(std::static_pointer_cast<HTTPLoader>(loader));

	#if defined(EMSCRIPTEN)
	std::cout << "HTTPLoader::load(): " << "call emscripten_async_wget_data" << std::endl;
	emscripten_async_wget_data(_filename.c_str(), loader.get(), &completeHandler, &errorHandler);
	#else

	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();

	if (curl)
	{
		struct MemoryStruct mem;
		mem.memory = (char *)malloc(1);
		mem.size = 0; 

		curl_easy_setopt(curl, CURLOPT_URL, _filename.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteMemoryHandler);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&mem);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			errorHandler(loader.get());
		}
		else
		{
			curl_easy_cleanup(curl);
			completeHandler(loader.get(), mem.memory, mem.size);

			if (mem.memory)
				free(mem.memory);
		}
	}
	#endif
}


#ifndef EMSCRIPTEN
size_t
HTTPLoader::curlWriteMemoryHandler(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL)
		return 0;

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}
#endif
