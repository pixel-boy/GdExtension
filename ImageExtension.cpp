#include "ImageExtension.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/classes/curve.hpp>

using namespace godot;


PackedVector2Array ImageExtension::brush_to_image_with_pos(Ref<Image> image, Ref<Image> brush_image, Ref<Curve> resistance,float strength,float reduce_factor = 0.0) {
    PackedVector2Array change_pixel_array;
    PackedByteArray image_data = image->get_data();
    uint8_t* image_data_ptr = image_data.ptrw();
    PackedByteArray brush_image_data = brush_image->get_data();
    uint8_t* brush_data_ptr = brush_image_data.ptrw();
    int size = image_data.size();
    bool modif = false;
    for (int i = 0; i < size; ++i) {
        int current_value = image_data[i];
        if (current_value == 0){
            continue;
        }
        if (brush_data_ptr[i] == 0){
            continue;
        }
        int dig_value = (((brush_data_ptr[i]*5)*strength)/(1.0+((resistance->sample(1.0-(current_value/255.0)))*255)));
        if (dig_value == 0){
            brush_data_ptr[i] *= reduce_factor;
            modif = true;
            continue;
        }
        int new_value = MAX(current_value - dig_value,0);
        image_data_ptr[i] = new_value;
        change_pixel_array.append(Vector2(i,current_value-new_value));
        brush_data_ptr[i] *= reduce_factor;
        modif = true;
    }
    if (modif){
        image->set_data(image->get_width(),image->get_height(),false,image->get_format(),image_data);
    }
    brush_image->set_data(brush_image->get_width(),brush_image->get_height(),false,brush_image->get_format(),brush_image_data);
    return change_pixel_array;
}


PackedVector2Array ImageExtension::get_image_border(Ref<Image> image,int resize_factor) {
    PackedVector2Array image_border;
    PackedByteArray image_data = image->get_data();
    uint8_t* image_data_ptr = image_data.ptrw();
    
    int width = image->get_width();
    int height = image->get_height();
    int pitch = width * 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * pitch) + (x * 2); 
            uint8_t alpha = image_data_ptr[index + 1]; 
            if (alpha == 0) {
                continue;
            }
            if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
                uint8_t alpha_up = image_data_ptr[((y + 1) * pitch) + (x * 2) + 1];
                uint8_t alpha_down = image_data_ptr[((y - 1) * pitch) + (x * 2) + 1];
                uint8_t alpha_right = image_data_ptr[(y * pitch) + ((x + 1) * 2) + 1];
                uint8_t alpha_left = image_data_ptr[(y * pitch) + ((x - 1) * 2) + 1];
                if (alpha_up > 0 && alpha_down > 0 && alpha_right > 0 && alpha_left > 0) {
                    continue;
                }
            }
            image_border.append(Vector2(x, y)*resize_factor);
        }
    }
    return image_border;
}


void ImageExtension::add_brush_to_image(Ref<Image> image, Ref<Image> brush_image,float amount) {
    PackedByteArray image_data = image->get_data();
    uint8_t* image_data_ptr = image_data.ptrw();
    PackedByteArray brush_image_data = brush_image->get_data();
    uint8_t* brush_data_ptr = brush_image_data.ptrw();
    int size = image_data.size();
    bool modif = false;
    for (int i = 0; i < size; ++i) {
        int current_value = image_data[i];
        if (current_value >= 255){
            continue;
        }
        int add_value = brush_data_ptr[i]*amount;
        if (add_value == 0){
            continue;
        }
        image_data_ptr[i] = MIN(image_data_ptr[i]+add_value,255);
        modif = true;
    }
    if (modif){
        image->set_data(image->get_width(),image->get_height(),false,image->get_format(),image_data);
    }
}

void ImageExtension::convert_image_to_occluder(Ref<Image> image) {
    PackedByteArray image_data = image->get_data();
    int size = image_data.size();
    for (int i = 0; i < size/4; ++i) {
        int id = (i*4)+3;
        int alpha = image_data[id];
        if (alpha == 0){
            image_data[id-1] = 0;
            image_data[id-2] = 0;
            image_data[id-3] = 0;
            image_data[id] = 255;
        }else{
            image_data[id-1] = 255;
            image_data[id-2] = 255;
            image_data[id-3] = 255;
            image_data[id] = 255;
        }
    }
    image->set_data(image->get_width(),image->get_height(),false,image->get_format(),image_data);
}

float ImageExtension::occluder(Ref<Image> image, Ref<Image> brush_image) {
    PackedByteArray image_data = image->get_data();
    uint8_t* image_data_ptr = image_data.ptrw();
    PackedByteArray brush_image_data = brush_image->get_data();
    uint8_t* brush_data_ptr = brush_image_data.ptrw();
    int size = image_data.size();
    float pixel_count = 0;
    for (int i = 0; i < size; ++i) {
        int current_value = image_data[i];
        if (current_value == 0){
            continue;
        }
        if (brush_data_ptr[i] > 0){
            pixel_count += float(current_value)/255.0;
            brush_data_ptr[i] = 0;
        }
    }
    brush_image->set_data(brush_image->get_width(),brush_image->get_height(),false,brush_image->get_format(),brush_image_data);
    return pixel_count;
}



float ImageExtension::count_image_pixel(Ref<Image> image) {
    PackedByteArray image_data = image->get_data();
    uint8_t* image_data_ptr = image_data.ptrw();
    float sum = 0;
    for (int i = 0; i < image_data.size(); ++i) {
        sum += image_data_ptr[i]/255.0;
    }
    return sum;
}


ImageExtension::ImageExtension(){
    _bind_methods();
}


ImageExtension::~ImageExtension(){
    
}


void ImageExtension::_bind_methods(){
    ClassDB::bind_method(D_METHOD("convert_image_to_occluder"), &ImageExtension::convert_image_to_occluder);
    ClassDB::bind_method(D_METHOD("brush_to_image_with_pos"), &ImageExtension::brush_to_image_with_pos);
    ClassDB::bind_method(D_METHOD("add_brush_to_image"), &ImageExtension::add_brush_to_image);
    ClassDB::bind_method(D_METHOD("occluder"), &ImageExtension::occluder);
    ClassDB::bind_method(D_METHOD("count_image_pixel"), &ImageExtension::count_image_pixel);
    ClassDB::bind_method(D_METHOD("get_image_border"), &ImageExtension::get_image_border);
}
