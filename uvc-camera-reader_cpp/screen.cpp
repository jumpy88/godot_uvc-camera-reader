#include "screen.h"

using namespace godot;

const uint32_t Screen::WIDTH = 640;
const uint32_t Screen::HEIGHT = 480;

void Screen::_register_methods(){
	register_method("_ready", &Screen::_ready);
	register_method("_process", &Screen::_process);
}

Screen::Screen(){
	res = uvc_init(&ctx, NULL);
	if(res < 0){
		uvc_perror(res, "uvc_init");
	}
	else{
		res = uvc_find_device(ctx, &dev, 0, 0, NULL);
		if(res < 0){
			uvc_perror(res, "uvc_find_device");
		}
		else{
			res = uvc_open(dev, &devh);
			if(res < 0){
				uvc_perror(res, "uvc_open");
			}
			else{
				res = uvc_get_stream_ctrl_format_size(devh, &ctrl, UVC_FRAME_FORMAT_YUYV, WIDTH, HEIGHT, 25);
				if(res < 0){
					uvc_perror(res, "get_mode");
				}
				else{
					uvc_stream_open_ctrl(devh, &strmhp, &ctrl);
					uvc_stream_start(strmhp, NULL, NULL, 0);
				}
			}
		}
	}
}

Screen::~Screen(){
	// add your cleanup here
	uvc_close(devh);
	uvc_unref_device(dev);
	uvc_exit(ctx);
}

void Screen::_init(){
	// initialize any variables here
	_frameBuffer = Ref<Image>(Image::_new());
	_frameBuffer->create(WIDTH, HEIGHT, true, Image::FORMAT_RGB8);
	_frameBuffer->fill(Color(0.54, 0.17, 0.89, 1));
	_frameBuffer->lock();
	for(int32_t i = 0; i < _frameBuffer->get_width(); ++i){
		_frameBuffer->set_pixel(i, 0, Color(1, 0, 0));
		_frameBuffer->set_pixel(i, 1, Color(1, 0, 0));
		_frameBuffer->set_pixel(i, 2, Color(1, 0, 0));
		_frameBuffer->set_pixel(i, HEIGHT - 1, Color(1, 0, 0));
		_frameBuffer->set_pixel(i, HEIGHT - 2, Color(1, 0, 0));
		_frameBuffer->set_pixel(i, HEIGHT - 3, Color(1, 0, 0));
	}
	for(int32_t i = 0; i < _frameBuffer->get_height(); ++i){
		_frameBuffer->set_pixel(0, i, Color(1, 0, 0));
		_frameBuffer->set_pixel(1, i, Color(1, 0, 0));
		_frameBuffer->set_pixel(2, i, Color(1, 0, 0));
		_frameBuffer->set_pixel(WIDTH - 1, i, Color(1, 0, 0));
		_frameBuffer->set_pixel(WIDTH - 2, i, Color(1, 0, 0));
		_frameBuffer->set_pixel(WIDTH - 3, i, Color(1, 0, 0));
	}
	_frameBuffer->unlock();

	_texture = Ref<ImageTexture>(ImageTexture::_new());
	_texture->create_from_image(_frameBuffer);

	_material = Ref<SpatialMaterial>(SpatialMaterial::_new());
	_material->set_texture(SpatialMaterial::TEXTURE_ALBEDO, _texture);

	set_surface_material(0, _material);
}

void Screen::_ready(){
}

void Screen::_process(float delta){
	(void)delta;
	uvc_stream_get_frame(strmhp, &_frameUVC, 0);
	uvc_frame_t* rgb;
	uvc_error_t ret;
	rgb = uvc_allocate_frame(_frameUVC->width * _frameUVC->height * 3);
	if(rgb){
		ret = uvc_any2rgb(_frameUVC, rgb);
		if(ret){
			uvc_perror(ret, "uvc_any2rgb");
			uvc_free_frame(rgb);
			rgb = nullptr;
		}
	}

	PoolByteArray buffer;
	buffer.resize(WIDTH * HEIGHT * 3);
	PoolByteArray::Write bufferWriter = buffer.write();
	memcpy(bufferWriter.ptr(), rgb->data, WIDTH * HEIGHT * 3);

	_frameBuffer->create_from_data(WIDTH, HEIGHT, false, Image::FORMAT_RGB8, buffer);
	_texture->set_data(_frameBuffer);

	if(rgb) uvc_free_frame(rgb);
}
