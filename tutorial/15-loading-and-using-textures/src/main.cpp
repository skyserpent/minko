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

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Loading and using textures", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()		
		->queue("effect/Basic.effect")
		->queue("texture/my_texture.jpg")
		->options()->registerParser<file::JPEGParser>("jpg");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(PerspectiveCamera::create(
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, float(M_PI) * 0.25f, .1f, 1000.f)
			);
		root->addChild(camera);

		auto cube = scene::Node::create("cube")
		->addComponent(Transform::create(Matrix4x4::create()->translation(0.f, 0.f, -5.f)))
			->addComponent(Surface::create(
			geometry::CubeGeometry::create(sceneManager->assets()->context()),
			material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture("texture/my_texture.jpg")),
			sceneManager->assets()->effect("effect/Basic.effect")
			));
		root->addChild(cube);

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
		{
			cube->component<Transform>()->matrix()->prependRotationY(.01f);
			sceneManager->nextFrame(t, dt);
		});

		canvas->run();
	});

	sceneManager->assets()->loader()->load();

	return 0;
}
