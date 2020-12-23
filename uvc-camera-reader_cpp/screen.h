#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <Godot.hpp>
#include <MeshInstance.hpp>
#include <ImageTexture.hpp>
#include <SpatialMaterial.hpp>

#include <libuvc/libuvc.h>

namespace godot{

class Screen : public MeshInstance{
	GODOT_CLASS(Screen, MeshInstance)

private:
	static const uint32_t WIDTH;
	static const uint32_t HEIGHT;

	uvc_context_t* ctx;
	uvc_device_t* dev;
	uvc_device_handle_t* devh;
	uvc_stream_ctrl_t ctrl;
	uvc_stream_handle_t* strmhp;
	uvc_frame_t* _frameUVC;
	uvc_error_t res;

	Ref<Image> _frameBuffer;
	Ref<ImageTexture> _texture;
	Ref<SpatialMaterial> _material;

public:
	static void _register_methods();

	Screen();
	~Screen();

	void _init(); // our initializer called by Godot
	void _ready();
	void _process(float delta);
};

}

#endif
