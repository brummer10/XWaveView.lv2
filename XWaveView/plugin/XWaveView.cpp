/*
 * Copyright (C) 2021 Hermann Meyer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */


#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <unistd.h>

////////////////////////////// LOCAL INCLUDES //////////////////////////

#include "XWaveView.h"        // define struct PortIndex

////////////////////////////// PLUG-IN CLASS ///////////////////////////

namespace osc {

class XWaveView
{
private:
    const LV2_Atom_Sequence* control;
    LV2_Atom_Sequence* notify;
    LV2_URID_Map* map;

    LV2_Atom_Forge forge;
    LV2_Atom_Sequence* notify_port;
    LV2_Atom_Forge_Frame notify_frame;
    OSCLV2URIs uris;
    // pointer to buffer
    float*          output;
    float*          input;

    // private functions
    inline void run_dsp_(uint32_t n_samples);
    inline void connect_(uint32_t port,void* data);
    inline void init_dsp_(uint32_t rate);
    inline void connect_all__ports(uint32_t port, void* data);

public:
    // LV2 Descriptor
    static const LV2_Descriptor descriptor;
    // static wrapper to private functions
    static void cleanup(LV2_Handle instance);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void connect_port(LV2_Handle instance, uint32_t port, void* data);
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor,
                                  double rate, const char* bundle_path,
                                  const LV2_Feature* const* features);
    XWaveView();
};

// constructor
XWaveView::XWaveView() :
    output(NULL),
    input(NULL) {};

///////////////////////// PRIVATE CLASS  FUNCTIONS /////////////////////

void XWaveView::init_dsp_(uint32_t rate)
{

}

// connect the Ports used by the plug-in class
void XWaveView::connect_(uint32_t port,void* data)
{
    switch ((PortIndex)port)
    {
    case EFFECTS_OUTPUT:
        output = static_cast<float*>(data);
        break;
    case EFFECTS_INPUT:
        input = static_cast<float*>(data);
        break;
    case NOTIFY:
        notify = (LV2_Atom_Sequence*)data;
        break;
    default:
        break;
    }
}

void XWaveView::run_dsp_(uint32_t n_samples)
{
    if (n_samples == 0) return;
    OSCLV2URIs* uris = &this->uris;
    const uint32_t notify_capacity = this->notify->atom.size;
    lv2_atom_forge_set_buffer(&forge, (uint8_t*)notify, notify_capacity);
    lv2_atom_forge_sequence_head(&forge, &notify_frame, 0);
    if (notify_capacity<n_samples) return;

    // do inplace processing at default
    if (output != input)
        memcpy(output, input, n_samples*sizeof(float));
    float buf[n_samples];
    memcpy(buf, input, n_samples*sizeof(float));
    LV2_Atom_Forge_Frame frame;
    lv2_atom_forge_frame_time(&this->forge, 0);
    lv2_atom_forge_object(&this->forge, &frame, 1, uris->atom_Float);
    lv2_atom_forge_property_head(&this->forge, uris->atom_Vector,0);
    lv2_atom_forge_vector(&this->forge, sizeof(float), uris->atom_Float, n_samples, (void*)buf);
    lv2_atom_forge_pop(&this->forge, &frame);

}

void XWaveView::connect_all__ports(uint32_t port, void* data)
{
    // connect the Ports used by the plug-in class
    connect_(port,data);
}

////////////////////// STATIC CLASS  FUNCTIONS  ////////////////////////

LV2_Handle
XWaveView::instantiate(const LV2_Descriptor* descriptor,
                       double rate, const char* bundle_path,
                       const LV2_Feature* const* features)
{
    LV2_URID_Map* map = NULL;
    for (int i = 0; features[i]; ++i) {
        if (!strcmp(features[i]->URI, LV2_URID__map)) {
            map = (LV2_URID_Map*)features[i]->data;
        }
    }
    if (!map) {
        return NULL;
    }

    // init the plug-in class
    XWaveView *self = new XWaveView();
    if (!self) {
        return NULL;
    }

    map_osclv2_uris(map, &self->uris);
    lv2_atom_forge_init(&self->forge, map);

    self->map = map;

    self->init_dsp_((uint32_t)rate);

    return (LV2_Handle)self;
}

void XWaveView::connect_port(LV2_Handle instance,
                             uint32_t port, void* data)
{
    // connect all ports
    static_cast<XWaveView*>(instance)->connect_all__ports(port, data);
}

void XWaveView::run(LV2_Handle instance, uint32_t n_samples)
{
    // run dsp
    static_cast<XWaveView*>(instance)->run_dsp_(n_samples);
}

void XWaveView::cleanup(LV2_Handle instance)
{
    // well, clean up after us
    XWaveView* self = static_cast<XWaveView*>(instance);
    delete self;
}

const LV2_Descriptor XWaveView::descriptor =
{
    PLUGIN_URI,
    XWaveView::instantiate,
    XWaveView::connect_port,
    NULL,
    XWaveView::run,
    NULL,
    XWaveView::cleanup,
    NULL
};


} // end namespace osc

////////////////////////// LV2 SYMBOL EXPORT ///////////////////////////

extern "C"
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
    switch (index)
    {
    case 0:
        return &osc::XWaveView::descriptor;
    default:
        return NULL;
    }
}

///////////////////////////// FIN //////////////////////////////////////
