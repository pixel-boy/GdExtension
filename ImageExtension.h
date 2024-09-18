#ifndef IMAGE_EXTENSION_H
#define IMAGE_EXTENSION_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/curve.hpp>

namespace godot {
    class ImageExtension : public Node {
            GDCLASS(ImageExtension, Node)
        protected:
            static void _bind_methods();

        public:
            ImageExtension(); // constructor
            ~ImageExtension(); // destructor
            PackedVector2Array brush_to_image_with_pos(Ref<Image> image,Ref<Image> brush_image,Ref<Curve> resistance,float strength,float reduce_factor);
            void convert_image_to_occluder(Ref<Image> image);
            float occluder(Ref<Image> image,Ref<Image> brush_image);
            void add_brush_to_image(Ref<Image> image,Ref<Image> brush_image,float amount);
            float count_image_pixel(Ref<Image> image);
            PackedVector2Array get_image_border(Ref<Image> image,int resize_factor);
    };
}

#endif