/*
Copyright (c) 2013 Aerys

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

#pragma once

#include "minko/Common.hpp"
#include "minko/file/Options.hpp"
#include "minko/AssetsLibrary.hpp"
#include "minko/deserialize/AssetsDeserializer.hpp"
#include "minko/deserialize/NameConverter.hpp"

namespace minko
{
	namespace file
	{
	class MkOptions
	{
	public:
		typedef std::shared_ptr<MkOptions> Ptr;

	private:
		std::shared_ptr<AssetsLibrary>						_assetsLibary;
		std::shared_ptr<Options>							_options;
		std::shared_ptr<deserialize::AssetsDeserializer>	_deserilizeAssets;
		std::shared_ptr<deserialize::NameConverter>			_nameConverter;

	public:
			unsigned int									_numMesh;

	public:
		inline static
		Ptr
		create(std::shared_ptr<file::Options>					options,
			   std::shared_ptr<AssetsLibrary>					assetLibrary)
		{
			return std::shared_ptr<MkOptions>(new MkOptions(options, assetLibrary));
		}

		inline
		std::shared_ptr<AssetsLibrary>
		assetsLibrary()
		{
			return _assetsLibary;
		}

		inline
		std::shared_ptr<deserialize::AssetsDeserializer>
		deserializedAssets()
		{
			return _deserilizeAssets;
		}

		inline
		void
		deserializedAssets(std::shared_ptr<deserialize::AssetsDeserializer> assets)
		{
			_deserilizeAssets = assets;
		}

		inline
		std::shared_ptr<Options>
		parseOptions()
		{
			return _options;
		}

		inline
		std::shared_ptr<deserialize::NameConverter>
		nameConverter()
		{
			return _nameConverter;
		}

	protected:
		MkOptions(std::shared_ptr<file::Options>				options,
			   std::shared_ptr<AssetsLibrary>					assetLibrary):
			_options(options),
			_assetsLibary(assetLibrary),
			_numMesh(0)
		{
			_nameConverter = deserialize::NameConverter::create();
		}
	};
	}
}
